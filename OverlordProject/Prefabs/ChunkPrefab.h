#pragma once
#include <stdint.h>
#include "Content/BlockJsonParser.h"
#include "ChunkManager.h"

#define RandDirtDepth 4
#define TreeChance 0.1f
#define FlowerChance 15.f

#define MinLeavesWidth 3
#define MaxLeavesWidth 4

#define  MaxLeavesHeight 8
#define  MinLeavesHeight 5

#define  BaseTreeLength 5
#define  MaxTreeLength 4
enum class BlockTypes : uint8_t
{
	DIRT, AIR
};
struct ChunkPosistion {
	int x;
	int y;
	int z;
};
class ChunkMeshComponent;
class ChunkPrefab : public GameObject
{
public:
	ChunkPrefab(XMFLOAT3 chunkPos, ChunkManager* pchunkmanger, BaseMaterial* pMaterial, const unsigned int seed);
	~ChunkPrefab() override = default;

	ChunkPrefab(const ChunkPrefab& other) = delete;
	ChunkPrefab(ChunkPrefab&& other) noexcept = delete;
	ChunkPrefab& operator=(const ChunkPrefab& other) = delete;
	ChunkPrefab& operator=(ChunkPrefab&& other) noexcept = delete;

	void DrawImGui();
	void UpdateMesh(const SceneContext& scenContext);

	uint8_t DeleteBlock(int x, int y, int z);
	bool AddBlock(uint8_t id, int x, int y, int z);

	bool IsBlockSolid(int x, int y, int z) const;
	uint8_t GenerateBlockType(int x, int y, int z, int maxHeight, std::vector<ChunkPosistion>& buildTreePos);

	void SetDirty() { m_NeedUpdate = true; };
	bool GetDirtyFlag() { return m_NeedUpdate; }
protected:
	void Initialize(const SceneContext&) override;
	void Update(const SceneContext&) override;
private:
	uint8_t cubeArray[ChunkSizeY][ChunkSizeX][ChunkSizeZ]{ {{0}} };
	XMFLOAT3 m_ChunkPosition = XMFLOAT3{0,0,0};
	ChunkMeshComponent* m_pChunkComponent;
	ChunkManager* m_pChunkManager = nullptr;
	bool m_NeedUpdate = false;
	BaseMaterial* m_pBaseMaterial;
private:
	void BuildTree(int x, int y, int z);
	bool IsIndexInBounds(int x, int y, int z) const;

};

