#pragma once

enum LightType
{
	OMNI_LGT,
	SPOT_LGT,
	DIRECT_LGT,
	AMBIENT_LGT
};

#include "WMO_Headers.h"

class WMO_Part_Light 
{
public:
	WMO_Part_Light(const SWMO_LightDef& WMOLightProto);

	void setup(uint32 light);

public:
	glm::vec3 m_Position;
	glm::vec4 fcolor;

private:
	const SWMO_LightDef m_WMOLightProto;
};