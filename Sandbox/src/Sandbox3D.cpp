#include "Sandbox3D.h"
#include "LODTool.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <algorithm>
#include <exception>
#include <filesystem>
#include <fstream>
#include <sstream>

namespace
{
	std::string Trim(const std::string& value)
	{
		const auto first = value.find_first_not_of(" \t\r\n");
		if (first == std::string::npos)
			return "";
		const auto last = value.find_last_not_of(" \t\r\n");
		return value.substr(first, last - first + 1);
	}

	bool StartsWith(const std::string& value, const char* prefix)
	{
		return value.rfind(prefix, 0) == 0;
	}
}

Sandbox3D::Sandbox3D() :
	m_CameraController(
		true, (float)Hazel::DEFAULT_WINDOW_WIDTH / (float)Hazel::DEFAULT_WINDOW_HEIGHT
	), Layer("Sandbox3D")
{
	float aspectRatio = (float)Hazel::DEFAULT_WINDOW_WIDTH / (float)Hazel::DEFAULT_WINDOW_HEIGHT;
}

void Sandbox3D::OnAttach()
{
	m_Model = Hazel::CreateRef<Hazel::Model>(m_ModelPath);
	m_FallbackMaterial = Hazel::CreateRef<Hazel::Material>();
	ParseOriginalObjMaterialInfo();
	CaptureSourceMeshes();
	m_ModelVisible = false;
	m_LodDirty = true;
	m_LodStatus = "Model imported. Click Render to generate and display LOD mesh.";

	// light source

	m_DirectionalLight = Hazel::CreateRef<Hazel::DirectionalLight>(
		m_DirectionalLightProp.color, m_DirectionalLightProp.direction,
		m_DirectionalLightProp.ambient, m_DirectionalLightProp.diffuse, m_DirectionalLightProp.specular
	);


	float lightVertices[] = {
		-0.1f, -0.1f, -0.1f,
		 0.1f, -0.1f, -0.1f,
		 0.1f,  0.1f, -0.1f,
		 0.1f,  0.1f, -0.1f,
		-0.1f,  0.1f, -0.1f,
		-0.1f, -0.1f, -0.1f,

		-0.1f, -0.1f,  0.1f,
		 0.1f, -0.1f,  0.1f,
		 0.1f,  0.1f,  0.1f,
		 0.1f,  0.1f,  0.1f,
		-0.1f,  0.1f,  0.1f,
		-0.1f, -0.1f,  0.1f,

		-0.1f,  0.1f,  0.1f,
		-0.1f,  0.1f, -0.1f,
		-0.1f, -0.1f, -0.1f,
		-0.1f, -0.1f, -0.1f,
		-0.1f, -0.1f,  0.1f,
		-0.1f,  0.1f,  0.1f,

		 0.1f,  0.1f,  0.1f,
		 0.1f,  0.1f, -0.1f,
		 0.1f, -0.1f, -0.1f,
		 0.1f, -0.1f, -0.1f,
		 0.1f, -0.1f,  0.1f,
		 0.1f,  0.1f,  0.1f,

		-0.1f, -0.1f, -0.1f,
		 0.1f, -0.1f, -0.1f,
		 0.1f, -0.1f,  0.1f,
		 0.1f, -0.1f,  0.1f,
		-0.1f, -0.1f,  0.1f,
		-0.1f, -0.1f, -0.1f,

		-0.1f,  0.1f, -0.1f,
		 0.1f,  0.1f, -0.1f,
		 0.1f,  0.1f,  0.1f,
		 0.1f,  0.1f,  0.1f,
		-0.1f,  0.1f,  0.1f,
		-0.1f,  0.1f, -0.1f
	};
	Hazel::Ref<Hazel::VertexBuffer> pointLightVB = Hazel::VertexBuffer::Create(lightVertices, sizeof(lightVertices));
	pointLightVB->SetLayout({
		{ Hazel::ShaderDataType::Float3, "a_Position" }
		});
	// vertices array
	m_PointLightVA = Hazel::VertexArray::Create();
	m_PointLightVA->AddVertexBuffer(pointLightVB);

	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.7f,  0.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
	};

	for (int i = 0;i < 4;++i)
	{
		PointLightProps prop = PointLightProps();
		prop.position = pointLightPositions[i];
		m_PointLightProps.push_back(prop);

		m_PointLights.push_back(Hazel::CreateRef < Hazel::PointLight>(
			prop.color,prop.position,
			prop.ambient, prop.diffuse, prop.specular,
			prop.constant, prop.linear, prop.quadratic
		));
	}

	//spot light
	m_SpotLight = Hazel::CreateRef<Hazel::SpotLight>(
		m_SpotLightProp.color,m_CameraController.GetCamera().GetPosition(),-m_CameraController.GetCamera().GetZAxis(),
		m_SpotLightProp.ambient,m_SpotLightProp.diffuse,m_SpotLightProp.specular,
		m_SpotLightProp.constant,m_SpotLightProp.linear,m_SpotLightProp.quadratic,
		glm::cos(glm::radians(m_SpotLightProp.cutOff)), glm::cos(glm::radians(m_SpotLightProp.cutOff + m_SpotLightProp.epsilon))
	);


	// view phong illumination clearly
	m_CameraController.SetPosition({ 0.0f, 0.0f, 5.0f });
	//m_CameraController.SetRotation(glm::normalize(glm::quat(1.0f, 0.09f, 0.04f, 0.0f)));
}

