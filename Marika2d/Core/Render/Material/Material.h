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
#define MRK_SHADERPROGRAM(x, y) private: static inline bool _mrk_macro_shader_program_##x##_register = [](){ Mrk::ShaderProgramHut::RegisterShaderProgram<x>(y); return true; }();	\
virtual std::string_view GetName() override { return y; }
#endif // !MRK_SHADERPROGRAM

#ifndef MRK_UNIFORM
#define MRK_UNIFORM(x)														\
RTTR_ENABLE(Uniform)														\
public: x() = default;														\
private: static inline bool _mrk_uniform_##x##_register_ = [](){			\
	UniformHut::Regeister<x>(#x);											\
	return true;															\
	}();																														
#endif // !MRK_UNIFORM

namespace Mrk
{
	class ShaderSetting : public ConfigGroup
	{
		MRK_CONFIG(ShaderSetting)
	public:
		std::string defaultShaderProgramName = "Unlit";
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
		static std::shared_ptr<ShaderProgram> GetShaderProgram(std::string_view spPath);
	private:
		std::map<std::string, std::shared_ptr<ShaderProgram>> sps;
		std::map<std::string, std::function<std::shared_ptr<ShaderProgram>()>> spctors;
	};

	class MaterialHut : public Singleton<MaterialHut>
	{
		MRK_SINGLETON(MaterialHut)
	public:
		static void ReloadMaterial(const std::filesystem::path& matPath);
		static std::shared_ptr<Material> GetMaterial(const std::filesystem::path& matPath);
		static std::shared_ptr<Material> LoadMaterial(const std::filesystem::path& matPath);
		static void SaveMaterial(std::shared_ptr<Material> material, const std::filesystem::path& matPath);
	private:
		std::map<std::string, std::shared_ptr<Material>> materials;
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
		virtual std::shared_ptr<Material> CreateMaterial();	// TODO : 暂时用这种方式，后面改成解析shader来提取uniforms
		virtual std::string_view GetName() = 0;
		GLuint GetId();
		bool Link();
		void Use();
		void AttachShader(GLuint id);
	private:
		GLuint id;
		std::map<std::string, std::weak_ptr<Material>> sharedMaterials;
	};

	class UnlitShaderProgram : public ShaderProgram	// TODO ：暂时用这种方式，后面改成自己解析shader来生成sp
	{
		MRK_SHADERPROGRAM(UnlitShaderProgram, "Unlit")
	public:
		virtual std::shared_ptr<Material> CreateMaterial() override;
	};

	class Uniform;

	class UniformHut : public Singleton<UniformHut>
	{
		MRK_SINGLETON(UniformHut)
	public:
		template<typename T>
		static void Regeister(std::string_view name);
		template<typename T>
		static std::unique_ptr<Uniform> CreateUniform();
		template<typename T, typename ...Args>
		static std::unique_ptr<Uniform> CreateUniform(Args&&... args);
		static std::unique_ptr<Uniform> CreateUniform(std::string_view name);
	private:
		std::map<std::string, std::function<std::unique_ptr<Uniform>()>> ctors;
	};

	class Uniform
	{
		RTTR_ENABLE()
	public:
		Uniform();	// 留给序列化使用
		Uniform(std::string_view name);

		const std::string& GetName();
		void SetName_(const std::string& name);

		void Bind(GLuint sp);

	protected:
		virtual void BindUniform(GLuint sp) = 0;
	protected:
		std::string name;
		GLint location = -1;
	};

	class FloatUniform : public Uniform
	{
		MRK_UNIFORM(FloatUniform)
	public:
		float val = 0;

		FloatUniform(std::string_view name);
	private:
		virtual void BindUniform(GLuint sp);
	};

	enum class TextureUnit : GLuint
	{
		Tex0 = GL_TEXTURE0,
		Tex1 = GL_TEXTURE1,
		Tex2 = GL_TEXTURE2,
		Tex3 = GL_TEXTURE3,
		Tex4 = GL_TEXTURE4,
		Tex5 = GL_TEXTURE5,
		Tex6 = GL_TEXTURE6,
		Tex7 = GL_TEXTURE7,
		Tex8 = GL_TEXTURE8,
		Tex9 = GL_TEXTURE9,
		Tex10 = GL_TEXTURE10,
		Tex11 = GL_TEXTURE11,
		Tex12 = GL_TEXTURE12,
		Tex13 = GL_TEXTURE13
		//Tex14 = GL_TEXTURE14,		// 留给环境贴图
		//Tex15 = GL_TEXTURE15,		// 留给光照贴图
	};

	enum class TextureType : GLuint
	{
		Tex2D = GL_TEXTURE_2D,
		TexCube = GL_TEXTURE_CUBE_MAP
	};

	class UniformTexture2D : public Uniform
	{
		MRK_UNIFORM(UniformTexture2D)
	public:
		UniformTexture2D(std::string_view name, TextureUnit unit);

		const std::string& GetTexturePath();
		void SetTexturePath(const std::string& texturePath);

		TextureUnit GetTextureUnit();
		void SetTextureUnit_(TextureUnit unit);

	private:
		virtual void BindUniform(GLuint sp) override;
	private:
		bool isDirty = true;
		GLuint textureId = 0;
		std::string texturePath;
		TextureUnit textureUnit = TextureUnit::Tex0;
	};

	class Material : public Object
	{
		friend class ShaderProgram;

		MRK_OBJECT(Material) RTTR_ENABLE(Object)
	public:
		Material();
	public:
		void Bind(GLuint sp);

		void AddUniform(std::unique_ptr<Uniform> uniform);
		const std::vector<std::unique_ptr<Uniform>>& GetUniforms();
		virtual Json::Value ToJson(Mrk::JsonAllocator& alloctor);
		virtual void FromJson(const Json::Value& json);
	private:
		std::vector<std::unique_ptr<Uniform>> uniforms;
	};
}

namespace Mrk
{
	template<typename T>
	inline void UniformHut::Regeister(std::string_view name)
	{
		static_assert(std::is_base_of_v<Uniform, T>, "T is not Uniform !");

		MRK_INSTANCE_REF;

		instance.ctors.try_emplace(name.data(), []() {
			return std::make_unique<T>();
			});
	}
	template<typename T>
	inline std::unique_ptr<Uniform> UniformHut::CreateUniform()
	{
		static_assert(std::is_base_of_v<Uniform, T>, "T is not Uniform !");

		return std::make_unique<T>();
	}

	template<typename T, typename ...Args>
	inline std::unique_ptr<Uniform> UniformHut::CreateUniform(Args && ...args)
	{
		static_assert(std::is_base_of_v<Uniform, T>, "T is not Uniform !");

		return std::make_unique<T>(std::forward<Args>(args)...);
	}

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
