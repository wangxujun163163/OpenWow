#include "stdafx.h"

// General
#include "TechniquesManager.h"

// Additional
#include "Gbuffer.h"

TechniquesMgr::TechniquesMgr()
{
	m_MapChunk_GeometryPass = nullptr;
	m_MapTileLowRes_GeometryPass = nullptr;
	m_MDX_GeometryPass = nullptr;
	m_WMO_GeometryPass = nullptr;
	m_SimpleRender = nullptr;
}

TechniquesMgr::~TechniquesMgr()
{
	delete m_MapChunk_GeometryPass;
	delete m_MapTileLowRes_GeometryPass;
	delete m_MDX_GeometryPass;
	delete m_WMO_GeometryPass;

	delete m_SimpleRender;
}

void TechniquesMgr::Init()
{
	m_Billboard = new Billboard_GeometryPass();
	m_Billboard->Init();
	m_Billboard->Bind();
	m_Billboard->SetColorTextureUnit(COLOR_TEXTURE_UNIT_INDEX);
	m_Billboard->SetSpecularTextureUnit(SPECULAR_TEXTURE_UNIT_INDEX);
	m_Billboard->Unbind();

	//

	m_Sky_GeometryPass = new Sky_GeometryPass();
	m_Sky_GeometryPass->Init();
	m_Sky_GeometryPass->Bind();
	m_Sky_GeometryPass->Unbind();

	//----------------------------------------------------------------//

	m_MapChunk_GeometryPass = new MapChunk_GeometryPass();
	m_MapChunk_GeometryPass->Init();
	m_MapChunk_GeometryPass->Bind();

	m_MapChunk_GeometryPass->SetColorTextureUnit0(0);
	m_MapChunk_GeometryPass->SetColorTextureUnit1(1);
	m_MapChunk_GeometryPass->SetColorTextureUnit2(2);
	m_MapChunk_GeometryPass->SetColorTextureUnit3(3);

	m_MapChunk_GeometryPass->SetBlendBuffer(4);

	m_MapChunk_GeometryPass->SetSpecularTextureUnit(SPECULAR_TEXTURE_UNIT_INDEX);
	m_MapChunk_GeometryPass->Unbind();

	//----------------------------------------------------------------//

	m_MapTileLowRes_GeometryPass = new MapTileLowRes_GeometryPass();
	m_MapTileLowRes_GeometryPass->Init();
	m_MapTileLowRes_GeometryPass->Bind();
	m_MapTileLowRes_GeometryPass->Unbind();

	//----------------------------------------------------------------//

	m_MDX_GeometryPass = new MDX_GeometryPass();
	m_MDX_GeometryPass->Init();
	m_MDX_GeometryPass->Bind();
	m_MDX_GeometryPass->SetColorTextureUnit(COLOR_TEXTURE_UNIT_INDEX);
	m_MDX_GeometryPass->SetSpecularTextureUnit(SPECULAR_TEXTURE_UNIT_INDEX);
	m_MDX_GeometryPass->Unbind();

	//
	m_WMO_MH2O_GeometryPass = new WMO_MH2O_GeometryPass();
	m_WMO_MH2O_GeometryPass->Init();
	m_WMO_MH2O_GeometryPass->Bind();
	m_WMO_MH2O_GeometryPass->SetColorTextureUnit(COLOR_TEXTURE_UNIT_INDEX);
	m_WMO_MH2O_GeometryPass->SetSpecularTextureUnit(SPECULAR_TEXTURE_UNIT_INDEX);
	m_WMO_MH2O_GeometryPass->Unbind();

	//----------------------------------------------------------------//

	m_WMO_GeometryPass = new WMO_GeomertyPass();
	m_WMO_GeometryPass->Init();
	m_WMO_GeometryPass->Bind();
	m_WMO_GeometryPass->SetColorTextureUnit(COLOR_TEXTURE_UNIT_INDEX);
	m_WMO_GeometryPass->SetSpecularTextureUnit(SPECULAR_TEXTURE_UNIT_INDEX);
	m_WMO_GeometryPass->Unbind();

	//----------------------------------------------------------------//

	//----------------------------------------------------------------//
	m_DSDirLightPassTech = new DSDirLightPassTech();
	m_DSDirLightPassTech->Init();
	m_DSDirLightPassTech->Bind();

	m_DSDirLightPassTech->SetProjectionMatrix(glm::mat4());
	m_DSDirLightPassTech->SetViewMatrix(glm::mat4());
	m_DSDirLightPassTech->SetWorldMatrix(glm::mat4());

	m_DSDirLightPassTech->BindToPostprocess();

	m_DSDirLightPassTech->SetScreenSize(_Settings->windowSizeX, _Settings->windowSizeY);
	m_DSDirLightPassTech->SetMatSpecularPower(64);
	
	m_DSDirLightPassTech->Unbind();

	//----------------------------------------------------------------//

	/*m_SimpleRender = new SimpleRenderGBuffer();
	m_SimpleRender->Init();
	m_SimpleRender->Bind();

	m_SimpleRender->SetProjectionMatrix(glm::mat4(1.0f));
	m_SimpleRender->SetViewMatrix(glm::mat4(1.0f));
	m_SimpleRender->SetWorldMatrix(glm::mat4(1.0f));

	m_DSDirLightPassTech->BindToPostprocess();

	m_SimpleRender->SetScreenSize(_Settings->windowSizeX, _Settings->windowSizeY);
	m_SimpleRender->Unbind();*/
}
