#pragma once

#include "Common/Singleton/Singleton.h"

#include "Core/Math/Math.h"
#include "Core/OpenGL/OpenGL.h"
#include "Core/Config/ConfigSys.h"

#include <functional>
#include <string>
#include <variant>
#include <map>

#ifndef MRK_SHADERPROGRAM
#define MRK_SHADERPROGRAM(x) private: static inline bool _mrk_macro_shader_program_##x##_register = [](){ MrkTest::ShaderProgramHut::Register<x>(#x); return true; }();
#endif // !MRK_SHADERPROGRAM

namespace MrkTest
{
	class MaterialHut : Mrk::Singleton<MaterialHut>
	{
		MRK_SINGLETON(MaterialHut)
	public:

	private:

	};

	class ShaderProgram;

	class ShaderProgramHut : public Mrk::Singleton<ShaderProgramHut>
	{
		MRK_SINGLETON(ShaderProgramHut)
	public:
		template<typename T>
		static void Register(std::string_view name);
		static std::shared_ptr<ShaderProgram> GetShaderProgram(std::string_view name);
	private:
		std::map<std::string, std::shared_ptr<ShaderProgram>> sps;
		std::map<std::string, std::function<std::shared_ptr<ShaderProgram>()>> spctors;
	};

	struct Shader
	{
		Shader(const std::filesystem::path& path, int shaderType);

		GLuint id = 0;
	};

	class ShaderProgram
	{
	public:
		ShaderProgram();
		virtual ~ShaderProgram();
		virtual std::map<std::string, std::variant<float, GLuint, Mrk::Vector4>> CreateUniforms() = 0;
		bool Link();
		void Use();
		void AttachShader(GLuint id);
	private:
		GLuint id;
	};

	class UnlitShaderProgram : public ShaderProgram
	{
		MRK_SHADERPROGRAM(UnlitShaderProgram)
	public:
		virtual std::map<std::string, std::variant<float, GLuint, Mrk::Vector4>> CreateUniforms() override;
	};

	class Material
	{
	public:
		void SetShaderProgram(std::shared_ptr<ShaderProgram> shaderProgram);
		const std::map<std::string, std::variant<float, GLuint, Mrk::Vector4>>& GetUniforms();
	private:
		std::shared_ptr<ShaderProgram> shaderProgram;
		std::map<std::string, std::variant<float, GLuint, Mrk::Vector4>> uniforms;
	};
}

namespace MrkTest
{
	template<typename T>
	inline void ShaderProgramHut::Register(std::string_view name)
	{
		static_assert(std::is_base_of_v<ShaderProgram, T>, "T is not IShaderProgram !");

		MRK_INSTANCE_REF;

		instance.spctors.emplace(name.data(), []() {
			return std::make_shared<T>();
			});
	}
}