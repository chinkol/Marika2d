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

void Mrk::RenderSys::Commit(const RenderItem& renderItem)
{
	MRK_INSTANCE_REF;

	if (renderItem.mat && renderItem.mesh && renderItem.sp)
	{
		instance.spGroups[renderItem.sp][renderItem.mat][renderItem.mesh].push_back(renderItem);
	}
}