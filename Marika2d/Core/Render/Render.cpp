#include "Render.h"

#include "Core/Camera/Camera.h"
#include "Core/Mesh/Mesh.h"

void Mrk::RenderSys::Draw()
{
    MRK_INSTANCE_REF;

	if (auto mainCamera = CameraHut::GetMainCamera())
	{
		auto output = mainCamera->GetComponent<CameraOutput>();
		output->Shot(instance.spGroups);
	}

	instance.spGroups.clear();
}

void Mrk::RenderSys::Commit(std::shared_ptr<RenderItem> renderItem)
{
	MRK_INSTANCE_REF;

	if (!renderItem || !renderItem->mat || !renderItem->mesh)
		return;

    if (auto shader = renderItem->mat->GetShaderProgram())
    {
		instance.spGroups[shader][renderItem->mat][renderItem->mesh].push_back(renderItem);
    }
}
