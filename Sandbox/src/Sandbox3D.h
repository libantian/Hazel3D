#pragma once

#include <Hazel.h>

#include "imgui/imgui.h"

#include <memory>
#include <cstdint>
#include <string>
#include <vector>

class Sandbox3D : public Hazel::Layer
{
public:
	Sandbox3D();

	virtual void OnAttach() override;
	virtual void OnUpdate(Hazel::Timestep ts) override;
	virtual void OnDetach() override;

	virtual void OnImGuiRender() override;

	virtual void OnEvent(Hazel::Event& e) override;

private:
	Hazel::CameraController m_CameraController;
	bool m_isPerspective = true;

	Hazel::Ref<Hazel::Model> m_Model;
	std::string m_ModelPath = "Sandbox/assets/models/nanosuit/nanosuit.obj";
	glm::vec3 m_ModelPos = { 0.0f, -1.3f, 0.0f };
	float m_ModelScale = 0.2f;

	struct MeshLODData
	{
		std::string Name;
		std::string MaterialName;
		Hazel::Ref<Hazel::Material> Material;
		std::vector<Hazel::Vertex> Vertices;
		std::vector<uint32_t> Indices;
	};
	std::vector<MeshLODData> m_SourceMeshes;
	std::vector<MeshLODData> m_CurrentLodMeshes;
	std::vector<Hazel::Mesh> m_RenderMeshes;
	Hazel::Ref<Hazel::Material> m_FallbackMaterial;

	float m_VertexKeepPercent = 100.0f;
	float m_AppliedVertexKeepPercent = 100.0f;
	bool m_ModelVisible = false;
	bool m_LodDirty = true;
	std::string m_LodStatus = "Model loaded. Click Render to show.";
	std::string m_ExportStatus = "Not exported yet.";
	char m_ExportPathBuffer[260] = "Sandbox/assets/models/simplified_model.obj";
	std::string m_OriginalMtlFileName;
	std::vector<std::string> m_OriginalUseMtlOrder;

	Hazel::Ref<Hazel::DirectionalLight> m_DirectionalLight;
	struct {
		glm::vec3 direction = glm::vec3(-0.2f, -1.0f, -0.3f);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
		float ambient = 0.1f;
		float diffuse = 1.0f;
		float specular = 1.0f;
	}m_DirectionalLightProp;

	std::vector<Hazel::Ref<Hazel::PointLight>> m_PointLights;
	struct PointLightProps
	{
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec3 position = glm::vec3(-0.2f, -1.0f, -0.3f);
		float ambient = 0.1f;
		float diffuse = 1.0f;
		float specular = 1.0f;
		float constant = 1.0f;
		float linear = 0.09f;
		float quadratic = 0.032f;
	};
	std::vector<PointLightProps> m_PointLightProps;
	Hazel::Ref<Hazel::VertexArray> m_PointLightVA;
	int m_PointLightActivated = 0;

	Hazel::Ref<Hazel::SpotLight> m_SpotLight;
	struct {
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
		float ambient = 0.1f;
		float diffuse = 1.0f;
		float specular = 1.0f;
		float constant = 1.0f;
		float linear = 0.09f;
		float quadratic = 0.032f;
		float cutOff = 12.5f;
		float epsilon = 5.0f;
	}m_SpotLightProp;

private:
	void ParseOriginalObjMaterialInfo();
	void CaptureSourceMeshes();
	void BuildLodMeshes(float keepPercent);
	bool ExportCurrentLodObj(const std::string& outputObjPath);
	void MarkLodDirtyAndPause();
	static uint64_t CountTotalVertices(const std::vector<MeshLODData>& meshes);
	static uint64_t CountTotalTriangles(const std::vector<MeshLODData>& meshes);
};
