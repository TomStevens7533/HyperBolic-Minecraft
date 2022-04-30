#pragma once
#include <stdint.h>
#include "Content/BlockJsonParser.h"
#include "ChunkManager.h"
enum class BlockTypes : uint8_t
{
	DIRT, AIR
};
class ChunkMeshComponent;
class ChunkPrefab : public GameObject
{
public:
	ChunkPrefab(XMFLOAT3 chunkPos, const ChunkManager* pchunkmanger, BaseMaterial* pMaterial);
	~ChunkPrefab() override = default;

	ChunkPrefab(const ChunkPrefab& other) = delete;
	ChunkPrefab(ChunkPrefab&& other) noexcept = delete;
	ChunkPrefab& operator=(const ChunkPrefab& other) = delete;
	ChunkPrefab& operator=(ChunkPrefab&& other) noexcept = delete;

	void DrawImGui();
	void UpdateMesh(const SceneContext& scenContext);
	bool DeleteBlock(int x, int y, int z);
	bool IsBlockSolid(int x, int y, int z) const;
	uint8_t GenerateBlockType(int x, int y, int z);

	void SetDirty() { m_NeedUpdate = true; };
	bool GetDirtyFlag() { return m_NeedUpdate; }
protected:
	void Initialize(const SceneContext&) override;
	void Update(const SceneContext&) override;
private:
	uint8_t cubeArray[ChunkSizeY][ChunkSizeX][ChunkSizeZ]{ {{0}} };
	XMFLOAT3 m_ChunkPosition = XMFLOAT3{0,0,0};
	ChunkMeshComponent* m_pChunkComponent;
	const ChunkManager* m_pChunkManager = nullptr;
	bool m_NeedUpdate = false;
	BaseMaterial* m_pBaseMaterial;
};

