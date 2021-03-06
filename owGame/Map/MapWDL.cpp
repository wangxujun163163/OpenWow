#include "stdafx.h"

// Include
#include "Map.h"
#include "WDL_Node_Material.h"

// General
#include "MapWDL.h"

// Additional
#include "WoWChunkReader.h"

CMapWDL::CMapWDL(IBaseManager& BaseManager, IRenderDevice& RenderDevice, const CMap& Map)
	: m_Minimap(nullptr)
	, m_BaseManager(BaseManager)
	, m_RenderDevice(RenderDevice)
	, m_MapController(Map)
{}

CMapWDL::~CMapWDL()
{
}

//--

//
// ISceneNodeProvider
//
void CMapWDL::CreateInsances(const std::shared_ptr<ISceneNode3D>& Parent) const
{
	std::string fileName = m_MapController.GetMapFolder() + ".wdl";

	// Low-resolution tiles
	std::shared_ptr<IFile> f = m_BaseManager.GetManager<IFilesManager>()->Open(fileName);
	if (f == nullptr)
	{
		Log::Error("World[%s]: WDL: Error opening.", fileName.c_str());
		return;
	}

	// Material
	m_LowResilutionTileMaterial = std::make_shared<WDL_Node_Material>(m_RenderDevice);

	// Heightmap
	glm::vec3 lowres[17][17];
	glm::vec3 lowsub[16][16];

	for (uint8 j = 0; j < C_TilesInMap; j++)
	{
		for (uint8 i = 0; i < C_TilesInMap; i++)
		{
			if (m_MAREOffsets[j][i])
			{
				// Read data             
				f->seek(m_MAREOffsets[j][i] + 4 + 4);

				int16 tilebuf[17 * 17];
				f->readBytes(tilebuf, 17 * 17 * 2);

				int16 tilebuf2[16 * 16];
				f->readBytes(tilebuf2, 16 * 16 * 2);

				for (int y = 0; y < 17; y++)
					for (int x = 0; x < 17; x++)
						lowres[y][x] = glm::vec3(C_TileSize*(i + x / 16.0f), tilebuf[y * 17 + x], C_TileSize*(j + y / 16.0f));

				for (int y = 0; y < 16; y++)
					for (int x = 0; x < 16; x++)
						lowsub[y][x] = glm::vec3(C_TileSize*(i + (x + 0.5f) / 16.0f), tilebuf2[y * 16 + x], C_TileSize*(j + (y + 0.5f) / 16.0f));


				std::vector<glm::vec3> vecrtices;

				for (int y = 0; y < 16; y++)
				{
					for (int x = 0; x < 16; x++)
					{
						vecrtices.push_back(lowres[y][x]);
						vecrtices.push_back(lowsub[y][x]);
						vecrtices.push_back(lowres[y][x + 1]);

						vecrtices.push_back(lowres[y][x + 1]);
						vecrtices.push_back(lowsub[y][x]);
						vecrtices.push_back(lowres[y + 1][x + 1]);

						vecrtices.push_back(lowres[y + 1][x + 1]);
						vecrtices.push_back(lowsub[y][x]);
						vecrtices.push_back(lowres[y + 1][x]);

						vecrtices.push_back(lowres[y + 1][x]);
						vecrtices.push_back(lowsub[y][x]);
						vecrtices.push_back(lowres[y][x]);
					}
				}

				// Vertex buffer
				std::shared_ptr<IBuffer> __vb = m_RenderDevice.GetObjectsFactory().CreateVertexBuffer(vecrtices);

				std::shared_ptr<IGeometry> geometry = m_RenderDevice.GetObjectsFactory().CreateGeometry();
				geometry->SetVertexBuffer(__vb);		
				
				std::shared_ptr<CWDL_LowResTile> lowResTile = std::make_shared<CWDL_LowResTile>(m_RenderDevice, m_MapController, i, j);
				lowResTile->AddConnection(m_LowResilutionTileMaterial, geometry);
				Parent->GetComponent<IModelsComponent3D>()->AddModel(lowResTile);
			}
		}
	}

	// Load low-resolution WMOs
	Log::Green("Map_GlobalWMOs[]: Low WMOs count [%d].", m_LowResolutionWMOsPlacementInfo.size());
	for (auto it : m_LowResolutionWMOsPlacementInfo)
	{
		//_ASSERT(false);
		//CMapWMOInstance* wmoInstance = _parent->CreateSceneNode<CMapWMOInstance>(m_LowResolutionWMOsNames[it.nameIndex], it);
		//m_LowResolutionWMOs.push_back(wmoInstance);
	}
}

void CMapWDL::UpdateCamera(const ICameraComponent3D * camera)
{
	//if (m_LowResilutionTileMaterial)
	//	m_LowResilutionTileMaterial->SetDiffuseColor(vec4(GetManager<ISkyManager>()->GetColor(LightColors::LIGHT_COLOR_FOG), 1.0f));
}

