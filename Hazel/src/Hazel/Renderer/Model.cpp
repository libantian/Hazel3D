#include "hzpch.h"
#include "Model.h"

#include "Hazel/Core/AssetsManager.h"
#include <filesystem>

namespace Hazel {

	void Model::LoadModel(const std::string& path)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			HZ_CORE_ERROR("ERROR::ASSIMP::{0}", importer.GetErrorString());
			return;
		}
		m_Directory = std::filesystem::path(path).parent_path().generic_string();

		ProcessNode(scene->mRootNode, scene);
	}

	void Model::ProcessNode(aiNode* node, const aiScene* scene)
	{
		for (uint32_t i = 0;i < node->mNumMeshes;i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			m_Meshes.push_back(ProcessMesh(mesh, scene));
		}

		for (uint32_t i = 0;i < node->mNumChildren;++i)
		{
			ProcessNode(node->mChildren[i], scene);
		}
	}
	Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
	{
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		Ref<Material> material;
		const bool hasNormals = mesh && mesh->HasNormals();
		const bool hasTexCoord0 = mesh && mesh->HasTextureCoords(0);

		for (uint32_t i = 0;i < mesh->mNumVertices;i++)
		{
			Vertex vertex;
			aiVector3D& aiVertex = mesh->mVertices[i];
			vertex.Position = { aiVertex.x,aiVertex.y,aiVertex.z };
			if (hasNormals)
			{
				aiVector3D& aiNormal = mesh->mNormals[i];
				vertex.Normal = { aiNormal.x,aiNormal.y,aiNormal.z };
			}
			else
			{
				vertex.Normal = { 0.0f, 1.0f, 0.0f };
			}
			if (hasTexCoord0)
			{
				aiVector3D& aiTexCoord = mesh->mTextureCoords[0][i];
				vertex.TexCoord = { aiTexCoord.x,aiTexCoord.y };
			}
			else
			{
				vertex.TexCoord = { 0.0f, 0.0f };
			}
			vertices.push_back(vertex);
		}

		for (uint32_t i = 0;i < mesh->mNumFaces;i++)
		{
			aiFace& face = mesh->mFaces[i];
			for (uint32_t j = 0;j < face.mNumIndices;j++) 
			{
				indices.push_back(face.mIndices[j]);
			}
		}

		// Some OBJ files do not contain normal data.
		// Rebuild smooth vertex normals from triangle geometry to prevent nullptr access and broken lighting.
		if (!hasNormals && !vertices.empty() && !indices.empty())
		{
			std::vector<glm::vec3> normalAccum(vertices.size(), glm::vec3(0.0f));
			for (uint32_t i = 0; i + 2 < indices.size(); i += 3)
			{
				uint32_t i0 = indices[i];
				uint32_t i1 = indices[i + 1];
				uint32_t i2 = indices[i + 2];
				if (i0 >= vertices.size() || i1 >= vertices.size() || i2 >= vertices.size())
					continue;

				const glm::vec3& p0 = vertices[i0].Position;
				const glm::vec3& p1 = vertices[i1].Position;
				const glm::vec3& p2 = vertices[i2].Position;
				glm::vec3 faceNormal = glm::cross(p1 - p0, p2 - p0);
				float len = glm::length(faceNormal);
				if (len < 1e-8f)
					continue;
				faceNormal /= len;

				normalAccum[i0] += faceNormal;
				normalAccum[i1] += faceNormal;
				normalAccum[i2] += faceNormal;
			}

			for (uint32_t i = 0; i < vertices.size(); ++i)
			{
				float len = glm::length(normalAccum[i]);
				if (len < 1e-8f)
					vertices[i].Normal = { 0.0f, 1.0f, 0.0f };
				else
					vertices[i].Normal = normalAccum[i] / len;
			}
		}

		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* aiMaterial = scene->mMaterials[mesh->mMaterialIndex];
			material = LoadMaterial(aiMaterial);
		}

		return Mesh(vertices, indices, material);
	}

	Ref<Material> Model::LoadMaterial(aiMaterial* mat)
	{
		Ref<Material> material = CreateRef<Material>();

		constexpr uint32_t numTexTypes = 2;
		std::array<aiTextureType, numTexTypes> aiTypes = { aiTextureType_DIFFUSE, aiTextureType_SPECULAR };
		std::array<Texture::Type, numTexTypes> types = { Texture::Type::Diffuse, Texture::Type::Specular };

		for (uint32_t TexTypeIdx = 0;TexTypeIdx < numTexTypes;TexTypeIdx++)
		{
			aiTextureType aiType = aiTypes[TexTypeIdx];
			Texture::Type type = types[TexTypeIdx];
			for (uint32_t i = 0;i < mat->GetTextureCount(aiType);i++)
			{
				aiString str;
				mat->GetTexture(aiType, i, &str);
				const std::string texRelative = std::filesystem::path(str.C_Str()).generic_string();
				const std::string path = (std::filesystem::path(m_Directory) / texRelative).generic_string();
				Ref<Texture2D> texture = std::dynamic_pointer_cast<Texture2D>(AssetsManager::LoadTexture(path));
				material->SetTextureMap(texture, type);
			}
		}
		return material;
	}
}
