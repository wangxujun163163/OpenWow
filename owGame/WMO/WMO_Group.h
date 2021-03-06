﻿#pragma once

// OpenWoW
#include "WoWChunkReader.h"

// WMO Group
#include "WMO_Group_Headers.h"

// Parts
#include "WMO_Liquid.h"
#include "WMO_Part_Portal.h"

#include "WMO_Group_Part_Batch.h"
#include "WMO_Group_Part_BSP_Node.h"

// FORWARD BEGIN
class CWMO;
class CWMO_Group_Instance;
class CWMO_Doodad_Instance;
// FORWARD END

class WMO_Group
	: public CLoadableObject
{
public:
	WMO_Group(IBaseManager& BaseManager, IRenderDevice& RenderDevice, const std::shared_ptr<CWMO>& WMOModel, const uint32 GroupIndex, const SWMO_GroupInfoDef& GroupProto);
	virtual ~WMO_Group();

	// WMO_Group
	const uint32 GetGroupIndex() const;
	void AddPortal(const CWMO_Part_Portal& WMOPartPortal);
	const std::vector<CWMO_Part_Portal>& GetPortals() const;

	// ISceneNodeProvider
	void CreateInsances(const std::shared_ptr<CWMO_Group_Instance>& Parent) const;

	// CLoadableObject
	bool Load() override;


private:
	void FixColors(CBgra* mocv, uint32 mocv_count, const SWMO_Group_BatchDef* moba);

public:
	std::string                             m_GroupName;
	
	SWMO_Group_HeaderDef					m_GroupHeader;
	BoundingBox								m_Bounds;

public:
	//-- Triangles --//
	glm::vec3 * dataFromMOVT;
	std::vector<SWMO_Group_MaterialDef>		m_MaterialsInfo;
	bool									m_IsMOCVExists;

	//-- Render bathes --//
	std::vector<std::shared_ptr<WMO_Group_Part_Batch>> m_WMOBatchIndexes;

	//-- Lights --//
	std::vector<uint16>						m_WMOLightsIndexes;

	//-- Doodads references --//
	std::vector<uint16>						m_DoodadsPlacementIndexes;

	//-- Collision --//
	// MOBN chunk
	// MOBR chunk
	std::vector<uint16>                     collisionIndexes;
	std::shared_ptr<IBuffer>                 VB_Collision;
	std::vector<std::shared_ptr<CWMO_Group_Part_BSP_Node>>	m_CollisionNodes;

	//-- Liquid --//
	std::shared_ptr<CWMO_Liquid>            m_WMOLiqiud;
	

private:
	std::vector<CWMO_Part_Portal> m_Portals;

private:
	IBaseManager& m_BaseManager;
	IRenderDevice& m_RenderDevice;
	const CWMO& m_WMOModel;
	const uint32 m_GroupIndex;
	std::unique_ptr<WoWChunkReader> m_ChunkReader;
};