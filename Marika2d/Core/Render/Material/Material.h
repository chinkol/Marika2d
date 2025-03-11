#pragma once

#include "Common/Singleton/Singleton.h"

#include "Core/Math/Math.h"
#include "Core/OpenGL/OpenGL.h"
#include "Core/Config/ConfigSys.h"

#include <filesystem>
#include <functional>
#include <string>
#include <variant>
#include <map>

#ifndef MRK_SHADERPROGRAM
#define MRK_SHADERPROGRAM(x) private: static inline bool _mrk_macro_shader_program_##x##_register = [](){ Mrk::ShaderProgramHut::RegisterShaderProgram<x>(#x); return true; }();
#endif // !MRK_SHADERPROGRAM

namespace Mrk
{
	class ShaderSetting : public ConfigGroup
	{
		MRK_CONFIG(ShaderSetting)
	public:
		std::string defaultShaderProgramName = "UnlitShaderProgram";
		std::string defaultMaterialName = "default";
		std::string shaderSourcesDir = "../TestProject/ShaderSources";
	};

	class Material;
	class ShaderProgram;

	class ShaderProgramHut : Mrk::Singleton<ShaderProgramHut>
	{
		MRK_SINGLETON(ShaderProgramHut)
	public:
		template<typename T>
		static void RegisterShaderProgram(std::string_view name);
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

	class ShaderProgram : public std::enable_shared_from_this<ShaderProgram>
	{
	public:
		ShaderProgram();
		virtual ~ShaderProgram();
		virtual std::shared_ptr<Material> CreateMaterial();

		GLuint GetId();
		bool Link();
		void Use();
		void AttachShader(GLuint id);
		std::shared_ptr<Material> GetSharedMaterial(std::string_view name);
		std::shared_ptr<Material> GetUniqueMaterial();
	private:
		GLuint id;
		std::map<std::string, std::weak_ptr<Material>> sharedMaterials;
	};

	class UnlitShaderProgram : public ShaderProgram
	{
		MRK_SHADERPROGRAM(UnlitShaderProgram)
	public:
		virtual std::shared_ptr<Material> CreateMaterial() override;
	};

	class Material
	{
		friend class ShaderProgram;
	private:
		Material();
	public:
		void Bind();
		void UploadUniforms();
		void AddFloat(std::string_view name);
		void AddTexture(std::string_view name);
		void AddVector(std::string_view name);
		const std::map<std::string, float>& GetFloatUniforms();
		const std::map<std::string, GLuint>& GetTextureUniforms();
		const std::map<std::string, Vector4>& GetVectorUniforms();
	private:
		std::shared_ptr<ShaderProgram> shaderProgram;

		std::map<std::string, float> floatUniforms;
		std::map<std::string, GLuint> textureUniforms;
		std::map<std::string, Vector4> vectorUniforms;
	};
}

namespace Mrk
{
	template<typename T>
	inline void ShaderProgramHut::RegisterShaderProgram(std::string_view name)
	{
		static_assert(std::is_base_of_v<ShaderProgram, T>, "T is not IShaderProgram !");

		MRK_INSTANCE_REF;

		instance.spctors.emplace(name.data(), []() {
			return std::make_shared<T>();
			});
	}
}