void Sandbox3D::OnUpdate(Hazel::Timestep ts)
{
	m_CameraController.OnUpdate(ts);

	Hazel::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
	Hazel::RenderCommand::Clear();

	m_CameraController.SetPerspective(m_isPerspective);
	m_DirectionalLight->SetColor(m_DirectionalLightProp.color);
	m_DirectionalLight->SetDirection(m_DirectionalLightProp.direction);
	m_DirectionalLight->SetIntensity(
		m_DirectionalLightProp.ambient,
		m_DirectionalLightProp.diffuse,
		m_DirectionalLightProp.specular
	);

	for (int i = 0;i < m_PointLights.size();++i)
	{
		m_PointLights[i]->SetColor(m_PointLightProps[i].color);
		m_PointLights[i]->SetPosition(m_PointLightProps[i].position);
		m_PointLights[i]->SetIntensity(
			m_PointLightProps[i].ambient,
			m_PointLightProps[i].diffuse,
			m_PointLightProps[i].specular
		);
		m_PointLights[i]->SetAttenuation(
			m_PointLightProps[i].constant,
			m_PointLightProps[i].linear,
			m_PointLightProps[i].quadratic
		);
	}

	m_SpotLight->SetColor(m_SpotLightProp.color);
	m_SpotLight->SetPosition(m_CameraController.GetCamera().GetPosition());
	m_SpotLight->SetDirection(-m_CameraController.GetCamera().GetZAxis());
	m_SpotLight->SetIntensity(
		m_SpotLightProp.ambient, 
		m_SpotLightProp.diffuse, 
		m_SpotLightProp.specular
	);
	m_SpotLight->SetAttenuation(
		m_SpotLightProp.constant, 
		m_SpotLightProp.linear,
		m_SpotLightProp.quadratic
	);
	m_SpotLight->SetCutOffs(
		glm::cos(glm::radians(m_SpotLightProp.cutOff)), 
		glm::cos(glm::radians(m_SpotLightProp.cutOff + m_SpotLightProp.epsilon))
	);

	if (!m_ModelVisible || m_RenderMeshes.empty())
		return;

	Hazel::Renderer::BeginScene(m_CameraController.GetCamera(),
		m_DirectionalLight,m_PointLights,m_SpotLight);
	glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), m_ModelPos);
	modelMatrix = glm::scale(modelMatrix, glm::vec3(m_ModelScale));
	for (const auto& mesh : m_RenderMeshes)
		Hazel::Renderer::Submit(mesh, modelMatrix);

	for (int i = 0;i < m_PointLights.size();++i)
	{
		Hazel::Renderer::Submit(
			m_PointLightVA, m_PointLights[i],
			glm::translate(glm::mat4(1.0f), m_PointLightProps[i].position)
		);
	}
	Hazel::Renderer::EndScene();
}

void Sandbox3D::OnDetach()
{
	m_Model.reset();
	m_FallbackMaterial.reset();
	m_SourceMeshes.clear();
	m_CurrentLodMeshes.clear();
	m_RenderMeshes.clear();

	m_DirectionalLight.reset();

	m_PointLightVA.reset();
	m_PointLights.clear();
	m_PointLightProps.clear();
}

