#include "Render.h"

#include "Core/Camera/Camera.h"

void Mrk::RenderSys::Draw()
{
	auto instance = Instance();

	if (auto mainCamera = CameraHut::GetMainCamera())
	{
		auto output = mainCamera->GetComponent<CameraOutput>();
		output->Shot(instance.renderItems);
	}
}

void Mrk::RenderSys::Commit(RenderLayer renderLayer, const RenderItem& renderParam)
{
	MRK_INSTANCE_REF;

	assert((int)renderLayer < instance.renderItems.size());

	instance.renderItems[(int)renderLayer].push_back(renderParam);
}