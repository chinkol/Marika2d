#include "OpenGL.h"

#include "Third/SOIL2/SOIL2.h"

#include <iostream>

Mrk::EBO::EBO(const GLuint* data, size_t size)
{
	glGenBuffers(1, &id);
	Bind();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

Mrk::EBO::~EBO()
{
	glDeleteBuffers(1, &id);
}

void Mrk::EBO::Bind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
}

void Mrk::EBO::UnBind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

GLuint Mrk::EBO::GetID() const
{
	return id;
}

Mrk::Texture::Texture(std::string_view path)
{
	id = SOIL_load_OGL_texture(path.data(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_DDS_LOAD_DIRECT | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS);

	Bind();
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	UnBind();
}

Mrk::Texture::~Texture()
{
	glDeleteTextures(1, &id);
}

void Mrk::Texture::Bind() const
{
	glBindTexture(GL_TEXTURE_2D, id);
}

void Mrk::Texture::UnBind() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

GLuint Mrk::Texture::GetID() const
{
	return id;
}

Mrk::VBO::VBO(const void* data, size_t size)
{
	glGenBuffers(1, &id);
	Bind();
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

Mrk::VBO::~VBO()
{
	glDeleteBuffers(1, &id);
}

void Mrk::VBO::Bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, id);
}

void Mrk::VBO::UnBind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

GLuint Mrk::VBO::GetID() const
{
	return id;
}

Mrk::VAO::VAO()
{
	glGenVertexArrays(1, &id);
}

Mrk::VAO::~VAO()
{
	glDeleteVertexArrays(1, &id);
}

void Mrk::VAO::Bind() const
{
	glBindVertexArray(id);
}

void Mrk::VAO::UnBind() const
{
	glBindVertexArray(0);
}

GLuint Mrk::VAO::GetID() const
{
	return id;
}

Mrk::ShaderProgram& Mrk::OpenGLContext::GetShaderProgram()
{
	return Instance().program;
}

Mrk::OpenGLContext::OpenGLContext() :
	program()
{
}

Mrk::ShaderProgram::ShaderProgram()
{
}

GLuint Mrk::ShaderProgram::GetID()
{
	return id;
}

void Mrk::ShaderProgram::Create()
{
	id = glCreateProgram();
}

void Mrk::ShaderProgram::AddShader(GLuint id)
{
	glAttachShader(this->id, id);
}

void Mrk::ShaderProgram::Delete()
{
	glDeleteProgram(id);
}

void Mrk::ShaderProgram::Link()
{
	glLinkProgram(id);

	GLint success;
	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if (!success) {
		// 链接失败，获取错误日志
		GLint logLength;
		glGetProgramiv(id, GL_INFO_LOG_LENGTH, &logLength);

		// 分配足够的空间存储日志
		char* infoLog = (char*)malloc(logLength);
		glGetProgramInfoLog(id, logLength, NULL, infoLog);

		// 输出错误日志
		std::cout << infoLog << std::endl;

		// 释放内存
		free(infoLog);

		throw;
	}
}

void Mrk::ShaderProgram::Use()
{
	glUseProgram(id);
}

Mrk::Shader::Shader(std::string_view shaderPath, Type shaderType) :
	type(shaderType)
{
	std::string code;
	std::ifstream file;
	file.exceptions(std::ifstream::badbit);
	try
	{
		file.open(shaderPath.data());
		std::stringstream stream;
		stream << file.rdbuf();
		file.close();
		code = stream.str();
	}
	catch (std::ifstream::failure e)
	{
		//std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

	id = glCreateShader(shaderType == Type::Vertex ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER);
	auto lcode = code.c_str();	//临时左值
	glShaderSource(id, 1, &lcode, NULL);
	glCompileShader(id);

	GLint success;
	glGetShaderiv(id, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		isValid = false;
	}
	else
	{
		isValid = true;
	}
}

Mrk::Shader::~Shader()
{
}

GLuint Mrk::Shader::GetID() const
{
	return id;
}

Mrk::Shader::Type Mrk::Shader::GetType() const
{
	return type;
}

bool Mrk::Shader::IsValid() const
{
	return isValid;
}

void Mrk::Shader::Delete()
{
	glDeleteShader(id);
}

GLuint Mrk::ShaderProgramHut::GetShaderProgramID(std::string_view vs, std::string_view fs)
{
	MRK_INSTANCE_REF;

	std::hash<std::string> hasher;

	auto hash = hasher(vs.data());
	hash ^= hasher(fs.data()) + 0x9e3779b9 + (hash << 6) + (hash >> 2);

	auto ret = instance.sps.find(hash);
	if (ret == instance.sps.end())
	{
		auto sp = ShaderProgram();
		auto vs_ = GetShaderID(Shader::Type::Vertex, vs);
		auto ps_ = GetShaderID(Shader::Type::Fragmet, fs);
		if (vs_ && ps_)
		{
			sp.Create();
			sp.AddShader(vs_);
			sp.AddShader(ps_);
			sp.Link();
			return instance.sps.emplace(hash, sp).first->second.GetID();
		}
		else
		{
			return 0;
		}
	}

	return ret->second.GetID();
}

GLuint Mrk::ShaderProgramHut::GetShaderID(Shader::Type type, std::string_view path)
{
	MRK_INSTANCE_REF;

	auto ret = instance.temps.find(path.data());
	if (ret == instance.temps.end())
	{
		auto shader = Shader(path, type);
		if (shader.IsValid())
		{
			return instance.temps.emplace(path.data(), shader).first->second.GetID();
		}
		else
		{
			return 0;
		}
	}
	return ret->second.GetID();
}