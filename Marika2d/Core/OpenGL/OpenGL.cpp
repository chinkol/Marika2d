#include "OpenGL.h"

#include "Third/stb_image/stb_image.h"

inline Mrk::EBO::EBO(const GLuint* data, size_t size)
{
	glGenBuffers(1, &id);
	Bind();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

inline Mrk::EBO::~EBO() 
{
	glDeleteBuffers(1, &id); 
}

inline void Mrk::EBO::Bind() const 
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id); 
}

inline void Mrk::EBO::UnBind() const 
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); 
}

inline GLuint Mrk::EBO::GetID() const
{
	return id;
}

inline Mrk::Texture::Texture(std::string_view path)
{
	glGenTextures(1, &id);
	Bind();

	int width, height, channels;
	auto imageData = stbi_load(path.data(), &width, &height, &channels, 0);

	GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, imageData);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	UnBind();

	stbi_image_free(imageData);
}

inline Mrk::Texture::~Texture() 
{
	glDeleteTextures(1, &id); 
}

inline void Mrk::Texture::Bind() const 
{
	glBindTexture(GL_TEXTURE_2D, id); 
}

inline void Mrk::Texture::UnBind() const 
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

inline GLuint Mrk::Texture::GetID() const
{
	return id;
}

inline Mrk::VBO::VBO(const void* data, size_t size)
{
	glGenBuffers(1, &id);
	Bind();
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

inline Mrk::VBO::~VBO() 
{ 
	glDeleteBuffers(1, &id); 
}

inline void Mrk::VBO::Bind() const 
{ 
	glBindBuffer(GL_ARRAY_BUFFER, id); 
}

inline void Mrk::VBO::UnBind() const 
{
	glBindBuffer(GL_ARRAY_BUFFER, 0); 
}

inline GLuint Mrk::VBO::GetID() const
{
	return id;
}

inline Mrk::VAO::VAO() 
{
	glGenVertexArrays(1, &id);
}

inline Mrk::VAO::~VAO() 
{ 
	glDeleteVertexArrays(1, &id); 
}

inline void Mrk::VAO::Bind() const
{
	glBindVertexArray(id);
}

inline void Mrk::VAO::UnBind() const
{
	glBindVertexArray(0); 
}

inline GLuint Mrk::VAO::GetID() const
{
	return id;
}

Mrk::ShaderProgram& Mrk::OpenGLContext::GetShaderProgram()
{
	return Instance().program;
}

inline Mrk::OpenGLContext::OpenGLContext() :
	program()
{
}

inline Mrk::ShaderProgram::ShaderProgram()
{
	id = glCreateProgram();
}

inline void Mrk::ShaderProgram::AddShader(const Shader& shader)
{
	auto ret = shaders.try_emplace(shader.GetType(), shader);
	if (!ret.second)
	{
		ret.first->second = shader;
		glDetachShader(id, shader.GetID());
	}
	else
	{
		//log
	}
	glAttachShader(id, shader.GetID());
}

void Mrk::ShaderProgram::Delete()
{
	for (auto& shader : shaders)
	{
		shader.second.Delete();
	}
	glDeleteProgram(id);
}

inline void Mrk::ShaderProgram::Link()
{
	glLinkProgram(id);
}

void Mrk::ShaderProgram::Use()
{
	glUseProgram(id);
}

inline Mrk::Shader::Shader(std::string_view shaderPath, Type shaderType) :
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
	auto lcode = code.c_str();	//ÁÙÊ±×óÖµ
	glShaderSource(id, 1, &lcode, NULL);
	glCompileShader(id);

	GLint success;
	glGetShaderiv(id, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		GLchar errlog[512];
		glGetShaderInfoLog(id, 512, NULL, errlog);
		//std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	};
}

inline GLuint Mrk::Shader::GetID() const
{
	return id;
}

inline Mrk::Shader::Type Mrk::Shader::GetType() const
{
	return type;
}

inline void Mrk::Shader::Delete()
{
	glDeleteShader(id);
}
