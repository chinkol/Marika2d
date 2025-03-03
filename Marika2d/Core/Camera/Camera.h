#pragma once

#include "Common/Singleton/Singleton.h"

#include "Core/Component/Component.h"
#include "Core/GameObject/GameObject.h"
#include "Core/Math/Math.h"
#include "Core/OpenGL/OpenGL.h"

#include <array>
#include <vector>

namespace Mrk
{
	class Camera;

	class CameraHut : public Singleton<CameraHut>
	{
		friend class Camera;
	public:
		static const std::vector<std::weak_ptr<Camera>>& GetCameras();
	private:
		static void AddCamera(std::weak_ptr<Camera> camera);
	private:
		std::weak_ptr<Camera> mainCamera;
		std::vector<std::weak_ptr<Camera>> cameras;
	};

	class CameraOutput : public Component
	{
		MRK_COMPONENT(CameraOutput) MRK_COMPONENT_UNREMOVABLE
	public:
		void Start();
		const Vector2i& GetResolution();
		void SetResolution(const Vector2i& resolution);

		//void ToRu(const std::vector<RenderItem>& items, const std::vector<Light> lights);
	private:
		void ReSize(const Vector2i& newSize);
	private:
		Vector2i resolution;
		Vector2i outputPos = { 0, 0 };

		std::array<GLuint, 2> idTextures;
		std::array<GLuint, 2> backBuffers;
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