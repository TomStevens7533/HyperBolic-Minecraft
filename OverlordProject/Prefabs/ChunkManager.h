#pragma once
#include <stdint.h>
#include "Content/BlockJsonParser.h"
#include "Base/Structs.h"
#include "Scenegraph/GameObject.h"
#include <map>
#include <thread>
#include <mutex>

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
	void CreateChunksAroundPos(const SceneContext& sc);
	void SetNewOriginPos(const XMFLOAT3& newOrigin);
	uint8_t RemoveBlock(XMFLOAT3 position, std::tuple<int, int, int>& blockPos);
	bool IsBlockSolid(XMFLOAT3 position) const;
	bool Addblock(XMFLOAT3 position, uint8_t id);
	bool IsBlockInChunkSolid(std::pair<int, int> chunkPos, int x, int y, int z) const;
	bool IsBlockInChunkSolid(XMFLOAT3 pos) const;

	void ReloadNeigbourhingChunks(std::pair<int, int> chunkPos);
	const std::map< Faces, std::vector<XMFLOAT2>>* GetUVOfType(uint8_t id) const;
	const std::string GetNameOfID(uint8_t id) { return m_LevelJsonParser.GetName(id); }

	std::pair<int, int> GetChunkIdx(XMFLOAT3 pos);

protected:
	void Initialize(const SceneContext&) override;
	void Update(const SceneContext& sc) override;
private:
	std::map<std::pair<int, int>, ChunkPrefab*> m_ChunkVec;
	std::map<std::pair<int, int>, ChunkPrefab*> m_TempChunkMap;
	std::jthread m_UpdateChunkThread;
	std::jthread m_CreateChunkThread;

	//m_LevelJsonParser.ParseFile();
	friend ChunkPrefab;
	static BlockJsonParser m_LevelJsonParser;
	unsigned int m_Seed{};
	int m_ChunkDistance = 20;
	DirectX::XMFLOAT3 m_OriginPos;
	std::atomic<float> m_OriginXPos;
	std::atomic<float> m_OriginYPos;
	std::atomic<float> m_OriginZPos;

	ChunkShadowDifffuseMaterial* m_pMaterial = nullptr;
	std::atomic<bool> m_IsShutdown = false;
	std::atomic<bool> m_IsCycleCreateDone = false;
	std::atomic<bool> m_IsCycleUpdateDone = false;

	//Mult
	std::mutex m_MutexCreate;
	std::mutex m_MutexUpdate;

	std::condition_variable cond;

};

