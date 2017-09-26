#pragma once

#include "technique.h"

class MDX_GeometryPass : public Technique
{
public:
	MDX_GeometryPass();
	virtual bool Init();

	//

	inline void SetColorTextureUnit(int TextureUnit)
	{
		setInt("gColorMap", TextureUnit);
	}

	inline void SetSpecularTextureUnit(int TextureUnit)
	{
		setInt("gSpecularMap", TextureUnit);
	}
};