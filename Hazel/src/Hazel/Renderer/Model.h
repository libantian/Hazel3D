#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"

namespace Hazel {

	class Model
	{
	public:
		Model() = delete;
		Model(const std::string& path)
		{
			LoadModel(path);
		}
		const std::vector<Mesh>& GetMeshes() const { return m_Meshes; }
		const std::string& GetDirectory() const { return m_Directory; }

		// Reload model from a new file path (clears existing mesh data)
		void Load(const std::string& path)
		{
			m_Meshes.clear();
			m_Directory.clear();
			LoadModel(path);
		}
	private:
		void LoadModel(const std::string& path);
		void ProcessNode(aiNode* node, const aiScene* scene);
		Mesh ProcessMesh(aiMesh* mesh, const aiScene* Scene);
		Ref<Material> LoadMaterial(aiMaterial* mat);
	private:
		std::vector<Mesh> m_Meshes;
		std::string m_Directory;
	};
}
