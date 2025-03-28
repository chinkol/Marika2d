#include "Texture.h"

#include "Third/SOIL2/SOIL2.h"

int Mrk::Texture::GetWidth()
{
	return width;
}

int Mrk::Texture::GetHeight()
{
	return height;
}

int Mrk::Texture::GetChannels()
{
	return channels;
}

GLuint Mrk::Texture::GetID()
{
    return id;
}

void Mrk::Texture::Bind()
{
	glBindBuffer(GL_TEXTURE_2D, id);
}

void Mrk::Texture::UnBind()
{
	glBindBuffer(GL_TEXTURE_2D, 0);
}

const std::map<std::string, std::shared_ptr<Mrk::Texture>>& Mrk::TextureHut::GetTextures()
{
	MRK_INSTANCE_REF;

	return instance.textures;
}

std::shared_ptr<Mrk::Texture> Mrk::TextureHut::GetTexture(std::string_view path)
{
	MRK_INSTANCE_REF;

	auto ret = instance.textures.find(path.data());
	if (ret != instance.textures.end())
	{
		return ret->second;
	}
	else
	{
		if (auto tex = LoadTexture(path))
		{
			instance.textures.emplace(path.data(), tex);
			return tex;
		}
		
	}

	return nullptr;
}

std::shared_ptr<Mrk::Texture> Mrk::TextureHut::LoadTexture(std::string_view path)
{
	int width, height, channels;

	// 获取纹理信息
	auto image = SOIL_load_image(path.data(), &width, &height, &channels, SOIL_LOAD_AUTO);

	if (image)
	{
		auto id = SOIL_load_OGL_texture(path.data(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_DDS_LOAD_DIRECT | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS);
		SOIL_free_image_data(image);

		if (id)
		{
			auto texture = std::make_shared<Mrk::Texture>();
			texture->id = id;
			texture->width = width;
			texture->height = height;
			texture->channels = channels;

			texture->Bind();
			glGenerateMipmap(GL_TEXTURE_2D);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			texture->UnBind();

			return texture;
		}
	}

	return nullptr;
}
