#include "stdafx.h"
#include "ChunkMeshFilter.h"
ChunkMeshFilter::~ChunkMeshFilter()
{
	m_Positions.clear();
	m_TexCoords.clear();
	m_Indices.clear();


	m_VertexBuffer.Destroy();
	SafeRelease(m_pIndexBuffer);
}

ChunkMeshFilter::ChunkMeshFilter()
{
	SetElement(ILSemantic::POSITION);
	SetElement(ILSemantic::TEXCOORD);
}

void ChunkMeshFilter::AddFaceToMesh(std::vector<XMFLOAT3>& verticesToAdd, const std::vector<XMFLOAT2>* uv)
{
	//Add indices
	std::vector<UINT> indic ={ m_TempIndexCount, m_TempIndexCount + 1, m_TempIndexCount + 2, m_TempIndexCount + 2, m_TempIndexCount + 3, m_TempIndexCount,
			m_TempIndexCount + 4, m_TempIndexCount + 5, m_TempIndexCount + 6, m_TempIndexCount + 6, m_TempIndexCount + 7, m_TempIndexCount + 4 };

	m_TempIndices.insert(m_TempIndices.end(), indic.begin(), indic.end());
	m_TempPositions.insert(m_TempPositions.end(), verticesToAdd.begin(), verticesToAdd.end());

	for (size_t i = 0; i < verticesToAdd.size(); i++)
	{
		m_TempTexCoords.push_back((*uv)[i]);
		m_TempTexCoordCount += 2;
	}
	m_TempVertexCount += 4;
	m_TempIndexCount += 8;
}

void ChunkMeshFilter::UpdateBuffer(const SceneContext& gameContext, BaseMaterial* pMaterial)
{

	m_Indices = std::move(m_TempIndices);
	m_Positions = std::move(m_TempPositions);
	m_TexCoords = std::move(m_TempTexCoords);
	m_IndexCount = m_TempIndexCount;
	m_VertexCount = m_TempVertexCount;
	m_TexCoordCount = m_TempTexCoordCount;

	m_TempIndexCount = 0;
	m_TempVertexCount = 0;
	m_TempTexCoordCount = 0;


	BuildVertexBuffer(gameContext, pMaterial);
	BuildIndexBuffer(gameContext);


}



void ChunkMeshFilter::BuildVertexBuffer(const SceneContext& gameContext, BaseMaterial* pMaterial)
{
	auto& techniqueContext = pMaterial->GetTechniqueContext();

	//Check if VertexBufferInfo already exists with requested InputLayout
	//if (GetVertexBufferId(techniqueContext.inputLayoutID) >= 0)
	//	return;

	VertexBufferData data;
	m_VertexBuffer.VertexStride = techniqueContext.inputLayoutSize;
	m_VertexBuffer.VertexCount = m_VertexCount;
	m_VertexBuffer.BufferSize = m_VertexBuffer.VertexStride * m_VertexCount;
	m_VertexBuffer.IndexCount = m_IndexCount;

	void* pDataLocation = malloc(m_VertexBuffer.BufferSize);
	if (pDataLocation == nullptr)
	{
		Logger::LogWarning(L"Failed to allocate the required memory!");
		return;
	}

	m_VertexBuffer.pDataStart = pDataLocation;
	m_VertexBuffer.InputLayoutID = techniqueContext.inputLayoutID;

	for (UINT i = 0; i < m_VertexCount; ++i)
	{
		for (UINT j = 0; j < techniqueContext.pInputLayoutDescriptions.size(); ++j)
		{
			const ILDescription& ilDescription = techniqueContext.pInputLayoutDescriptions[j];

			switch (ilDescription.SemanticType)
			{
			case ILSemantic::POSITION:
				memcpy(pDataLocation, HasElement(ilDescription.SemanticType) ? &m_Positions[i] : &m_DefaultFloat3_ex, ilDescription.Offset);
				break;

			case ILSemantic::TEXCOORD:
				memcpy(pDataLocation, HasElement(ilDescription.SemanticType) ? &m_TexCoords[i] : &m_DefaultFloat2_ex, ilDescription.Offset);
				break;

			default:
				HANDLE_ERROR(L"Unsupported SemanticType!");
				break;
			}

			pDataLocation = static_cast<char*>(pDataLocation) + ilDescription.Offset;
		}
	}

	//fill a buffer description to copy the vertexdata into graphics memory
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = m_VertexBuffer.BufferSize;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData{};
	initData.pSysMem = m_VertexBuffer.pDataStart;

	//create a ID3D10Buffer in graphics memory containing the vertex info
	gameContext.d3dContext.pDevice->CreateBuffer(&bd, &initData, &m_VertexBuffer.pVertexBuffer);

}

void ChunkMeshFilter::BuildIndexBuffer(const SceneContext& gameContext)
{
	if (m_pIndexBuffer != nullptr)
		return;

	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(UINT) * UINT(m_Indices.size());
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA initData{};
	initData.pSysMem = m_Indices.data();

	HANDLE_ERROR(gameContext.d3dContext.pDevice->CreateBuffer(&bd, &initData, &m_pIndexBuffer))
}

const VertexBufferData& ChunkMeshFilter::GetVertexBufferData(const SceneContext& , BaseMaterial* )
{

	return m_VertexBuffer;
}

DirectX::XMFLOAT4 ChunkMeshFilter::m_DefaultColor_ex;

DirectX::XMFLOAT4 ChunkMeshFilter::m_DefaultFloat4_ex;

DirectX::XMFLOAT3 ChunkMeshFilter::m_DefaultFloat3_ex;

DirectX::XMFLOAT2 ChunkMeshFilter::m_DefaultFloat2_ex;
