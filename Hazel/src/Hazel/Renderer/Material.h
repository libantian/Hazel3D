#pragma once

#include "Hazel/Core/Core.h"
#include <Hazel/Renderer/Texture.h>
#include <Hazel/Renderer/Shader.h>

#include <glm/glm.hpp>

namespace Hazel {

	class Material
	{
	public:
		Material(Ref<Shader> shader, float shininess = 32.0f);
		Material(
			Ref<Texture2D> diffuseMap = nullptr,
			Ref<Texture2D> specularMap = nullptr,
			Ref<Texture2D> emissionMap = nullptr,
			float shininess = 32.0f
		);

		inline void Material::SetDiffuseMap(Ref<Texture2D> texture)
		{
			m_DiffuseMap = texture;
		}

		inline void Material::SetSpecularMap(Ref<Texture2D> texture)
		{
			m_SpecularMap = texture;
		}

		inline void Material::SetEmissionMap(Ref<Texture2D> texture)
		{
			m_EmissionMap = texture;
		}

		void SetTextureMap(Ref<Texture2D> texture, Texture::Type type);

		inline float GetShininess() const { return m_Shininess; }
		inline void SetShininess(float shininess) { m_Shininess = shininess; }

		inline const Ref<Shader>& GetShader() const { return m_Shader; }

		void Bind(const Ref<Shader>& shader);
	private:
		static const int s_DiffuseSlot = 0;
		static const int s_SpecularSlot = 1;
		static const int s_EmissionSlot = 2;
	private:
		Ref<Shader> m_Shader;
		Ref<Texture2D> m_DiffuseMap;
		Ref<Texture2D> m_SpecularMap;
		Ref<Texture2D> m_EmissionMap;
		float m_Shininess;
	};

}
