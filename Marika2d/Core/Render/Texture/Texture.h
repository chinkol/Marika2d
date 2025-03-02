#pragma once

#include "Common/Singleton/Singleton.h"

#include "Core/OpenGL/OpenGL.h"

#include <map>
#include <string>

namespace Mrk
{
	enum TextureType
	{
		Tex2D = GL_TEXTURE_2D,
		TexCube = GL_TEXTURE_CUBE_MAP,
	};

	class Texture_
	{
		friend class TextureHub;
	public:
		GLuint GetID();
		void Bind();
		void UnBind();
	private:
		GLuint id;
	};

	class TextureHub : public Singleton<TextureHub>
	{
	public:
		std::shared_ptr<Texture_> GetTexture(std::string_view path);
	private:
		std::shared_ptr<Texture_> LoadTexture(std::string_view path);
	private:
		std::map<std::string, std::shared_ptr<Texture_>> textures;
	};
}


