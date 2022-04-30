#pragma once
#include "Base/Structs.h"
class ChunkMeshFilter final
{
public:
	ChunkMeshFilter();
	~ChunkMeshFilter();
	ChunkMeshFilter(const ChunkMeshFilter& other) = delete;
	ChunkMeshFilter(ChunkMeshFilter&& other) noexcept = delete;
	ChunkMeshFilter& operator=(const ChunkMeshFilter& other) = delete;
	ChunkMeshFilter& operator=(ChunkMeshFilter&& other) noexcept = delete;

	void AddFaceToMesh(std::vector<XMFLOAT3>& verticesToAdd, const std::vector<XMFLOAT2>* uv);
	void UpdateBuffer(const SceneContext& gameContext, BaseMaterial* pMaterial);
private:
	friend class ChunkMeshComponent;

	void BuildVertexBuffer(const SceneContext& gameContext, BaseMaterial* pMaterial);
	void BuildIndexBuffer(const SceneContext& gameContext);

	void SetElement(ILSemantic element) { m_Elements |= element; }
	bool HasElement(ILSemantic element) const { return isSet(m_Elements, element); }

	const VertexBufferData& GetVertexBufferData(const SceneContext& sceneContext, BaseMaterial* pMaterial);

	//VERTEX DATA
	UINT m_VertexCount{}, m_IndexCount{}, m_TexCoordCount{};
	UINT m_TempVertexCount{}, m_TempIndexCount{}, m_TempTexCoordCount{};

	ILSemantic m_Elements{ ILSemantic::NONE };
	std::vector<XMFLOAT3> m_Positions{};
	std::vector<XMFLOAT3> m_TempPositions{};

	std::vector<XMFLOAT2> m_TexCoords{};
	std::vector<XMFLOAT2> m_TempTexCoords{};


	//INDEX DATA
	std::vector<UINT> m_Indices{};
	std::vector<UINT> m_TempIndices{};

	VertexBufferData m_VertexBuffer{};
	ID3D11Buffer* m_pIndexBuffer{};


	static XMFLOAT4 m_DefaultColor_ex;
	static XMFLOAT4 m_DefaultFloat4_ex;
	static XMFLOAT3 m_DefaultFloat3_ex;
	static XMFLOAT2 m_DefaultFloat2_ex;
};

