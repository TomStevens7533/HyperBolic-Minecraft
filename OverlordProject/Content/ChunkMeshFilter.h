#pragma once
#include "Base/Structs.h"

struct VertexChunk
{
	XMFLOAT3 position{};
	XMFLOAT2 uv{};

};
class BaseMaterial;
class ChunkMeshFilter final
{
public:
	ChunkMeshFilter(BaseMaterial* pMaterial);
	~ChunkMeshFilter();
	ChunkMeshFilter(const ChunkMeshFilter& other) = delete;
	ChunkMeshFilter(ChunkMeshFilter&& other) noexcept = delete;
	ChunkMeshFilter& operator=(const ChunkMeshFilter& other) = delete;
	ChunkMeshFilter& operator=(ChunkMeshFilter&& other) noexcept = delete;

	void AddFaceToMesh(std::vector<XMFLOAT3>& verticesToAdd, const std::vector<XMFLOAT2>* uv);
	void UpdateBuffer(const SceneContext& gameContext);
private:
	friend class ChunkMeshComponent;

	void SetElement(ILSemantic element) { m_Elements |= element; }
	bool HasElement(ILSemantic element) const { return isSet(m_Elements, element); }
	void Draw(const SceneContext& sceneContext);
	//const VertexBufferData& GetVertexBufferData(const SceneContext& sceneContext, BaseMaterial* pMaterial);
	void UpdateBufferBIG(const SceneContext& context, BaseMaterial* pMaterial);
	void CreateChunkVertices(void* pBuffer);
	void CreateChunkIndices(void* pBuffer);


	//VERTEX DATA
	UINT m_VertexCount{}, m_IndexCount{}, m_TexCoordCount{}, m_BufferSize{}, m_IndexBufferSize{};
	UINT m_TempVertexCount{}, m_TempIndexCount{}, m_TempTexCoordCount{};

	ILSemantic m_Elements{ ILSemantic::NONE };
	std::vector<XMFLOAT3> m_Positions{};
	std::vector<XMFLOAT3> m_TempPositions{};

	std::vector<XMFLOAT2> m_TexCoords{};
	std::vector<XMFLOAT2> m_TempTexCoords{};

	ID3D11InputLayout* m_pInputLayout{};
	BaseMaterial* m_pMaterial;
	//INDEX DATA
	std::vector<UINT> m_Indices{};
	std::vector<UINT> m_TempIndices{};

	VertexBufferData m_VertexBuffer{};
	ID3D11Buffer* m_pIndexBuffer{};
	std::atomic<bool> m_IsUpdated = false;
	bool m_Iscreated = false;
};

