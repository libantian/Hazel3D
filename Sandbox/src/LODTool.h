#pragma once

// Mesh LOD Simplification using Quadric Error Metrics (Garland & Heckbert, 1997)
// OBJ export utility
// Requires: Hazel.h included before this header (provides Hazel::Vertex, glm)

#include <vector>
#include <array>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <algorithm>
#include <fstream>
#include <string>
#include <cmath>
#include <cassert>
#include <cstdint>

// ============================================================
//  LODSimplifier
//  Simplifies a triangle mesh by collapsing edges greedily,
//  always picking the collapse with the smallest quadric error.
// ============================================================
class LODSimplifier
{
public:
    struct MeshData
    {
        std::vector<Hazel::Vertex> vertices;
        std::vector<uint32_t>     indices;
    };

    // targetRatio : fraction of vertices to keep [0.0, 1.0]
    static MeshData Simplify(
        const std::vector<Hazel::Vertex>& inVerts,
        const std::vector<uint32_t>&      inIdx,
        float targetRatio)
    {
        targetRatio = std::max(0.0f, std::min(1.0f, targetRatio));

        auto safeNormalize = [](const glm::vec3& v, const glm::vec3& fallback = glm::vec3(0.0f, 1.0f, 0.0f))
        {
            float len = glm::length(v);
            if (len < 1e-12f) return fallback;
            return v / len;
        };

        int inputN = (int)inVerts.size();
        int inputF = (int)inIdx.size() / 3;
        if (inputN == 0 || inputF == 0)
            return { inVerts, inIdx };

        // Weld duplicated seam vertices by position so topology checks operate on
        // geometric connectivity instead of UV chart boundaries.
        constexpr float weldEps = 1e-6f;
        constexpr float invWeldEps = 1.0f / weldEps;

        struct WeldKey
        {
            int64_t x, y, z;
            bool operator==(const WeldKey& other) const
            {
                return x == other.x && y == other.y && z == other.z;
            }
        };
        struct WeldKeyHash
        {
            size_t operator()(const WeldKey& key) const
            {
                size_t h = std::hash<int64_t>()(key.x);
                h ^= std::hash<int64_t>()(key.y) + 0x9e3779b9 + (h << 6) + (h >> 2);
                h ^= std::hash<int64_t>()(key.z) + 0x9e3779b9 + (h << 6) + (h >> 2);
                return h;
            }
        };
        auto makeKey = [invWeldEps](const glm::vec3& p) -> WeldKey
        {
            return {
                (int64_t)std::llround((double)p.x * invWeldEps),
                (int64_t)std::llround((double)p.y * invWeldEps),
                (int64_t)std::llround((double)p.z * invWeldEps)
            };
        };

        std::unordered_map<WeldKey, std::vector<int>, WeldKeyHash> buckets;
        std::vector<Hazel::Vertex> weldedVerts;
        std::vector<int> weldedCount;
        std::vector<int> origToWeld(inVerts.size(), -1);
        weldedVerts.reserve(inVerts.size());
        weldedCount.reserve(inVerts.size());

        const float weldEps2 = weldEps * weldEps;
        for (size_t i = 0; i < inVerts.size(); ++i)
        {
            const auto& vin = inVerts[i];
            WeldKey key = makeKey(vin.Position);
            auto& bucket = buckets[key];

            int chosen = -1;
            for (int wi : bucket)
            {
                glm::vec3 d = weldedVerts[wi].Position - vin.Position;
                if (glm::dot(d, d) <= weldEps2)
                {
                    chosen = wi;
                    break;
                }
            }

            if (chosen < 0)
            {
                chosen = (int)weldedVerts.size();
                weldedVerts.push_back(vin);
                weldedCount.push_back(1);
                bucket.push_back(chosen);
            }
            else
            {
                weldedVerts[chosen].Normal += vin.Normal;
                weldedVerts[chosen].TexCoord += vin.TexCoord;
                weldedCount[chosen] += 1;
            }
            origToWeld[i] = chosen;
        }

        for (size_t i = 0; i < weldedVerts.size(); ++i)
        {
            if (weldedCount[i] > 1)
                weldedVerts[i].TexCoord /= (float)weldedCount[i];
            weldedVerts[i].Normal = safeNormalize(weldedVerts[i].Normal);
        }

        struct TriKey
        {
            int a, b, c;
            bool operator==(const TriKey& other) const
            {
                return a == other.a && b == other.b && c == other.c;
            }
        };
        struct TriKeyHash
        {
            size_t operator()(const TriKey& key) const
            {
                size_t h = std::hash<int>()(key.a);
                h ^= std::hash<int>()(key.b) + 0x9e3779b9 + (h << 6) + (h >> 2);
                h ^= std::hash<int>()(key.c) + 0x9e3779b9 + (h << 6) + (h >> 2);
                return h;
            }
        };
        auto makeTriKey = [](int i0, int i1, int i2) -> TriKey
        {
            int a = i0, b = i1, c = i2;
            if (a > b) std::swap(a, b);
            if (b > c) std::swap(b, c);
            if (a > b) std::swap(a, b);
            return { a, b, c };
        };

        std::vector<uint32_t> weldedIdx;
        weldedIdx.reserve(inIdx.size());
        std::unordered_set<TriKey, TriKeyHash> uniqueFaces;
        for (size_t i = 0; i + 2 < inIdx.size(); i += 3)
        {
            int i0 = origToWeld[inIdx[i]];
            int i1 = origToWeld[inIdx[i + 1]];
            int i2 = origToWeld[inIdx[i + 2]];
            if (i0 == i1 || i1 == i2 || i0 == i2)
                continue;

            TriKey key = makeTriKey(i0, i1, i2);
            if (!uniqueFaces.insert(key).second)
                continue;

            weldedIdx.push_back((uint32_t)i0);
            weldedIdx.push_back((uint32_t)i1);
            weldedIdx.push_back((uint32_t)i2);
        }

        if (weldedVerts.empty() || weldedIdx.size() < 3)
            return { inVerts, inIdx };

        const std::vector<Hazel::Vertex>& sourceVerts = weldedVerts;
        const std::vector<uint32_t>& sourceIdx = weldedIdx;
        int N = (int)sourceVerts.size();
        int F = (int)sourceIdx.size() / 3;

        int minTarget = (N >= 4) ? 4 : N;
        int targetN = std::max(minTarget, (int)(N * targetRatio));
        if (targetRatio <= 0.0f)
            targetN = minTarget;

        if (targetN >= N)
            return { inVerts, inIdx };

        // ------- working buffers (CPU only) -------
        std::vector<glm::vec3> pos(N), nor(N);
        std::vector<glm::vec2> uv(N);
        for (int i = 0; i < N; ++i)
        {
            pos[i] = sourceVerts[i].Position;
            nor[i] = sourceVerts[i].Normal;
            uv[i]  = sourceVerts[i].TexCoord;
        }

        // Each face stored as 3 ints (-1 means deleted slot – we use fDel instead)
        std::vector<std::array<int, 3>> faces(F);
        for (int i = 0; i < F; ++i)
            faces[i] = { (int)sourceIdx[i * 3], (int)sourceIdx[i * 3 + 1], (int)sourceIdx[i * 3 + 2] };

        std::vector<bool> fDel(F, false);
        std::vector<bool> vDel(N, false);
        int aliveV = N;

        // vertex → adjacent faces
        std::vector<std::vector<int>> v2f(N);
        for (int f = 0; f < F; ++f)
            for (int k = 0; k < 3; ++k)
                v2f[faces[f][k]].push_back(f);

        // ------- per-vertex quadric matrices -------
        // Q stored as 10 upper-triangle elements of a 4×4 symmetric matrix
        // Q = Σ (plane^T · plane)  where plane = (a,b,c,d), ax+by+cz+d=0
        std::vector<std::array<double, 10>> Q(N);
        for (auto& q : Q) q.fill(0.0);

        auto addPlane = [&](int vi, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2)
        {
            glm::vec3 n = glm::cross(p1 - p0, p2 - p0);
            float len = glm::length(n);
            if (len < 1e-10f) return;
            n /= len;
            double a = n.x, b = n.y, c = n.z, d = -(double)glm::dot(n, p0);
            auto& q = Q[vi];
            q[0] += a*a; q[1] += a*b; q[2] += a*c; q[3] += a*d;
            q[4] += b*b; q[5] += b*c; q[6] += b*d;
            q[7] += c*c; q[8] += c*d; q[9] += d*d;
        };

        for (int f = 0; f < F; ++f)
        {
            auto [i0, i1, i2] = faces[f];
            addPlane(i0, pos[i0], pos[i1], pos[i2]);
            addPlane(i1, pos[i0], pos[i1], pos[i2]);
            addPlane(i2, pos[i0], pos[i1], pos[i2]);
        }

        // ------- helpers -------
        auto qAdd = [](const std::array<double,10>& a,
                       const std::array<double,10>& b) -> std::array<double,10>
        {
            std::array<double,10> r;
            for (int i = 0; i < 10; ++i) r[i] = a[i] + b[i];
            return r;
        };

        auto qEval = [](const std::array<double,10>& q, const glm::vec3& v) -> double
        {
            double x = v.x, y = v.y, z = v.z;
            return q[0]*x*x + 2*q[1]*x*y + 2*q[2]*x*z + 2*q[3]*x
                 + q[4]*y*y + 2*q[5]*y*z + 2*q[6]*y
                 + q[7]*z*z + 2*q[8]*z + q[9];
        };

        // Solve A·v = b via Gaussian elimination with partial pivoting
        // A = [[q0,q1,q2],[q1,q4,q5],[q2,q5,q7]], b = [-q3,-q6,-q8]
        auto qOptPos = [](const std::array<double,10>& q, glm::vec3& out) -> bool
        {
            double A[3][3] = {
                { q[0], q[1], q[2] },
                { q[1], q[4], q[5] },
                { q[2], q[5], q[7] }
            };
            double b[3] = { -q[3], -q[6], -q[8] };
            for (int col = 0; col < 3; ++col)
            {
                int pivot = col;
                for (int row = col + 1; row < 3; ++row)
                    if (std::fabs(A[row][col]) > std::fabs(A[pivot][col]))
                        pivot = row;
                std::swap(A[col], A[pivot]);
                std::swap(b[col], b[pivot]);
                if (std::fabs(A[col][col]) < 1e-12) return false;
                for (int row = col + 1; row < 3; ++row)
                {
                    double f = A[row][col] / A[col][col];
                    for (int k = col; k < 3; ++k) A[row][k] -= f * A[col][k];
                    b[row] -= f * b[col];
                }
            }
            double z = b[2] / A[2][2];
            double y = (b[1] - A[1][2]*z) / A[1][1];
            double x = (b[0] - A[0][1]*y - A[0][2]*z) / A[0][0];
            out = glm::vec3((float)x, (float)y, (float)z);
            return true;
        };

        // Unique key for an undirected edge (a, b), a ≤ b
        auto edgeKey = [](int a, int b) -> long long
        {
            if (a > b) std::swap(a, b);
            return ((long long)(unsigned int)a << 32) | (unsigned int)b;
        };

        // ------- priority queue of edge collapses -------
        struct EdgeCollapse
        {
            int v1, v2;          // v1 < v2
            double cost;
            glm::vec3 newPos;
            int version;
            bool operator>(const EdgeCollapse& o) const { return cost > o.cost; }
        };

        auto faceHasVertex = [&](const std::array<int, 3>& f, int v) -> bool
        {
            return f[0] == v || f[1] == v || f[2] == v;
        };

        auto faceHasEdge = [&](const std::array<int, 3>& f, int a, int b) -> bool
        {
            int hit = 0;
            if (f[0] == a || f[0] == b) ++hit;
            if (f[1] == a || f[1] == b) ++hit;
            if (f[2] == a || f[2] == b) ++hit;
            return hit == 2;
        };

        auto gatherNeighbors = [&](int v, int ignore, std::unordered_set<int>& out)
        {
            out.clear();
            std::unordered_set<int> visitedFaces;
            for (int f : v2f[v])
            {
                if (f < 0 || f >= F) continue;
                if (!visitedFaces.insert(f).second) continue;
                if (fDel[f]) continue;
                const auto& tri = faces[f];
                if (!faceHasVertex(tri, v)) continue;
                for (int k = 0; k < 3; ++k)
                {
                    int vi = tri[k];
                    if (vi == v || vi == ignore || vDel[vi]) continue;
                    out.insert(vi);
                }
            }
        };

        auto isBoundaryVertex = [&](int v) -> bool
        {
            std::unordered_map<long long, int> incidentEdgeCount;
            std::unordered_set<int> visitedFaces;
            for (int f : v2f[v])
            {
                if (f < 0 || f >= F) continue;
                if (!visitedFaces.insert(f).second) continue;
                if (fDel[f]) continue;
                const auto& tri = faces[f];
                if (!faceHasVertex(tri, v)) continue;

                int idx = -1;
                for (int k = 0; k < 3; ++k)
                    if (tri[k] == v) { idx = k; break; }
                if (idx < 0) continue;

                int a = tri[(idx + 1) % 3];
                int b = tri[(idx + 2) % 3];
                if (!vDel[a]) incidentEdgeCount[edgeKey(v, a)]++;
                if (!vDel[b]) incidentEdgeCount[edgeKey(v, b)]++;
            }

            for (const auto& [k, cnt] : incidentEdgeCount)
                if (cnt == 1) return true;
            return false;
        };

        auto topologicalCheck = [&](int a, int b) -> bool
        {
            if (a == b || vDel[a] || vDel[b]) return false;

            std::unordered_set<int> commonFaces;
            std::unordered_set<int> seenFaces;
            for (int f : v2f[a])
            {
                if (f < 0 || f >= F) continue;
                if (!seenFaces.insert(f).second) continue;
                if (fDel[f]) continue;
                const auto& tri = faces[f];
                if (faceHasEdge(tri, a, b))
                    commonFaces.insert(f);
            }

            const int commonFaceCount = (int)commonFaces.size();
            if (commonFaceCount < 1 || commonFaceCount > 2) return false;

            const bool boundaryA = isBoundaryVertex(a);
            const bool boundaryB = isBoundaryVertex(b);
            if (boundaryA != boundaryB) return false;
            if (boundaryA && commonFaceCount != 1) return false;

            std::unordered_set<int> na, nb;
            gatherNeighbors(a, b, na);
            gatherNeighbors(b, a, nb);

            int commonNeighborCount = 0;
            for (int v : na)
                if (nb.find(v) != nb.end())
                    ++commonNeighborCount;

            const int requiredCommon = (commonFaceCount == 1) ? 1 : 2;
            if (commonNeighborCount != requiredCommon) return false;

            return true;
        };

        auto geometricCheck = [&](int vKeep, int vLose, const glm::vec3& newPos) -> bool
        {
            std::unordered_set<int> affected;
            for (int f : v2f[vKeep]) affected.insert(f);
            for (int f : v2f[vLose]) affected.insert(f);

            constexpr float minArea = 1e-12f;
            constexpr float minNormalDot = 0.25f;
            std::unordered_set<TriKey, TriKeyHash> localFaces;

            for (int f : affected)
            {
                if (f < 0 || f >= F) continue;
                if (fDel[f]) continue;

                const auto& tri = faces[f];
                if (!faceHasVertex(tri, vKeep) && !faceHasVertex(tri, vLose))
                    continue;

                int oldIdx[3] = { tri[0], tri[1], tri[2] };
                int newIdx[3] = { tri[0], tri[1], tri[2] };
                for (int k = 0; k < 3; ++k)
                    if (newIdx[k] == vLose) newIdx[k] = vKeep;

                if (newIdx[0] == newIdx[1] || newIdx[1] == newIdx[2] || newIdx[0] == newIdx[2])
                    continue;

                TriKey tkey = makeTriKey(newIdx[0], newIdx[1], newIdx[2]);
                if (!localFaces.insert(tkey).second)
                    return false;

                glm::vec3 oldP[3] = { pos[oldIdx[0]], pos[oldIdx[1]], pos[oldIdx[2]] };
                glm::vec3 newP[3] = { pos[newIdx[0]], pos[newIdx[1]], pos[newIdx[2]] };
                for (int k = 0; k < 3; ++k)
                    if (newIdx[k] == vKeep) newP[k] = newPos;

                glm::vec3 oldN = glm::cross(oldP[1] - oldP[0], oldP[2] - oldP[0]);
                glm::vec3 newN = glm::cross(newP[1] - newP[0], newP[2] - newP[0]);
                float oldL = glm::length(oldN);
                float newL = glm::length(newN);

                if (newL < minArea) return false;
                if (oldL > minArea)
                {
                    float d = glm::dot(oldN / oldL, newN / newL);
                    if (d < minNormalDot) return false;
                }
            }
            return true;
        };

        std::unordered_map<long long, int> edgeVer;
        std::priority_queue<EdgeCollapse,
            std::vector<EdgeCollapse>,
            std::greater<EdgeCollapse>> pq;

        auto pushEdge = [&](int a, int b)
        {
            if (a > b) std::swap(a, b);
            long long k   = edgeKey(a, b);
            auto qe       = qAdd(Q[a], Q[b]);
            glm::vec3 np  = (pos[a] + pos[b]) * 0.5f;  // default: midpoint
            double bestC  = qEval(qe, np);

            glm::vec3 opt;
            if (qOptPos(qe, opt))
            {
                double c = qEval(qe, opt);
                if (c < bestC) { bestC = c; np = opt; }
            }
            for (auto& p : { pos[a], pos[b] })
            {
                double c = qEval(qe, p);
                if (c < bestC) { bestC = c; np = p; }
            }

            int ver = ++edgeVer[k];
            pq.push({ a, b, bestC, np, ver });
        };

        // seed the queue with all unique edges
        std::unordered_set<long long> seen;
        for (int f = 0; f < F; ++f)
        {
            for (int k = 0; k < 3; ++k)
            {
                int a = faces[f][k], b = faces[f][(k+1)%3];
                long long ek = edgeKey(a, b);
                if (seen.insert(ek).second)
                    pushEdge(a, b);
            }
        }

        // ------- greedy edge collapses -------
        while (aliveV > targetN && !pq.empty())
        {
            auto ec = pq.top();  pq.pop();
            if (vDel[ec.v1] || vDel[ec.v2]) continue;

            long long k = edgeKey(ec.v1, ec.v2);
            auto it = edgeVer.find(k);
            if (it == edgeVer.end() || it->second != ec.version) continue;

            if (!topologicalCheck(ec.v1, ec.v2)) continue;

            int vKeep = ec.v1, vLose = ec.v2;
            bool valid12 = geometricCheck(ec.v1, ec.v2, ec.newPos);
            bool valid21 = geometricCheck(ec.v2, ec.v1, ec.newPos);
            if (!valid12 && !valid21) continue;
            if (!valid12 && valid21)
            {
                vKeep = ec.v2;
                vLose = ec.v1;
            }

            pos[vKeep] = ec.newPos;
            nor[vKeep] = safeNormalize(nor[vKeep] + nor[vLose], nor[vKeep]);
            uv[vKeep]  = (uv[vKeep] + uv[vLose]) * 0.5f;
            Q[vKeep]   = qAdd(Q[vKeep], Q[vLose]);

            vDel[vLose] = true;
            --aliveV;

            // remap vLose → vKeep in all adjacent faces
            std::unordered_set<int> neighbors;
            for (int f : v2f[vLose])
            {
                if (fDel[f]) continue;
                for (int& vi : faces[f])
                    if (vi == vLose) vi = vKeep;

                if (faces[f][0] == faces[f][1] ||
                    faces[f][1] == faces[f][2] ||
                    faces[f][0] == faces[f][2])
                {
                    fDel[f] = true;
                }
                else
                {
                    v2f[vKeep].push_back(f);
                    for (int vi : faces[f])
                        if (vi != vKeep) neighbors.insert(vi);
                }
            }
            v2f[vLose].clear();

            for (int f : v2f[vKeep])
            {
                if (f < 0 || f >= F) continue;
                if (fDel[f]) continue;
                for (int vi : faces[f])
                    if (vi != vKeep && !vDel[vi]) neighbors.insert(vi);
            }

            for (int nb : neighbors)
                if (!vDel[nb])
                    pushEdge(vKeep, nb);
        }

        // ------- rebuild output -------
        std::vector<int> remap(N, -1);
        std::vector<Hazel::Vertex> outV;
        outV.reserve(aliveV);
        for (int i = 0; i < N; ++i)
        {
            if (!vDel[i])
            {
                remap[i] = (int)outV.size();
                outV.push_back({ pos[i], safeNormalize(nor[i]), uv[i] });
            }
        }

        std::vector<uint32_t> outI;
        outI.reserve(F * 3);
        for (int f = 0; f < F; ++f)
        {
            if (fDel[f]) continue;
            int i0 = remap[faces[f][0]];
            int i1 = remap[faces[f][1]];
            int i2 = remap[faces[f][2]];
            if (i0 < 0 || i1 < 0 || i2 < 0) continue;
            if (i0 == i1 || i1 == i2 || i0 == i2) continue;
            outI.push_back((uint32_t)i0);
            outI.push_back((uint32_t)i1);
            outI.push_back((uint32_t)i2);
        }

        // Rebuild smooth normals from simplified triangles to avoid lighting artifacts.
        if (!outV.empty() && outI.size() >= 3)
        {
            std::vector<glm::vec3> accum(outV.size(), glm::vec3(0.0f));
            for (size_t i = 0; i + 2 < outI.size(); i += 3)
            {
                uint32_t i0 = outI[i];
                uint32_t i1 = outI[i + 1];
                uint32_t i2 = outI[i + 2];
                const glm::vec3& p0 = outV[i0].Position;
                const glm::vec3& p1 = outV[i1].Position;
                const glm::vec3& p2 = outV[i2].Position;
                glm::vec3 fn = glm::cross(p1 - p0, p2 - p0);
                if (glm::length(fn) > 1e-12f)
                {
                    accum[i0] += fn;
                    accum[i1] += fn;
                    accum[i2] += fn;
                }
            }
            for (size_t i = 0; i < outV.size(); ++i)
                outV[i].Normal = safeNormalize(accum[i], outV[i].Normal);
        }

        return { outV, outI };
    }
};