void Sandbox3D::OnImGuiRender()
{
	ImGui::Begin("LOD Simplification");
	ImGui::Text("Model Path: %s", m_ModelPath.c_str());
	ImGui::Text("Source Meshes: %d", (int)m_SourceMeshes.size());
	ImGui::Separator();

	bool lodChanged = false;
	lodChanged |= ImGui::SliderFloat("Vertex Keep (%)", &m_VertexKeepPercent, 0.0f, 100.0f, "%.1f");
	lodChanged |= ImGui::InputFloat("Vertex Keep Input", &m_VertexKeepPercent, 1.0f, 5.0f, "%.2f");
	m_VertexKeepPercent = std::clamp(m_VertexKeepPercent, 0.0f, 100.0f);
	if (lodChanged)
		MarkLodDirtyAndPause();

	if (ImGui::Button("Render"))
	{
		BuildLodMeshes(m_VertexKeepPercent);
		m_ModelVisible = !m_RenderMeshes.empty();
	}
	ImGui::SameLine();
	if (ImGui::Button("Pause"))
	{
		m_ModelVisible = false;
		m_LodStatus = "Rendering paused.";
	}

	ImGui::InputText("Export OBJ Path", m_ExportPathBuffer, sizeof(m_ExportPathBuffer));
	if (ImGui::Button("Export OBJ"))
		ExportCurrentLodObj(std::string(m_ExportPathBuffer));

	const uint64_t sourceVertices = CountTotalVertices(m_SourceMeshes);
	const uint64_t sourceTriangles = CountTotalTriangles(m_SourceMeshes);
	const uint64_t currentVertices = CountTotalVertices(m_CurrentLodMeshes);
	const uint64_t currentTriangles = CountTotalTriangles(m_CurrentLodMeshes);
	const float actualKeep = (sourceVertices > 0)
		? ((float)currentVertices * 100.0f / (float)sourceVertices)
		: 0.0f;

	ImGui::Separator();
	ImGui::Text("Original Vertices: %llu", (unsigned long long)sourceVertices);
	ImGui::Text("Original Triangles: %llu", (unsigned long long)sourceTriangles);
	ImGui::Text("Current Vertices: %llu", (unsigned long long)currentVertices);
	ImGui::Text("Current Triangles: %llu", (unsigned long long)currentTriangles);
	ImGui::Text("Applied Keep: %.1f%%", m_AppliedVertexKeepPercent);
	ImGui::Text("Actual Keep: %.2f%%", actualKeep);
	ImGui::TextWrapped("LOD Status: %s", m_LodStatus.c_str());
	ImGui::TextWrapped("Export Status: %s", m_ExportStatus.c_str());
	ImGui::End();

	ImGui::Begin("Model Settings");
	ImGui::DragFloat3("Model Position", glm::value_ptr(m_ModelPos), 0.1f);
	ImGui::SliderFloat("Model Scale", &m_ModelScale, 0.0f, 2.0f);
	ImGui::End();

	ImGui::Begin("Directional Settings");
	ImGui::DragFloat3("Direction", glm::value_ptr(m_DirectionalLightProp.direction), 0.1f);
	ImGui::SliderFloat3("Color", glm::value_ptr(m_DirectionalLightProp.color), 0.0f,1.0f);
	ImGui::SliderFloat3("Intensity (a/d/s)", &(m_DirectionalLightProp.ambient), 0.0f,1.0f);
	ImGui::End();
	
	ImGui::Begin("Point Lights");
	for (int i = 0;i < m_PointLights.size();++i)
	{
		std::string hint = std::to_string(i);
		ImGui::RadioButton(hint.c_str(), &m_PointLightActivated, i);
		if (i < m_PointLights.size() - 1)
			ImGui::SameLine();

	}
	ImGui::DragFloat3("Position", glm::value_ptr(m_PointLightProps[m_PointLightActivated].position), 0.1f);
	ImGui::SliderFloat3("Color", glm::value_ptr(m_PointLightProps[m_PointLightActivated].color), 0.0f, 1.0f);
	ImGui::SliderFloat3("Intensity (a/d/s)", &(m_PointLightProps[m_PointLightActivated].ambient), 0.0f, 1.0f);
	ImGui::SliderFloat3("Attenuation", &(m_PointLightProps[m_PointLightActivated].constant), 0.0f, 1.0f);
	ImGui::End();

	ImGui::Begin("Spot Light");
	ImGui::SliderFloat3("Color", glm::value_ptr(m_SpotLightProp.color), 0.0f, 1.0f);
	ImGui::SliderFloat3("Intensity (a/d/s)", &(m_SpotLightProp.ambient), 0.0f, 1.0f);
	ImGui::SliderFloat3("Attenuation", &(m_SpotLightProp.constant), 0.0f, 1.0f);
	ImGui::SliderFloat("Light CutOff", &m_SpotLightProp.cutOff, 0.0f, 90.0f);
	ImGui::SliderFloat("Light CutOff Epsilon", &m_SpotLightProp.epsilon, 0, 90.0f - m_SpotLightProp.cutOff);
	ImGui::End();

	ImGui::Begin("Camera Status");
	ImGui::Checkbox("Perspective", &m_isPerspective);
	// show the current camera state
	const glm::vec3& cameraPos = m_CameraController.GetCamera().GetPosition();
	ImGui::Text("Position: (%.1f, %.1f, %.1f)", cameraPos.x, cameraPos.y, cameraPos.z);
	ImGui::Text("AspectRatio: %.2f, Fovy: %.1f, Zoom: %.1f", m_CameraController.GetAspectRatio(), m_CameraController.GetFovy(), m_CameraController.GetZoomLevel());
	const glm::quat& cameraRotation = m_CameraController.GetCamera().GetRotation();
	ImGui::Text("Rotation: %.2f + %.2fi + %.2fj + %.2fk (Norm: %.2f)", cameraRotation.w, cameraRotation.x, cameraRotation.y, cameraRotation.z, glm::length(cameraRotation));
	const glm::vec3& cameraX = m_CameraController.GetCamera().GetXAxis();
	ImGui::Text("X: (%.2f, %.2f, %.2f)", cameraX.x, cameraX.y, cameraX.z);
	const glm::vec3& cameraY = m_CameraController.GetCamera().GetYAxis();
	ImGui::Text("Y: (%.2f, %.2f, %.2f)", cameraY.x, cameraY.y, cameraY.z);
	const glm::vec3& cameraZ = m_CameraController.GetCamera().GetZAxis();
	ImGui::Text("Z: (%.2f, %.2f, %.2f)", cameraZ.x, cameraZ.y, cameraZ.z);
	ImGui::End();
}

