#pragma once

#include "Core/Component/Component.h"
#include "Core/GameObject/GameObject.h"
#include "Common/Singleton/Singleton.h"

#include "Third/glm/glm.hpp"

namespace Mrk
{
	class Camera;

	class CameraHouse : public Singleton<CameraHouse>
	{
	public:
		const std::vector<std::weak_ptr<Camera>>& GetCameras()
		{

		}
	private:
		std::vector<std::weak_ptr<Camera>> cameras;
	};

	class CameraScreenOutput
	{
	public:
		glm::i32vec2 GetResolution()
		{
			return resolution;
		}
		void SetResolution(glm::i32vec2 resolution)
		{
			this->resolution = resolution;
			//TODO : Resize
		}

		//void ToRu(const std::vector<RenderItem>& items, const std::vector<Light> lights);
	private:
		glm::i32vec2 resolution;
		glm::i32vec2 outputPos;
		//TODO : add framebuffer
	};

	class Camera : public GameObject
	{
		MRK_GAMEOBJECT(Camera)
	public:
		CameraScreenOutput output;
	};
}

