#include "Texture.h"

#include "Third/SOIL2/SOIL2.h"

GLuint Mrk::Texture_::GetID()
{
    return id;
}

void Mrk::Texture_::Bind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
}

void Mrk::Texture_::UnBind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

std::shared_ptr<Mrk::Texture_> Mrk::TextureHub::GetTexture(std::string_view path)
{
	auto ret = Instance().textures.try_emplace(path.data(), nullptr);
	if (ret.second)
	{
		ret.first->second = LoadTexture(path);
	}
	return ret.first->second;
}

std::shared_ptr<Mrk::Texture_> Mrk::TextureHub::LoadTexture(std::string_view path)
{
	auto texture = std::make_shared<Mrk::Texture_>();
	texture->id = SOIL_load_OGL_texture(path.data(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_DDS_LOAD_DIRECT | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS);

	texture->Bind();
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	texture->UnBind();

	return texture;
}
