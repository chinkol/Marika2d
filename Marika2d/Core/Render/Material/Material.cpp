#include "Material.h"

#include "Common/Utility/Utility.h"

#include "Core/Config/ConfigSys.h"
#include "Core/Render/Texture/Texture.h"

#include <filesystem>
#include <iostream>
#include <fstream>

Mrk::Material::Material()
{
}

void Mrk::Material::Bind(GLuint sp)
{
	for (auto& uniform : uniforms)
	{
		uniform->Bind(sp);
	}
}

const std::vector<std::unique_ptr<Mrk::Uniform>>& Mrk::Material::GetUniforms()
{
	return uniforms;
}

Json::Value Mrk::Material::ToJson(Mrk::JsonAllocator& alloctor)
{
	auto jmat = ReflectSys::ToJson(*this, alloctor);

	Json::Value juniforms(Json::ArrayType);

	for (auto& uniform : uniforms)
	{
		auto juniform = ReflectSys::ToJson(*uniform, alloctor);
		juniform.AddMember("_mrk_uniform_class_name_", Json::Value(uniform->get_type().get_name().data(), alloctor), alloctor);
		juniforms.PushBack(juniform, alloctor);
	}

	jmat.AddMember("uniforms", juniforms, alloctor);

	return jmat;
}

void Mrk::Material::FromJson(const Json::Value& json)
{
	if (json.IsObject())
	{
		ReflectSys::FromJson(*this, json);

		auto juniformsMember = json.FindMember("uniforms");
		if (juniformsMember != json.MemberEnd() && juniformsMember->value.IsArray())
		{
			auto juniforms = juniformsMember->value.GetArray();
			for (auto& juniform : juniforms)
			{
				if (juniform.IsObject())
				{
					auto jclassName = juniform.FindMember("_mrk_uniform_class_name_");
					if (jclassName != juniform.MemberEnd())
					{
						if (jclassName->value.IsString())
						{
							auto uniform = UniformHut::CreateUniform(jclassName->value.GetString());
							ReflectSys::FromJson(*uniform, juniform);
							uniforms.push_back(std::move(uniform));
						}
					}
				}
			}
		}
	}
}

void Mrk::Material::AddUniform(std::unique_ptr<Uniform> uniform)
{
	uniforms.push_back(std::move(uniform));
}

Mrk::Shader::Shader(const std::filesystem::path& path, int shaderType)
{
	std::string code;

	if (std::filesystem::exists(path))
	{
		std::ifstream file(path);
		if (file.is_open())
		{
			std::stringstream stream;
			stream << file.rdbuf();
			file.close();
			code = stream.str();
		}
	}

	auto lcode = code.c_str();
	id = glCreateShader(shaderType);
	glShaderSource(id, 1, &lcode, NULL);
	glCompileShader(id);

	GLint success;
	glGetShaderiv(id, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glDeleteShader(id);
	}
}

Mrk::ShaderProgram::ShaderProgram()
{
	id = glCreateProgram();
}

Mrk::ShaderProgram::~ShaderProgram()
{
	glDeleteProgram(id);
}

std::shared_ptr<Mrk::Material> Mrk::ShaderProgram::CreateMaterial()
{
	return std::make_shared<Material>();
}

GLuint Mrk::ShaderProgram::GetId()
{
	return id;
}

bool Mrk::ShaderProgram::Link()
{
	glLinkProgram(id);

	GLint success;
	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if (!success)
	{
		GLint logLength;
		glGetProgramiv(id, GL_INFO_LOG_LENGTH, &logLength);
		char* infoLog = (char*)malloc(logLength);
		glGetProgramInfoLog(id, logLength, NULL, infoLog);
		std::cout << infoLog << std::endl;
		free(infoLog);

		return false;
	}

	return true;
}

void Mrk::ShaderProgram::Use()
{
	glUseProgram(id);
}

void Mrk::ShaderProgram::AttachShader(GLuint id)
{
	glAttachShader(this->id, id);
}

std::shared_ptr<Mrk::Material> Mrk::UnlitShaderProgram::CreateMaterial()
{
	auto mat = ShaderProgram::CreateMaterial();
	mat->AddUniform(std::move(UniformHut::CreateUniform<UniformTexture2D>("diffuseMap", TextureUnit::Tex0)));

	return mat;
}

