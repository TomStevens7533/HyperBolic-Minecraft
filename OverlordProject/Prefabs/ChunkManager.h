#pragma once
#include <stdint.h>
#include "Content/BlockJsonParser.h"
#include "Base/Structs.h"
#include "Scenegraph/GameObject.h"
#include <map>
#include <thread>

#define ChunkSizeX 16
#define ChunkSizeY 256
#define ChunkSizeZ 16
#define ChunkMaxHeightGeneration 150
#define ChunkBaseTerrainHeight 30
#define ChunkWaterHeight 40
class ChunkPrefab;
class ChunkShadowDifffuseMaterial;
class ChunkManager : public GameObject
{
public:
	ChunkManager(DirectX::XMFLOAT3 originPos = XMFLOAT3{0,0,0});
	~ChunkManager() override;

	ChunkManager(const ChunkManager& other) = delete;
	ChunkManager(ChunkManager&& other) noexcept = delete;
	ChunkManager& operator=(const ChunkManager& other) = delete;
	ChunkManager& operator=(ChunkManager&& other) noexcept = delete;

	void DrawImGui();
	void UpdateChunksAroundPos(const SceneContext& sc);
	void SetNewOriginPos(const XMFLOAT3& newOrigin);
	uint8_t RemoveBlock(XMFLOAT3 position);
	bool IsBlockSolid(XMFLOAT3 position) const;
	bool Addblock(XMFLOAT3 position, uint8_t id);
	bool IsBlockInChunkSolid(std::pair<int, int> chunkPos, int x, int y, int z) const;
	void ReloadNeigbourhingChunks(std::pair<int, int> chunkPos);
	const std::map< Faces, std::vector<XMFLOAT2>>* GetUVOfType(uint8_t id) const;
	const std::string GetNameOfID(uint8_t id) { return m_LevelJsonParser.GetName(id); }
protected:
	void Initialize(const SceneContext&) override;
	void Update(const SceneContext& sc) override;
private:
	std::map<std::pair<int, int>, ChunkPrefab*> m_ChunkVec;
	std::jthread m_UpdateChunkThread;
	//m_LevelJsonParser.ParseFile();
	friend ChunkPrefab;
	static BlockJsonParser m_LevelJsonParser;
	int m_ChunkDistance = 10;
	DirectX::XMFLOAT3 m_OriginPos;
	ChunkShadowDifffuseMaterial* m_pMaterial = nullptr;
	std::atomic<bool> m_IsShutdown = false;
};

