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
	shaderProgram->Use();
}

void Mrk::Material::UploadUniforms()
{
	auto id = shaderProgram->GetId();

	for (auto& fu : floatUniforms)
	{
		glUniform1f(glGetUniformLocation(id, fu.first.c_str()), fu.second);
	}

	for (auto& tu : textureUniforms)
	{

	}

	// ...
}

void Mrk::Material::AddFloat(std::string_view name)
{
	floatUniforms.try_emplace(name.data(), 0.0f);
}

void Mrk::Material::AddTexture(std::string_view name)
{
	textureUniforms.try_emplace(name.data(), 0);
}

void Mrk::Material::AddVector(std::string_view name)
{
	vectorUniforms.try_emplace(name.data(), Vector4{ 0.0f,0.0f,0.0f,0.0f });
}

const std::map<std::string, float>& Mrk::Material::GetFloatUniforms()
{
	return floatUniforms;
}

const std::map<std::string, GLuint>& Mrk::Material::GetTextureUniforms()
{
	return textureUniforms;
}

const std::map<std::string, Mrk::Vector4>& Mrk::Material::GetVectorUniforms()
{
	return vectorUniforms;
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

	mat->AddTexture("diffuse");

	return mat;
}

std::shared_ptr<Mrk::ShaderProgram> Mrk::ShaderProgramHut::GetShaderProgram(std::string_view name)
{
	MRK_INSTANCE_REF;

	auto ret = instance.sps.find(name.data());
	if (ret != instance.sps.end())
	{
		return ret->second;
	}
	else
	{
		std::filesystem::path shaderDir = Mrk::ConfigSys::GetConfigItem<std::string>("ShaderSetting", "shaderSourcesDir");
		auto vsPath = (shaderDir / name.data()).replace_extension(".vert");
		auto fsPath = (shaderDir / name.data()).replace_extension(".frag");
		if (std::filesystem::exists(vsPath) && std::filesystem::exists(fsPath))
		{
			auto vs = Shader(vsPath, GL_VERTEX_SHADER).id;
			auto fs = Shader(fsPath, GL_FRAGMENT_SHADER).id;

			if (vs && fs)
			{
				auto ctor = instance.spctors.find(name.data());
				if (ctor != instance.spctors.end())
				{
					auto sp = ctor->second();

					sp->AttachShader(vs);
					sp->AttachShader(fs);

					if (sp->Link())
					{
						instance.sps.emplace(name.data(), sp);
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
