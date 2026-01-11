#include "hzpch.h"
#include "Material.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace Hazel {

	Material::Material(Ref<Shader> shader)
		:m_Shader(shader)
	{
	}

	void Material::SetTexture(Ref<Texture> texture, int slot)
	{
		texture->Bind(slot);
		m_Shader->Bind();
	}
}