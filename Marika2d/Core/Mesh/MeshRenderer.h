#pragma once

#include "Core/Component/Component.h"

#include "Core/Scene/Scene.h"

namespace Mrk
{
	class MeshRenderer : public Component
	{
		MRK_COMPONENT(MeshRenderer)
		MRK_POOLABLE(MeshRenderer, 100)
	public:
		void Awake()
		{
			//TODO : push meshrenderer to scene octTree
		}
	};
}


