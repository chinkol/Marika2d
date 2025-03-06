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


#define MRK_GL_LOCATING_POSITION 0
#define MRK_GL_LOCATING_NORMAL 1
#define MRK_GL_LOCATING_TEXCOORD 2
#define MRK_GL_LOCATING_VIEW 0
#define MRK_GL_LOCATING_PROJ 4
#define MRK_GL_LOCATING_VIEWPROJ 8
#define MRK_GL_LOCATING_WORLD 12
#define MRK_GL_LOCATING_OBJECTID 16
#define MRK_GL_LOCATING_DIFFUSEMAP 17

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
		~Shader();
		GLuint GetID() const;
		Type GetType() const;
		bool IsValid() const;
	private:
		void Delete();
	private:
		bool isValid = false;
		GLuint id = 0;
		Type type;
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
		GLuint id = 0;
	};

	class ShaderProgramHut : public Mrk::Singleton<ShaderProgramHut>
	{
		MRK_SINGLETON(ShaderProgramHut)
	public:
		static GLuint GetShaderProgramID(std::string_view vs, std::string_view fs);
		static GLuint GetShaderID(Shader::Type type, std::string_view path);
	private:
		std::map<size_t, ShaderProgram> sps;
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