// ============================================================
//  OBJExporter
//  Writes simplified mesh data to a Wavefront .obj file.
//  Vertices, normals and UVs share the same index (v/vt/vn = i/i/i).
// ============================================================
class OBJExporter
{
public:
    struct SubMesh
    {
        std::string name;
        std::string materialName;
        const std::vector<Hazel::Vertex>*  vertices = nullptr;
        const std::vector<uint32_t>*       indices  = nullptr;
    };

    // path    : full output path, e.g. "C:/out/model_lod.obj"
    // meshes  : one or more sub-meshes to export
    // mtlFile : optional MTL filename to reference (just the filename, not full path)
    static bool Export(
        const std::string&        path,
        const std::vector<SubMesh>& meshes,
        const std::string&        mtlFile = "")
    {
        std::ofstream f(path);
        if (!f.is_open()) return false;

        f << "# Exported by Hazel LOD Tool\n";
        if (!mtlFile.empty())
            f << "mtllib " << mtlFile << "\n";
        f << "\n";

        int vOffset = 0;
        for (const auto& m : meshes)
        {
            if (!m.vertices || !m.indices) continue;
            f << "o " << (m.name.empty() ? "mesh" : m.name) << "\n";
            if (!m.materialName.empty())
                f << "usemtl " << m.materialName << "\n";

            for (const auto& v : *m.vertices)
                f << "v "  << v.Position.x << " " << v.Position.y << " " << v.Position.z << "\n";
            for (const auto& v : *m.vertices)
                f << "vt " << v.TexCoord.x << " " << v.TexCoord.y << "\n";
            for (const auto& v : *m.vertices)
                f << "vn " << v.Normal.x   << " " << v.Normal.y   << " " << v.Normal.z   << "\n";

            f << "s off\n";

            for (size_t i = 0; i + 2 < m.indices->size(); i += 3)
            {
                int a = (int)(*m.indices)[i]     + vOffset + 1;
                int b = (int)(*m.indices)[i + 1] + vOffset + 1;
                int c = (int)(*m.indices)[i + 2] + vOffset + 1;
                f << "f " << a << "/" << a << "/" << a
                  << " "  << b << "/" << b << "/" << b
                  << " "  << c << "/" << c << "/" << c << "\n";
            }
            f << "\n";
            vOffset += (int)m.vertices->size();
        }
        return f.good();
    }
};
