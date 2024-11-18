#pragma once

#include "Common/Singleton/Singleton.h"

#include "Third/glad/include/glad.h"
#include "Third/glm/glm.hpp"

#include <cstdint>
#include <string>
#include <fstream>
#include <sstream>
#include <map>

namespace Mrk
{
	class VAO
	{
	public:
		VAO();
		~VAO();
		void Bind() const;
		void UnBind() const;
		GLuint GetID() const;

	private:
		GLuint id;
	};

	class VBO
	{
	public:
		VBO(const void* data, size_t size);
		~VBO();
		void Bind() const;
		void UnBind() const;
		GLuint GetID() const;

	private:
		GLuint id;
	};

	class EBO
	{
	public:
		EBO(const GLuint* data, size_t size);
		~EBO();
		void Bind() const;
		void UnBind() const;
		GLuint GetID() const;

	private:
		GLuint id;
	};

	class Shader
	{
	public:
		enum class Type
		{
			Vertex,
			Fragmet,
		};

	public:
		Shader(std::string_view shaderPath, Type shaderType);
		~Shader() = default;
		GLuint GetID() const;
		Type GetType() const;
		void Delete();

	private:
		Type type;
		GLuint id;
	};

	class ShaderProgram
	{
	public:
		ShaderProgram();
		void AddShader(const Shader& shader);
		void Delete();
		void Link();
		void Use();
		
	private:
		std::map<Shader::Type, Shader> shaders;
		GLuint id;
	};

	class Texture
	{
	public:
		Texture(std::string_view path);
		~Texture();
		void Bind() const;
		void UnBind() const;
		GLuint GetID() const;

	private:
		GLuint id;
	};

	class OpenGLContext : public Singleton<OpenGLContext>
	{
		MRK_SINGLETON(OpenGLContext)
	public:
		static ShaderProgram& GetShaderProgram();
	private:
		OpenGLContext();
		ShaderProgram program;
	};
}