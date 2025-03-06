#include "Render.h"

#include "Core/Camera/Camera.h"

void Mrk::RenderSys::Draw()
{
	MRK_INSTANCE_REF;

	if (auto mainCamera = CameraHut::GetMainCamera())
	{
		auto output = mainCamera->GetComponent<CameraOutput>();
		output->Shot(instance.renderLayers);
	}

	for (auto& renderLayer : instance.renderLayers)
	{
		renderLayer.clear();
	}
}

void Mrk::RenderSys::Commit(RenderLayer renderLayer, const RenderItem& renderItem)
{
	MRK_INSTANCE_REF;

	assert((int)renderLayer < instance.renderLayers.size());

	instance.renderLayers[(int)renderLayer].push_back(renderItem);
}
