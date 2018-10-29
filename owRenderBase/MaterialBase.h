#pragma once

#include "Material.h"
class Texture;
class ConstantBuffer;

class MaterialBase : public Material
{
public:
	enum class TextureType : uint8
	{
		Ambient = 0,
		Emissive = 1,
		Diffuse = 2,
		Specular = 3,
		SpecularPower = 4,
		Normal = 5,
		Bump = 6,
		Opacity = 7,
	};

public:
	MaterialBase(RenderDevice* renderDevice);
	virtual ~MaterialBase();

	cvec4 GetDiffuseColor() const;
	void SetDiffuseColor(cvec4 diffuse);

	cvec4 GetGlobalAmbientColor() const;
	void SetGlobalAmbientColor(cvec4 globalAmbient);

	cvec4 GetAmbientColor() const;
	void SetAmbientColor(cvec4 ambient);

	cvec4 GetEmissiveColor() const;
	void SetEmissiveColor(cvec4 emissive);

	cvec4 GetSpecularColor() const;
	void SetSpecularColor(cvec4 phong);

	float GetSpecularPower() const;
	void SetSpecularPower(float phongPower);

	cvec4 GetReflectance() const;
	void SetReflectance(cvec4 reflectance);

	const float GetOpacity() const;
	void SetOpacity(float Opacity);

	float GetIndexOfRefraction() const;
	void SetIndexOfRefraction(float indexOfRefraction);

	// When using bump maps, we can adjust the "intensity" of the normals generated
	// from the bump maps. We can even inverse the normals by using a negative intensity.
	// Default bump intensity is 1.0 and a value of 0 will remove the bump effect altogether.
	float GetBumpIntensity() const;
	void SetBumpIntensity(float bumpIntensity);

	std::shared_ptr<Texture> GetTexture(TextureType ID) const;
	void SetTexture(TextureType type, std::shared_ptr<Texture> texture);

	// This material defines a transparent material 
	// if the opacity value is < 1, or there is an opacity map, or the diffuse texture has an alpha channel.
	bool IsTransparent() const;

private:
	void UpdateConstantBuffer() const override;

	// ConstantBuffer
	__declspec(align(16)) struct MaterialProperties
	{
		MaterialProperties()
			: m_GlobalAmbient(0.1f, 0.1f, 0.15f, 1)
			, m_AmbientColor(0, 0, 0, 1)
			, m_EmissiveColor(0, 0, 0, 1)
			, m_DiffuseColor(1, 1, 1, 1)
			, m_SpecularColor(0, 0, 0, 1)
			, m_Reflectance(0, 0, 0, 0)
			, m_Opacity(1.0f)
			, m_SpecularPower(-1.0f)
			, m_IndexOfRefraction(-1.0f)
			, m_HasAmbientTexture(false)
			, m_HasEmissiveTexture(false)
			, m_HasDiffuseTexture(false)
			, m_HasSpecularTexture(false)
			, m_HasSpecularPowerTexture(false)
			, m_HasNormalTexture(false)
			, m_HasBumpTexture(false)
			, m_HasOpacityTexture(false)
			, m_BumpIntensity(5.0f)
			, m_SpecularScale(128.0f)
			, m_AlphaThreshold(0.1f)
		{}

		vec4   m_GlobalAmbient;
		//-------------------------- ( 16 bytes )
		vec4   m_AmbientColor;
		//-------------------------- ( 16 bytes )
		vec4   m_EmissiveColor;
		//-------------------------- ( 16 bytes )
		vec4   m_DiffuseColor;
		//-------------------------- ( 16 bytes )
		vec4   m_SpecularColor;
		//-------------------------- ( 16 bytes )
		vec4   m_Reflectance;
		//-------------------------- ( 16 bytes )
		// If Opacity < 1, then the material is transparent.
		float       m_Opacity;
		float       m_SpecularPower;
		// For transparent materials, IOR > 0.
		float       m_IndexOfRefraction;
		uint32_t    m_HasAmbientTexture;
		//-------------------------- ( 16 bytes )
		uint32_t    m_HasEmissiveTexture;
		uint32_t    m_HasDiffuseTexture;
		uint32_t    m_HasSpecularTexture;
		uint32_t    m_HasSpecularPowerTexture;
		//-------------------------- ( 16 bytes )
		uint32_t    m_HasNormalTexture;
		uint32_t    m_HasBumpTexture;
		uint32_t    m_HasOpacityTexture;
		float       m_BumpIntensity;    // When using bump textures (heightmaps) we need 
										// to scale the height values so the normals are visible.
		//-------------------------- ( 16 bytes )
		float       m_SpecularScale;    // When reading specular power from a texture, 
										// we need to scale it into the correct range.
		float       m_AlphaThreshold;   // Pixels with alpha < m_AlphaThreshold will be discarded.
		vec2        m_Padding;          // Pad to 16 byte boundary.
		//-------------------------- ( 16 bytes )
	};  //--------------------------- ( 16 * 10 = 160 bytes )
	MaterialProperties* m_pProperties;
};