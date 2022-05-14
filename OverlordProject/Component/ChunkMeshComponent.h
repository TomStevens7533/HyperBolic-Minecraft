#pragma once
#include "Content/ChunkStructs.h"

class ChunkMeshFilter;
class ChunkMeshComponent : public BaseComponent
{
public:
	ChunkMeshComponent(BaseMaterial* pMaterial);
	~ChunkMeshComponent() override;

	ChunkMeshComponent(const ChunkMeshComponent& other) = delete;
	ChunkMeshComponent(ChunkMeshComponent&& other) noexcept = delete;
	ChunkMeshComponent& operator=(const ChunkMeshComponent& other) = delete;
	ChunkMeshComponent& operator=(ChunkMeshComponent&& other) noexcept = delete;

	void SetMaterial(BaseMaterial* pMaterial);
	void SetMaterial(UINT materialId);

	bool AddFace(XMFLOAT3 chunkPos, XMFLOAT3 localBlockPos, Faces dir, const std::vector<XMFLOAT2>* uv, bool isCube);
	void BufferMesh(const SceneContext& gameContext);
	void ResetMesh();
protected:
	void Initialize(const SceneContext& sceneContext) override;
	void Update(const SceneContext&) override;
	void Draw(const SceneContext& sceneContext) override;
private:
	friend class ChunkMeshFilter;
private:
	BaseMaterial* m_pMaterial{};
	bool m_MaterialChanged{};
	std::atomic<bool> m_IsInitialized = false;
	ChunkMeshFilter* m_pChunkMeshFilter{};
};