void Sandbox3D::OnEvent(Hazel::Event& e)
{
	m_CameraController.OnEvent(e);
}

void Sandbox3D::ParseOriginalObjMaterialInfo()
{
	m_OriginalMtlFileName.clear();
	m_OriginalUseMtlOrder.clear();

	std::ifstream input(m_ModelPath);
	if (!input.is_open())
		return;

	std::string line;
	while (std::getline(input, line))
	{
		if (m_OriginalMtlFileName.empty() && StartsWith(line, "mtllib "))
		{
			m_OriginalMtlFileName = Trim(line.substr(7));
			continue;
		}

		if (StartsWith(line, "usemtl "))
		{
			const std::string materialName = Trim(line.substr(7));
			if (materialName.empty())
				continue;
			if (m_OriginalUseMtlOrder.empty() || m_OriginalUseMtlOrder.back() != materialName)
				m_OriginalUseMtlOrder.push_back(materialName);
		}
	}
}

void Sandbox3D::CaptureSourceMeshes()
{
	m_SourceMeshes.clear();
	m_CurrentLodMeshes.clear();
	m_RenderMeshes.clear();

	if (!m_Model)
		return;

	const auto& meshes = m_Model->GetMeshes();
	m_SourceMeshes.reserve(meshes.size());
	for (size_t i = 0; i < meshes.size(); ++i)
	{
		const auto& mesh = meshes[i];
		MeshLODData meshData;
		meshData.Name = "mesh_" + std::to_string(i);
		if (i < m_OriginalUseMtlOrder.size())
			meshData.MaterialName = m_OriginalUseMtlOrder[i];
		else if (!m_OriginalUseMtlOrder.empty())
			meshData.MaterialName = m_OriginalUseMtlOrder.back();

		meshData.Material = mesh.GetMaterial() ? mesh.GetMaterial() : m_FallbackMaterial;
		meshData.Vertices = mesh.GetVertices();
		meshData.Indices = mesh.GetIndices();
		m_SourceMeshes.push_back(std::move(meshData));
	}

	m_CurrentLodMeshes = m_SourceMeshes;
}

