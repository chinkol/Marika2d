#include "Material.h"

#include "Core/Config/ConfigSys.h"

#include <filesystem>
#include <iostream>

void MrkTest::Material::SetShaderProgram(std::shared_ptr<ShaderProgram> shaderProgram)
{
	if (shaderProgram)
	{
		if (this->shaderProgram == shaderProgram)
		{
			return;
		}
		else
		{
			this->shaderProgram = shaderProgram;
			uniforms = shaderProgram->CreateUniforms();
		}
	}
}

const std::map<std::string, std::variant<float, GLuint, Mrk::Vector4>>& MrkTest::Material::GetUniforms()
{
	return uniforms;
}

MrkTest::Shader::Shader(const std::filesystem::path& path, int shaderType)
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
	glShaderSource(id, 1, &lcode, NULL);
	glCompileShader(id);

	bool valid;
	glGetShaderiv(id, GL_COMPILE_STATUS, (GLint*)&valid);
	if (!valid)
	{
		glDeleteShader(id);
	}
}

std::shared_ptr<MrkTest::ShaderProgram> MrkTest::ShaderProgramHut::GetShaderProgram(std::string_view name)
{
	MRK_INSTANCE_REF;

	auto ret = instance.sps.find(name.data());
	if (ret != instance.sps.end())
	{
		return ret->second;
	}
	else
	{
		std::filesystem::path shaderDir = Mrk::ConfigSys::GetConfigItem<std::string>("AppConfig", "ShaderSourcesPath");
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
					instance.sps.emplace(name.data(), sp);

					sp->AttachShader(vs);
					sp->AttachShader(fs);
					sp->Link();

					return sp;
				}
			}
		}
	}

	return std::shared_ptr<ShaderProgram>();
}

MrkTest::ShaderProgram::ShaderProgram()
{
	id = glCreateProgram();
}

MrkTest::ShaderProgram::~ShaderProgram()
{
	glDeleteProgram(id);
}

bool MrkTest::ShaderProgram::Link()
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

void MrkTest::ShaderProgram::Use()
{
	glUseProgram(id);
}

void MrkTest::ShaderProgram::AttachShader(GLuint id)
{
	glAttachShader(this->id, id);
}

std::map<std::string, std::variant<float, GLuint, Mrk::Vector4>> MrkTest::UnlitShaderProgram::CreateUniforms()
{
	static std::map<std::string, std::variant<float, GLuint, Mrk::Vector4>> uniforms = {
		{ "diffuse", (GLuint)0 }
	};

	return uniforms;
}
