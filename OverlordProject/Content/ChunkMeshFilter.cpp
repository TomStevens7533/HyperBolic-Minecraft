#include "stdafx.h"
#include "ChunkMeshFilter.h"
#include "Utils/EffectHelper.h"
#include "Misc/BaseMaterial.h"
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

void ChunkMeshFilter::AddFaceToMesh(std::vector<XMFLOAT3>& verticesToAdd, const std::vector<XMFLOAT2>* uv, float lightLevel)
{
	//Add indices
	std::vector<UINT> indic = { m_TempIndexCount, m_TempIndexCount + 1, m_TempIndexCount + 2, m_TempIndexCount + 2, m_TempIndexCount + 3, m_TempIndexCount,
			m_TempIndexCount + 4, m_TempIndexCount + 5, m_TempIndexCount + 6, m_TempIndexCount + 6, m_TempIndexCount + 7, m_TempIndexCount + 4 };

	m_TempIndices.insert(m_TempIndices.end(), indic.begin(), indic.end());
	m_TempPositions.insert(m_TempPositions.end(), verticesToAdd.begin(), verticesToAdd.end());
	
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

void ChunkMeshFilter::UpdateBuffer(const SceneContext& gameContext)
{
	m_IsUpdated = false;
	m_Indices = std::move(m_TempIndices);
	m_Positions = std::move(m_TempPositions);
	m_TexCoords = std::move(m_TempTexCoords);
	m_LightLevel = std::move(m_TempLightLevel);
	m_IndexCount = m_TempIndexCount;
	m_VertexCount = m_TempVertexCount;
	m_TexCoordCount = m_TempTexCoordCount;

	m_TempIndexCount = 0;
	m_TempVertexCount = 0;
	m_TempTexCoordCount = 0;
	m_TempLightLevelCount = 0;
	UpdateBufferBIG(gameContext, m_pMaterial);

}



void ChunkMeshFilter::UpdateBufferBIG(const SceneContext& context, BaseMaterial* pMaterial)
{

	//Create new vertex
	if ((!m_VertexBuffer.pVertexBuffer || m_VertexCount > m_BufferSize)) {
		//Release Buffer if it exists
		SafeRelease(m_pIndexBuffer);
		SafeRelease(m_VertexBuffer.pVertexBuffer);
		m_pInputLayout = pMaterial->GetTechniqueContext().pInputLayout;

		//Set new buffersize if needed
		
			m_BufferSize = m_VertexCount;
		

		m_VertexBuffer.VertexStride = pMaterial->GetTechniqueContext().inputLayoutSize;
		m_VertexBuffer.VertexCount = m_VertexCount;
		m_VertexBuffer.BufferSize = m_VertexBuffer.VertexStride * m_VertexCount;
		m_VertexBuffer.IndexCount = m_IndexCount;
		m_VertexBuffer.InputLayoutID = pMaterial->GetTechniqueContext().inputLayoutID;



		//Set new buffersize if needed
		//Create Dynamic Buffer
		D3D11_BUFFER_DESC bufferDesc{};
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.ByteWidth = m_VertexBuffer.BufferSize;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = 0;

		HANDLE_ERROR(context.d3dContext.pDevice->CreateBuffer(&bufferDesc, nullptr, &m_VertexBuffer.pVertexBuffer));

	}
	if ((!m_pIndexBuffer || m_IndexCount > m_IndexBufferSize)) {

		
		m_IndexBufferSize = m_IndexCount;
		

		D3D11_BUFFER_DESC bd = {};
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(UINT) * UINT(m_Indices.size());
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bd.MiscFlags = 0;
		D3D11_SUBRESOURCE_DATA initData{};
		HANDLE_ERROR(context.d3dContext.pDevice->CreateBuffer(&bd, nullptr, &m_pIndexBuffer));
	}
	//Create new Index

	//Refresh Buffer
	m_BufferSize = m_VertexCount;


	m_VertexBuffer.VertexStride = pMaterial->GetTechniqueContext().inputLayoutSize;
	m_VertexBuffer.VertexCount = m_VertexCount;
	m_VertexBuffer.BufferSize = m_VertexBuffer.VertexStride * m_VertexCount;
	m_VertexBuffer.IndexCount = m_IndexCount;
	m_VertexBuffer.InputLayoutID = pMaterial->GetTechniqueContext().inputLayoutID;



	D3D11_MAPPED_SUBRESOURCE mappedResource{};
	_Analysis_assume_(m_pVertexBuffer != nullptr);
	D3D11_BUFFER_DESC bufferDesc{};
	m_VertexBuffer.pVertexBuffer->GetDesc(&bufferDesc);
	bufferDesc.ByteWidth = m_VertexBuffer.BufferSize;

	//consider data invalid and replaces it D3D11_MAP_WRITE_DISCARD
	context.d3dContext.pDeviceContext->Map(m_VertexBuffer.pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	CreateChunkVertices(mappedResource.pData);
	//create buffer

	context.d3dContext.pDeviceContext->Unmap(m_VertexBuffer.pVertexBuffer, 0);


	D3D11_MAPPED_SUBRESOURCE mappedResourceidx{};
	_Analysis_assume_(m_pIndexBuffer != nullptr);
	
	D3D11_BUFFER_DESC bd;
	m_pIndexBuffer->GetDesc(&bd);
	bd.ByteWidth = sizeof(UINT) * m_IndexCount;
	m_IndexBufferSize = m_IndexCount;

	//consider data invalid and replaces it D3D11_MAP_WRITE_DISCARD
	context.d3dContext.pDeviceContext->Map(m_pIndexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResourceidx);
	//create buffer
	CreateChunkIndices(mappedResourceidx.pData);
	context.d3dContext.pDeviceContext->Unmap(m_pIndexBuffer, 0);
	//other buffer

	m_IsUpdated = true;

}

void ChunkMeshFilter::CreateChunkVertices(void* pBuffer)
{
	for (UINT i = 0; i < m_BufferSize; ++i)
	{
		for (UINT j = 0; j < m_pMaterial->GetTechniqueContext().pInputLayoutDescriptions.size(); ++j)
		{
			const ILDescription& ilDescription = m_pMaterial->GetTechniqueContext().pInputLayoutDescriptions[j];
			XMFLOAT3 defv(0.f, 0.f, 0.f);
			XMFLOAT2 defu(0.f, 0.f);
			float defl = 0.f;

			switch (ilDescription.SemanticType)
			{
			case ILSemantic::POSITION:
				memcpy(pBuffer, i <= m_VertexBuffer.VertexCount ? &m_Positions[i] : &defv, ilDescription.Offset);
				break;

			case ILSemantic::TEXCOORD:
				memcpy(pBuffer, i <= m_VertexBuffer.VertexCount ? &m_TexCoords[i] : &defu, ilDescription.Offset);
				break;
			case ILSemantic::NORMAL:
				memcpy(pBuffer, i <= m_VertexBuffer.VertexCount ? &m_LightLevel[i] : &defl, ilDescription.Offset);
				break;
			default:
				HANDLE_ERROR(L"Unsupported SemanticType!");
				break;
			}
			pBuffer = static_cast<char*>(pBuffer) + ilDescription.Offset;
		}

	}
	
}


void ChunkMeshFilter::CreateChunkIndices(void* pBuffer)
{
	for (UINT i = 0; i < m_IndexBufferSize; ++i)
	{
		memcpy(pBuffer, i <= m_IndexCount ? &m_Indices[i] : 0, sizeof(UINT));
		pBuffer = static_cast<char*>(pBuffer) + sizeof(UINT);
	}
}

void ChunkMeshFilter::Draw(const SceneContext& sceneContext)
{
	//Set Render Pipeline
	if (m_IsUpdated == true) {
		//Set Render Pipeline
		const auto pDeviceContext = sceneContext.d3dContext.pDeviceContext;
		pDeviceContext->IASetInputLayout(m_pInputLayout);

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