std::shared_ptr<Mrk::ShaderProgram> Mrk::ShaderProgramHut::GetShaderProgram(std::string_view spPath)
{
	MRK_INSTANCE_REF;

	std::filesystem::path _spPath = spPath;
	auto spName = _spPath.filename().replace_extension().string();

	auto ret = instance.sps.find(spName);
	if (ret != instance.sps.end())
	{
		return ret->second;
	}
	else
	{
		auto ctor = instance.spctors.find(spName);

		if (ctor != instance.spctors.end())
		{
			auto sp = ctor->second();

			auto vsPath = _spPath.replace_filename(sp->GetName()).replace_extension(".vert");
			auto fsPath = _spPath.replace_filename(sp->GetName()).replace_extension(".frag");

			if (std::filesystem::exists(vsPath) && std::filesystem::exists(fsPath))
			{
				auto vs = Shader(vsPath, GL_VERTEX_SHADER).id;
				auto fs = Shader(fsPath, GL_FRAGMENT_SHADER).id;

				if (vs && fs)
				{
					sp->AttachShader(vs);
					sp->AttachShader(fs);

					if (sp->Link())
					{
						instance.sps.emplace(spName, sp);
						return sp;
					}
				}

				glDeleteShader(vs);
				glDeleteShader(fs);
			}
		}
	}

	return std::shared_ptr<ShaderProgram>();
}

Mrk::Uniform::Uniform()
{
}

Mrk::Uniform::Uniform(std::string_view name) :
	name(name)
{
}

const std::string& Mrk::Uniform::GetName()
{
	return name;
}

void Mrk::Uniform::SetName_(const std::string& name)
{
	this->name = name;
}

void Mrk::Uniform::Bind(GLuint sp)
{
	if (location == -1)
	{
		location = glGetUniformLocation(sp, name.data());
	}

	if (location)
	{
		BindUniform(sp);
	}
	else
	{
		std::cout << std::format("uniform '{}' not correct !", name) << std::endl;
	}

}

Mrk::FloatUniform::FloatUniform(std::string_view name) : Uniform(name)
{
}

void Mrk::FloatUniform::BindUniform(GLuint sp)
{
	glUniform1f(location, val);
}

const std::string& Mrk::UniformTexture2D::GetTexturePath()
{
	return texturePath;
}

void Mrk::UniformTexture2D::SetTexturePath(const std::string& texturePath)
{
	this->texturePath = texturePath;
	isDirty = true;
}

Mrk::UniformTexture2D::UniformTexture2D(std::string_view name, TextureUnit unit) : Uniform(name),
textureUnit(unit)
{
}

Mrk::TextureUnit Mrk::UniformTexture2D::GetTextureUnit()
{
	return textureUnit;
}

void Mrk::UniformTexture2D::SetTextureUnit_(TextureUnit unit)
{
	textureUnit = unit;
}

void Mrk::UniformTexture2D::BindUniform(GLuint sp)
{
	if (!textureId || isDirty)
	{
		if (auto tex = TextureHut::GetTexture(texturePath))
		{
			textureId = tex->GetID();
		}

		isDirty = false;
	}

	if (textureId)
	{
		glActiveTexture((GLenum)textureUnit);
		glBindTexture(GL_TEXTURE_2D, textureId);
		glUniform1i(location, (GLenum)textureUnit);
	}
}

void Mrk::MaterialHut::ReloadMaterial(const std::filesystem::path& matPath)
{
	MRK_INSTANCE_REF;

	auto pathStr = matPath.string();
	instance.materials[pathStr] = LoadMaterial(pathStr);
}

std::shared_ptr<Mrk::Material> Mrk::MaterialHut::GetMaterial(const std::filesystem::path& matPath)
{
	MRK_INSTANCE_REF;

	auto path = matPath.string();

	auto ret = instance.materials.find(path);
	if (ret != instance.materials.end())
	{
		return ret->second;
	}
	else
	{
		return instance.materials.emplace(matPath.string(), LoadMaterial(matPath)).first->second;
	}
}

std::shared_ptr<Mrk::Material> Mrk::MaterialHut::LoadMaterial(const std::filesystem::path& matPath)
{
	MRK_INSTANCE_REF;

	if (std::filesystem::exists(matPath))
	{
		auto ifstream = std::ifstream(matPath);
		if (ifstream.is_open())
		{
			std::string content;
			char ch;
			while (ifstream.get(ch))
			{
				content += ch;
			}
			ifstream.close();

			Json::Document jdoc;
			jdoc.Parse(content);
			if (!jdoc.HasParseError())
			{
				auto mat = std::make_shared<Material>();
				mat->FromJson(jdoc);
				return mat;
			}
		}
	}

	return std::shared_ptr<Mrk::Material>();
}

void Mrk::MaterialHut::SaveMaterial(std::shared_ptr<Material> material, const std::filesystem::path& matPath)
{
	Json::Document jdoc;
	Mrk::Utility::SaveJson(material->ToJson(jdoc.GetAllocator()), matPath.string());
}

std::unique_ptr<Mrk::Uniform> Mrk::UniformHut::CreateUniform(std::string_view name)
{
	MRK_INSTANCE_REF;

	auto ret = instance.ctors.find(name.data());
	if (ret != instance.ctors.end())
	{
		return ret->second();
	}

	throw "not exist type";
}