void CMapWDL::Load()
{
	// Offsets to MARE
	memset(m_MAREOffsets, 0x00, C_TilesInMap * C_TilesInMap * sizeof(uint32));

	std::shared_ptr<IByteBuffer> bytes = m_BaseManager.GetManager<IFilesManager>()->Open(m_MapController.GetMapFolder() + ".wdl");

	WoWChunkReader reader(m_BaseManager, bytes);


	if (auto buffer = reader.OpenChunk("MVER"))
	{
		uint32 version;
		buffer->readBytes(&version, 4);
		_ASSERT_EXPR(version == 18, "Version mismatch != 18");
	}
	if (auto buffer = reader.OpenChunk("MWMO")) // Filenames for WMO that appear in the low resolution map. Zero terminated strings.
	{
		//WOWCHUNK_READ_STRINGS_BEGIN
		//	m_LowResolutionWMOsNames.push_back(_string);
		//WOWCHUNK_READ_STRINGS_END;
	}
	if (auto buffer = reader.OpenChunk("MWID")) // List of indexes into the MWMO chunk.
	{
	}
	for (const auto& placement : reader.OpenChunkT<ADT_MODF>("MODF")) // Placement information for the WMO. Appears to be the same 64 byte structure used in the WDT and ADT MODF chunks.
	{
		m_LowResolutionWMOsPlacementInfo.push_back(placement);
	}
	if (auto buffer = reader.OpenChunk("MAOF")) // Contains 64*64 = 4096 unsigned 32-bit integers, these are absolute offsets in the file to each map tile's MapAreaLow-array-entry. For unused tiles the value is 0.
	{
		buffer->readBytes(m_MAREOffsets, C_TilesInMap * C_TilesInMap * sizeof(uint32));
	}
	//if (auto buffer = reader.OpenChunk("MARE")) // Heightmap for one map tile.
	//{
		// Contains 17 * 17 + 16 * 16 = 545 signed 16-bit integers. So a 17 by 17 grid of height values is given, with additional height values in between grid points. Here, the "outer" 17x17 points are listed (in the usual row major order), followed by 16x16 "inner" points. The height values are on the same scale as those used in the regular height maps.
	//}
	//if (auto buffer = reader.OpenChunk("MAHO"))
	//{
		// After each MARE chunk there follows a MAHO (MapAreaHOles) chunk. It may be left out if the data is supposed to be 0 all the time. Its an array of 16 shorts. Each short is a bitmask. If the bit is not set, there is a hole at this position.
	//}

	// Minimap
	std::shared_ptr<CImageBase> mimimapImage = std::make_shared<CImageBase>(512, 512, 32, false);

	// Heightmap
	glm::vec3 lowres[17][17];

	for (uint8 j = 0; j < C_TilesInMap; j++)
	{
		for (uint8 i = 0; i < C_TilesInMap; i++)
		{
			if (m_MAREOffsets[j][i])
			{
				// Read data             
				bytes->seek(m_MAREOffsets[j][i] + 4 + 4);

				int16 tilebuf[17 * 17];
				bytes->readBytes(tilebuf, 17 * 17 * 2);

				// make minimap
				for (int z = 0; z < 8; z++)
				{
					for (int x = 0; x < 8; x++)
					{
						short hval = tilebuf[(z * 2) * 17 + x * 2]; // for now
						uint8 r, g, b;
						if (hval < 0)
						{
							// water = blue
							if (hval < -511) 
								hval = -511;
							hval /= -2;
							r = g = 0;
							b = 255 - hval;
						}
						else
						{
							uint8 r1, r2, g1, g2, b1, b2;
							float t;

							if (hval < 600)
							{
								r1 = 20;
								r2 = 137;
								g1 = 149;
								g2 = 84;
								b1 = 7;
								b2 = 21;
								t = hval / 600.0f;
							}
							else if (hval < 1200)
							{
								r2 = 96;
								r1 = 137;
								g2 = 96;
								g1 = 84;
								b2 = 96;
								b1 = 21;
								t = (hval - 600) / 600.0f;
							}
							else /*if (hval < 1600)*/
							{
								r1 = 96;
								r2 = 255;
								g1 = 96;
								g2 = 255;
								b1 = 96;
								b2 = 255;
								if (hval >= 1600) hval = 1599;
								t = (hval - 1200) / 600.0f;
							}

							// TODO: add a regular palette here

							r = (uint8)(r2*t + r1 * (1.0f - t));
							g = (uint8)(g2*t + g1 * (1.0f - t));
							b = (uint8)(b2*t + b1 * (1.0f - t));
						}

						mimimapImage->GetPixel<uint32>(i * 8 + x, j * 8 + z) = (r) | (g << 8) | (b << 16) | (255 << 24);
					}
				}
			}
		}
	}

	// Finish minimap
	m_Minimap = m_RenderDevice.GetObjectsFactory().CreateEmptyTexture();
	m_Minimap->LoadTextureFromImage(mimimapImage);
}