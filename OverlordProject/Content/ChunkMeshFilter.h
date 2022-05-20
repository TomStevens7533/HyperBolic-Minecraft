#pragma once
#include "Base/Structs.h"
#include <mutex>
class ChunkMeshFilter final
{
public:
	ChunkMeshFilter(BaseMaterial* pMaterial);
	~ChunkMeshFilter();
	ChunkMeshFilter(const ChunkMeshFilter& other) = delete;
	ChunkMeshFilter(ChunkMeshFilter&& other) noexcept = delete;
	ChunkMeshFilter& operator=(const ChunkMeshFilter& other) = delete;
	ChunkMeshFilter& operator=(ChunkMeshFilter&& other) noexcept = delete;

	void AddFaceToMesh(std::vector<XMFLOAT3>& verticesToAdd, const std::vector<XMFLOAT2>* uv, float lightLevel, std::vector<XMFLOAT3>& blockNormal);
	const VertexBufferData& GetVertexBufferData() const;
	ID3D11Buffer* GetIndexBuffer() const { return m_pIndexBuffer; }
	UINT GetIndexCount() const { return m_IndexCount; }
	void DrawShadows(const SceneContext& sc, const XMFLOAT4X4& world);

	void UpdateBuffer(const SceneContext& gameContext);
private:
	friend class ChunkMeshComponent;

	void BuildVertexBuffer(const SceneContext& gameContext, BaseMaterial* pMaterial);
	void BuildIndexBuffer(const SceneContext& gameContext);
	void Draw(const SceneContext& sceneContext);
	void SetElement(ILSemantic element) { m_Elements |= element; }
	bool HasElement(ILSemantic element) const { return isSet(m_Elements, element); }
	BaseMaterial* m_pMaterial;

	//VERTEX DATA
	std::atomic<UINT> m_VertexCount{}, m_IndexCount{}, m_TexCoordCount{}, m_LightLevelCount{};
	UINT m_TempVertexCount{}, m_TempIndexCount{}, m_TempTexCoordCount{}, m_TempLightLevelCount{};

	ILSemantic m_Elements{ ILSemantic::NONE };
	std::vector<XMFLOAT3> m_Positions{};
	std::vector<XMFLOAT3> m_TempPositions{};

	std::vector<XMFLOAT2> m_TexCoords{};
	std::vector<XMFLOAT2> m_TempTexCoords{};


	std::vector<XMFLOAT3> m_Normals{};
	std::vector<XMFLOAT3> m_TempNormals{};

	std::vector<float> m_LightLevel{};
	std::vector<float> m_TempLightLevel{};

	//INDEX DATA
	std::vector<UINT> m_Indices{};
	std::vector<UINT> m_TempIndices{};

	 VertexBufferData m_VertexBuffer{};
	ID3D11Buffer* m_pIndexBuffer{};

	std::atomic<bool> m_IsIntialized = false;

};

