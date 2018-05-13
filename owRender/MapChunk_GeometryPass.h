#pragma once

#include "technique.h"

class MapChunk_GeometryPass : public Technique {
    int32 gColorMap[4];
    int32 gSpecularMap[4];

    int32 gShadowMapExists;
    int32 gShadowColor;

    int32 gLayersCount;
    int32 gBlend;

public:
	MapChunk_GeometryPass() : Technique("shaders/MapChunk.vs", "shaders/MapChunk.fs")
    {
        for (uint8 i = 0; i < 4; i++)
        {
            gColorMap[i] = getLocation((string("gColorMap[") + std::to_string(i) + "]").c_str());
            gSpecularMap[i] = getLocation((string("gSpecularMap[") + std::to_string(i) + "]").c_str());
        }

        gLayersCount = getLocation("gLayersCount");
        gBlend = getLocation("gBlend");

        gShadowMapExists = getLocation("gShadowMapExists");
        gShadowColor = getLocation("gShadowColor");
    }

	// ---------------------------------------------------

	void SetLayersCount(int _layersCount)
	{
		setInt(gLayersCount, _layersCount);
	}

	void SetColorTextureUnit(int _number, int TextureUnit)
	{
		setTexture(gColorMap[_number], TextureUnit);
	}

    void SetSpecularTextureUnit(int _number, int TextureUnit)
    {
        setTexture(gSpecularMap[_number], TextureUnit);
    }

	// ----------------------------------------------------

	void SetBlendBuffer(int TextureUnit)
	{
		setTexture(gBlend, TextureUnit);
	}

	// ----------------------------------------------------


	void SetShadowMapExists(bool _exists)
	{
		setInt(gShadowMapExists, _exists);
	}

	void SetShadowColor(vec3 _shadowColor)
	{
		setVec3(gShadowColor, _shadowColor);
	}
};