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

	class CameraOutput : public Component
	{
		MRK_COMPONENT(CameraOutput) MRK_COMPONENT_UNREMOVABLE
	public:
		~CameraOutput();

		void Start();
		void PreDraw();
		const Vector2i& GetResolution();
		void SetResolution(const Vector2i& resolution);

		GLuint GetBackBuffer();
		GLuint GetIdTexture();
		GLuint GetBackBuffertexture();
		GLuint GetDepthBuffertexture();

		void Shot(const std::array<std::vector<RenderItem>, 4>& renderLayers);
	private:
		void ReSize(const Vector2i& newSize);
		void Shot(const std::vector<RenderItem>& renderItems);
	private:
		Vector2i resolution;
		Vector2i outputPos = { 0, 0 };

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
