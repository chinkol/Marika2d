#include "Material.h"

#include "Core/Config/ConfigSys.h"

#include <filesystem>
#include <iostream>
#include <fstream>

Mrk::Material::Material()
{
}

void Mrk::Material::Bind()
{
	if (shaderProgram)
	{
		shaderProgram->Use();
		auto sp = shaderProgram->GetId();
		for (auto& uniform : uniforms)
		{
			uniform->Bind(sp);
		}
	}
}

const std::vector<std::unique_ptr<Mrk::Uniform>>& Mrk::Material::GetUniforms()
{
	return uniforms;
}

Json::Value Mrk::Material::ToJson(Mrk::JsonAllocator& alloctor)
{
	Json::Value jmat(Json::ArrayType);

	for (auto& uniform : uniforms)
	{
		jmat.PushBack(ReflectSys::ToJson(*uniform, alloctor), alloctor);
	}

	return jmat;
}

void Mrk::Material::FromJson(const Json::Value& json)
{
	if (json.IsArray())
	{
		auto jarr = json.GetArray();
		for (auto& item : jarr)
		{
			
		}
	}
}

void Mrk::Material::AddUniform(Uniform* uniform)
{
	uniforms.push_back(std::unique_ptr<Uniform>(uniform));
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
	auto mat = std::shared_ptr<Material>(new Mrk::Material());
	mat->shaderProgram = shared_from_this();

	return mat;
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

std::shared_ptr<Mrk::Material> Mrk::ShaderProgram::GetSharedMaterial(std::string_view name)
{
	auto ret = sharedMaterials.find(name.data());
	if (ret != sharedMaterials.end())
	{
		if (ret->second.expired())
		{
			ret->second = CreateMaterial();
		}
		return ret->second.lock();
	}
	else
	{
		auto mat = CreateMaterial();
		sharedMaterials.emplace(name.data(), mat);

		return mat;
	}
}

std::shared_ptr<Mrk::Material> Mrk::ShaderProgram::GetUniqueMaterial()
{
	return CreateMaterial();
}

std::shared_ptr<Mrk::Material> Mrk::UnlitShaderProgram::CreateMaterial()
{
	auto mat = ShaderProgram::CreateMaterial();
	mat->AddUniform(UniformFactory::CreateUniform<TextureUniform>("diffuse", TextureUnit::Tex0, TextureType::Tex2D));

	return mat;
}

std::shared_ptr<Mrk::ShaderProgram> Mrk::ShaderProgramHut::GetShaderProgram(std::string_view spPath)
{
	MRK_INSTANCE_REF;

	auto ret = instance.sps.find(spPath.data());
	if (ret != instance.sps.end())
	{
		return ret->second;
	}
	else
	{
		std::filesystem::path shaderFile = spPath;
		auto vsPath = shaderFile.replace_extension(".vert");
		auto fsPath = shaderFile.replace_extension(".frag");
		if (std::filesystem::exists(vsPath) && std::filesystem::exists(fsPath))
		{
			auto vs = Shader(vsPath, GL_VERTEX_SHADER).id;
			auto fs = Shader(fsPath, GL_FRAGMENT_SHADER).id;

			if (vs && fs)
			{
				auto spName = shaderFile.remove_filename().string();
				auto ctor = instance.spctors.find(spName);
				if (ctor != instance.spctors.end())
				{
					auto sp = ctor->second();

					sp->AttachShader(vs);
					sp->AttachShader(fs);

					if (sp->Link())
					{
						instance.sps.emplace(spName, sp);
						return sp;
					}
				}
			}

			glDeleteShader(vs);
			glDeleteShader(fs);
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
		if (location == -1)
		{
			std::cout << std::format("uniform {} not correct !", name) << std::endl;
			// TODO : log
		}
	}

	BindUniform(sp);
}

Mrk::FloatUniform::FloatUniform(std::string_view name) : Uniform(name)
{
}

void Mrk::FloatUniform::BindUniform(GLuint sp)
{
	glUniform1f(location, val);
}

const std::string& Mrk::TextureUniform::GetTexturePath()
{
	return texturePath;
}

void Mrk::TextureUniform::SetTexturePath_(const std::string& texturePath)
{
	this->texturePath = texturePath;
}

Mrk::TextureUniform::TextureUniform(std::string_view name, TextureUnit unit, TextureType type) : Uniform(name),
textureUnit(unit),
textureType(type)
{
}

Mrk::TextureUnit Mrk::TextureUniform::GetTextureUnit()
{
	return textureUnit;
}

void Mrk::TextureUniform::SetTextureUnit_(TextureUnit unit)
{
	textureUnit = unit;
}

Mrk::TextureType Mrk::TextureUniform::GetTextureType()
{
	return textureType;
}

void Mrk::TextureUniform::SetTextureType_(TextureType type)
{
	textureType = type;
}

void Mrk::TextureUniform::BindUniform(GLuint sp)
{
	glActiveTexture((GLenum)textureUnit);
	glBindTexture((GLenum)textureType, textureId);
	glUniform1i(location, (GLenum)textureUnit);
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
		return LoadMaterial(matPath);
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
			jdoc.Parse(content); if (!jdoc.HasParseError())
			{
				auto mat = std::make_shared<Material>();
				mat->FromJson(jdoc);
				return instance.materials.emplace(matPath.string(), mat).first->second;
			}
		}
	}

	return nullptr;
}

Mrk::Uniform* Mrk::UniformFactory::CreateUniform(std::string_view name)
{
	MRK_INSTANCE_REF;

	auto ret = instance.ctors.find(name.data());
	if (ret != instance.ctors.end())
	{
		return ret->second();
	}

	throw "not exist type";
}
