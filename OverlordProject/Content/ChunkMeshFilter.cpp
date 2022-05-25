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

ChunkMeshFilter::ChunkMeshFilter(BaseMaterial* pMaterial) : m_pMaterial{ pMaterial }
{
	SetElement(ILSemantic::POSITION);
	SetElement(ILSemantic::TEXCOORD);
}

void ChunkMeshFilter::AddFaceToMesh(std::vector<XMFLOAT3>& verticesToAdd, const std::vector<XMFLOAT2>* uv, float lightLevel, std::vector<XMFLOAT3>& blockNormal)
{
	//Add indices
	if (m_IsIntialized == false) {
		std::vector<UINT> indic = { m_TempIndexCount, m_TempIndexCount + 1, m_TempIndexCount + 2, m_TempIndexCount + 2, m_TempIndexCount + 3, m_TempIndexCount,
			m_TempIndexCount + 4, m_TempIndexCount + 5, m_TempIndexCount + 6, m_TempIndexCount + 6, m_TempIndexCount + 7, m_TempIndexCount + 4 };

		m_TempIndices.insert(m_TempIndices.end(), indic.begin(), indic.end());
		m_TempPositions.insert(m_TempPositions.end(), verticesToAdd.begin(), verticesToAdd.end());
		m_TempNormals.insert(m_TempNormals.end(), blockNormal.begin(), blockNormal.end());


		for (size_t i = 0; i < verticesToAdd.size(); i++)
		{
			m_TempLightLevel.push_back(lightLevel);
			++m_TempLightLevelCount;

		}

		for (size_t i = 0; i < verticesToAdd.size(); i++)
		{
			m_TempTexCoords.push_back((*uv)[i]);
			m_TempTexCoordCount += 2;
		}
		m_TempVertexCount += 4;
		m_TempIndexCount += 8;
	}
	
}

void ChunkMeshFilter::DrawShadows(const SceneContext& sc, const XMFLOAT4X4& world)
{
	//This function is only called during the ShadowPass (and if m_enableShadowMapDraw is true)
//Here we want to Draw this Mesh to the ShadowMap, using the ShadowMapRenderer::DrawMesh function
	if (m_IsIntialized == true) {
		ShadowMapRenderer::Get()->DrawMesh(sc, this, world);
	}
}

void ChunkMeshFilter::UpdateBuffer(const SceneContext& gameContext)
{
	m_Indices = std::move(m_TempIndices);
	m_Positions = std::move(m_TempPositions);
	m_TexCoords = std::move(m_TempTexCoords);
	m_Normals = std::move(m_TempNormals);
	m_LightLevel = std::move(m_TempLightLevel);
	m_IndexCount = m_TempIndexCount;
	m_VertexCount = m_TempVertexCount;
	m_TexCoordCount =  m_TempTexCoordCount;

	m_TempIndexCount = 0;
	m_TempVertexCount = 0;
	m_TempTexCoordCount = 0;

	m_VertexBuffer.Destroy();
	SafeRelease(m_pIndexBuffer);

	BuildVertexBuffer(gameContext, m_pMaterial);
	BuildIndexBuffer(gameContext);

	m_Indices.clear();
	m_Positions.clear();
	m_Normals.clear();
	m_LightLevel.clear();

	m_IsIntialized = true;


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
			XMFLOAT3 defv(0.f, 0.f, 0.f);
			XMFLOAT3 defn(0.f, 1.f, 0.f);

			XMFLOAT2 defu(0.f, 0.f);

			float defl = 0.f;

			switch (ilDescription.SemanticType)
			{
			case ILSemantic::POSITION:
				memcpy(pDataLocation, i <= m_VertexBuffer.VertexCount ? &m_Positions[i] : &defv, ilDescription.Offset);
				break;

			case ILSemantic::TEXCOORD:
				memcpy(pDataLocation, i <= m_VertexBuffer.VertexCount ? &m_TexCoords[i] : &defu, ilDescription.Offset);
				break;
			case ILSemantic::NORMAL:
				memcpy(pDataLocation, i <= m_VertexBuffer.VertexCount ? &m_Normals[i] : &defn, ilDescription.Offset);
				break;
			case ILSemantic::TANGENT:
				memcpy(pDataLocation, i <= m_VertexBuffer.VertexCount ? &m_LightLevel[i] : &defl, ilDescription.Offset);
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

void ChunkMeshFilter::Draw(const SceneContext& sceneContext)
{
	if (m_IsIntialized == true) {
		const auto pDeviceContext = sceneContext.d3dContext.pDeviceContext;
		pDeviceContext->IASetInputLayout(m_pMaterial->GetTechniqueContext().pInputLayout);

		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		constexpr UINT offset = 0;
		pDeviceContext->IASetVertexBuffers(0, 1, &m_VertexBuffer.pVertexBuffer, &m_VertexBuffer.VertexStride, &offset);


		//Set Index Buffer
		pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);


		auto tech = m_pMaterial->GetTechniqueContext().pTechnique;
		D3DX11_TECHNIQUE_DESC techDesc{};
		tech->GetDesc(&techDesc);
		for (UINT i = 0; i < techDesc.Passes; ++i)
		{
			tech->GetPassByIndex(i)->Apply(0, pDeviceContext);
			pDeviceContext->DrawIndexed(m_IndexCount, 0, 0);
		}
	}
	
}


const VertexBufferData& ChunkMeshFilter::GetVertexBufferData() const
{

	return m_VertexBuffer;
}

