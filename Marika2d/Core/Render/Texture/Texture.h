#pragma once

#include "Common/Singleton/Singleton.h"

#include "Core/OpenGL/OpenGL.h"

#include <map>
#include <string>
#include <memory>

namespace Mrk
{
	/*enum TextureType
	{
		Tex2D = GL_TEXTURE_2D,
		TexCube = GL_TEXTURE_CUBE_MAP,
	};*/

	class Texture
	{
		friend class TextureHut;
	public:
		int GetWidth();
		int GetHeight();
		int GetChannels();

		GLuint GetID();
		void Bind();
		void UnBind();
	private:
		GLuint id;
		int width;
		int height;
		int channels;
	};

	class TextureHut : public Singleton<TextureHut>
	{
	public:
		static const std::map<std::string, std::shared_ptr<Texture>>& GetTextures();
		static std::shared_ptr<Texture> GetTexture(std::string_view path);
	private:
		static std::shared_ptr<Texture> LoadTexture(std::string_view path);
	private:
		std::map<std::string, std::shared_ptr<Texture>> textures;
	};
}


