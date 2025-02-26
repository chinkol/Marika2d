#pragma once

#include "Common/Singleton/Singleton.h"

#include "Third/glad/include/glad.h"
#include "Third/glm/glm.hpp"

#include <cstdint>
#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>

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
		friend class ShaderProgramHut;
	public:
		ShaderProgram();
		GLuint GetID();
		void Delete();
		void Link();
		void Use();
	private:
		void Create();
		void AddShader(GLuint id);
	private:
		GLuint id;
	};

	class ShaderProgramHut : public Mrk::Singleton<ShaderProgramHut>
	{
		MRK_SINGLETON(ShaderProgramHut)
	public:
		static GLuint GetShaderProgramID(std::string_view vs, std::string_view fs);
		static GLuint GetShaderID(Shader::Type type, std::string_view path);
	private:
		std::map<size_t, ShaderProgram> sps;
		std::vector< ShaderProgram> spvs;
		std::map<std::string, Shader> temps;	// ±àÒëºÃµÄshader
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