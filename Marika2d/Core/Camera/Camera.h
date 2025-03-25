#pragma once

#include "Common/Singleton/Singleton.h"

#include "Core/Component/Component.h"
#include "Core/GameObject/GameObject.h"
#include "Core/Math/Math.h"
#include "Core/OpenGL/OpenGL.h"
#include "Core/Render/Render.h"

#include <array>
#include <vector>

namespace Mrk
{
	class Camera;

	class CameraHut : public Singleton<CameraHut>
	{
		friend class Camera;
	public:
		static std::shared_ptr<Camera> GetMainCamera();
		static const std::vector<std::weak_ptr<Camera>>& GetCameras();
	private:
		static void AddCamera(std::weak_ptr<Camera> camera);
	private:
		std::weak_ptr<Camera> mainCamera;
		std::vector<std::weak_ptr<Camera>> cameras;
	};

	class CameraFrustum
	{
	public:
		float GetNearZ() const;
		void SetNearZ(float nearZ);
		float GetFarZ() const;
		void SetFarZ(float farZ);
		float GetAngle() const;
		void SetAngle(float angle);
		float GetAspect() const;
		void SetAspect(float aspect);
		const Matrix4& GetProjMatrix();
	private:
		Matrix4 proj;
		float nearZ = 0.1f;
		float farZ = 100.0f;
		float angle = 45.0f;
		float aspect = 16.0f / 9.0f;
		bool isDirty = true;
	};

	class CameraController : public Component
	{
		MRK_COMPONENT(CameraController)
	public:
		void Start();
		void Update();
	private:
		bool FindCamera();
	private:
		std::shared_ptr<Transform> trans;	// 自身 的 trans ，引用transform一般不会造成循环引用
		std::shared_ptr<Transform> cameraTrans;
	};

	class CameraOutput : public Component
	{
		MRK_COMPONENT(CameraOutput) MRK_COMPONENT_UNREMOVABLE
	public:
		~CameraOutput();

		void Start();
		void PreDraw();

		const Vector2i& GetResolution() const;
		void SetResolution(const Vector2i& resolution);

		const CameraFrustum& GetFrustum() const;
		void SetFrustum_(const CameraFrustum& frumstum);

		GLuint GetBackBuffer();
		GLuint GetIdTexture();
		GLuint GetBackBuffertexture();
		GLuint GetDepthBuffertexture();

		void Shot(const RenderSpGroups& spGroups);
	private:
		void ReSize(const Vector2i& newSize);
		//void Shot(const std::vector<RenderItem>& renderItems);
	private:
		Vector2i resolution;
		Vector2i outputPos = { 0, 0 };

		CameraFrustum frustum;
		std::shared_ptr<Transform> trans;	// 缓存camera的trans, 这里不会构成循环引用

		int currBackBufferIndex = 0;
		std::array<GLuint, 2> backBuffers;
		std::array<GLuint, 2> idTextures;
		std::array<GLuint, 2> backBufferTextures;
		std::array<GLuint, 2> depthBufferTextures;
	};

	class Camera : public GameObject
	{
		MRK_GAMEOBJECT(Camera)
	public:
		virtual void Init() override;
	};
}