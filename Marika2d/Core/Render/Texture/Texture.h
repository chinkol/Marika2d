#pragma once

#include "Common/Singleton/Singleton.h"

#include "Core/OpenGL/OpenGL.h"

#include <map>
#include <string>
#include <memory>

namespace Mrk
{
	enum TextureType
	{
		Tex2D = GL_TEXTURE_2D,
		TexCube = GL_TEXTURE_CUBE_MAP,
	};

	class Texture
	{
		friend class TextureHut;
	public:
		GLuint GetID();
		void Bind();
		void UnBind();
	private:
		GLuint id;
	};

	class TextureHut : public Singleton<TextureHut>
	{
	public:
		std::shared_ptr<Texture> GetTexture(std::string_view path);
	private:
		std::shared_ptr<Texture> LoadTexture(std::string_view path);
	private:
		std::map<std::string, std::shared_ptr<Texture>> textures;
	};
}


