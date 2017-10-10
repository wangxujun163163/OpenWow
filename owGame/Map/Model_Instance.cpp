#include "../stdafx.h"

// General
#include "Model_Instance.h"

ModelInstance::ModelInstance(File& f)
{
	placementInfo = new ModelPlacementInfo;
	f.ReadBytes(placementInfo, ModelPlacementInfo::__size);

	// Convert rotation
	placementInfo->rotation = degToRad(placementInfo->rotation);
	placementInfo->rotation.x = -placementInfo->rotation.x;
	placementInfo->rotation.y = placementInfo->rotation.y - PI / 2.0;

	sc = static_cast<float>(placementInfo->scale) / 1024.0f;
}

ModelInstance::~ModelInstance()
{
	delete placementInfo;
}

void ModelInstance::draw()
{
	_Pipeline->Clear();
	{
		_Pipeline->Translate(placementInfo->position);

		_Pipeline->RotateX(placementInfo->rotation.z);
		_Pipeline->RotateY(placementInfo->rotation.y);
		_Pipeline->RotateZ(placementInfo->rotation.x);

		_Pipeline->Scale(sc);

		BoundingBox aabb = modelObject->m_Bounds;
		aabb.transform(_Pipeline->GetWorld());

		//float dist = (placementInfo->position - _Camera->Position).length();
		//if (dist > Modules::config().modeldrawdistance + modelObject->m_Radius * sc)
		//{
		//	return;
		//}

		if (_CameraFrustum->_frustum.cullBox(aabb))
		{
			return;
		}


		modelObject->draw();
		PERF_INC(PERF_MAP_MODELS_MDXs);
	}
}