void Sandbox3D::BuildLodMeshes(float keepPercent)
{
	m_RenderMeshes.clear();
	m_CurrentLodMeshes.clear();

	if (m_SourceMeshes.empty())
	{
		m_LodStatus = "No source mesh loaded.";
		return;
	}

	const float keepRatio = std::clamp(keepPercent / 100.0f, 0.0f, 1.0f);
	m_CurrentLodMeshes.reserve(m_SourceMeshes.size());
	int fallbackMeshCount = 0;

	for (const auto& source : m_SourceMeshes)
	{
		MeshLODData lodMesh;
		lodMesh.Name = source.Name;
		lodMesh.MaterialName = source.MaterialName;
		lodMesh.Material = source.Material ? source.Material : m_FallbackMaterial;

		if (keepRatio >= 0.999f)
		{
			lodMesh.Vertices = source.Vertices;
			lodMesh.Indices = source.Indices;
		}
		else
		{
			auto result = LODSimplifier::Simplify(source.Vertices, source.Indices, keepRatio);
			if (result.vertices.empty() || result.indices.size() < 3)
			{
				// Never drop a sub-mesh: fallback preserves closed appearance.
				lodMesh.Vertices = source.Vertices;
				lodMesh.Indices = source.Indices;
				++fallbackMeshCount;
			}
			else
			{
				lodMesh.Vertices = std::move(result.vertices);
				lodMesh.Indices = std::move(result.indices);
			}
		}

		m_CurrentLodMeshes.push_back(std::move(lodMesh));
		const auto& renderData = m_CurrentLodMeshes.back();
		m_RenderMeshes.emplace_back(renderData.Vertices, renderData.Indices, renderData.Material);
	}

	m_AppliedVertexKeepPercent = keepPercent;
	m_LodDirty = false;
	const uint64_t srcV = CountTotalVertices(m_SourceMeshes);
	const uint64_t curV = CountTotalVertices(m_CurrentLodMeshes);
	const float actualKeep = (srcV > 0) ? ((float)curV * 100.0f / (float)srcV) : 0.0f;

	std::ostringstream status;
	status << "Rendered with target " << keepPercent << "%, actual " << actualKeep << "%.";
	if (m_RenderMeshes.empty())
		status << " Result mesh is empty.";
	else
	{
		if (fallbackMeshCount > 0)
			status << " " << fallbackMeshCount << " mesh(es) kept original to avoid holes.";
		status << " Click Export OBJ to save current LOD.";
	}
	m_LodStatus = status.str();
}

bool Sandbox3D::ExportCurrentLodObj(const std::string& outputObjPath)
{
	if (m_LodDirty)
	{
		m_ExportStatus = "LOD parameters changed. Click Render before exporting.";
		return false;
	}

	if (m_CurrentLodMeshes.empty())
	{
		m_ExportStatus = "No LOD mesh available. Click Render first.";
		return false;
	}

	try
	{
		std::filesystem::path objPath(outputObjPath);
		if (!objPath.has_extension())
			objPath.replace_extension(".obj");
		if (objPath.has_parent_path())
			std::filesystem::create_directories(objPath.parent_path());

		std::string mtlFileForObj;
		if (!m_OriginalMtlFileName.empty())
		{
			const std::filesystem::path sourceMtl =
				std::filesystem::path(m_ModelPath).parent_path() / m_OriginalMtlFileName;
			if (std::filesystem::exists(sourceMtl))
			{
				const std::filesystem::path targetMtl = objPath.parent_path() / sourceMtl.filename();
				if (sourceMtl.lexically_normal() != targetMtl.lexically_normal())
				{
					std::filesystem::copy_file(
						sourceMtl,
						targetMtl,
						std::filesystem::copy_options::overwrite_existing
					);
				}
				mtlFileForObj = targetMtl.filename().string();
			}
		}

		std::vector<OBJExporter::SubMesh> subMeshes;
		subMeshes.reserve(m_CurrentLodMeshes.size());
		for (const auto& mesh : m_CurrentLodMeshes)
		{
			subMeshes.push_back({
				mesh.Name,
				mesh.MaterialName,
				&mesh.Vertices,
				&mesh.Indices
			});
		}

		const bool ok = OBJExporter::Export(objPath.string(), subMeshes, mtlFileForObj);
		m_ExportStatus = ok
			? "Exported: " + objPath.string()
			: "Export failed when writing OBJ file.";
		return ok;
	}
	catch (const std::exception& e)
	{
		m_ExportStatus = std::string("Export failed: ") + e.what();
		return false;
	}
}

void Sandbox3D::MarkLodDirtyAndPause()
{
	m_LodDirty = true;
	m_ModelVisible = false;
	m_LodStatus = "LOD parameter changed. Rendering paused. Click Render to apply.";
}

uint64_t Sandbox3D::CountTotalVertices(const std::vector<MeshLODData>& meshes)
{
	uint64_t total = 0;
	for (const auto& mesh : meshes)
		total += mesh.Vertices.size();
	return total;
}

uint64_t Sandbox3D::CountTotalTriangles(const std::vector<MeshLODData>& meshes)
{
	uint64_t total = 0;
	for (const auto& mesh : meshes)
		total += mesh.Indices.size() / 3;
	return total;
}