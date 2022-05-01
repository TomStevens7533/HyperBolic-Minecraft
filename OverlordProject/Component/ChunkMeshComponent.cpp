#include "stdafx.h"
#include "stdafx.h"
#include "ChunkMeshComponent.h"
#include "Misc/MeshFilter.h"
#include "Content/ChunkMeshFilter.h"
#include <array>
#include "Misc/BaseMaterial.h"

const std::array<float, 12> xFace1{
	0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0,
};

const std::array<float, 12> xFace2{
	0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1,
};
const std::array<float, 12> frontFace{
	0, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1,
};

const std::array<float, 12> backFace{
	1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0,
};

const std::array<float, 12> leftFace{
	0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 0,
};

const std::array<float, 12> rightFace{
	1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1,
};

const std::array<float, 12> topFace{
	0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0,
};
const std::array<float, 12> bottomFace{ 
	0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1
};

ChunkMeshComponent::ChunkMeshComponent(BaseMaterial* pMaterial) : m_pMaterial{pMaterial}
{
	m_pChunkMeshFilter = new ChunkMeshFilter(m_pMaterial);

}

ChunkMeshComponent::~ChunkMeshComponent()
{
	delete m_pChunkMeshFilter;
}

void ChunkMeshComponent::Initialize(const SceneContext&)
{

}

void ChunkMeshComponent::Update(const SceneContext&)
{

};

void ChunkMeshComponent::Draw(const SceneContext& sceneContext)
{

	if (m_IsInitialized = true) {
		m_pMaterial->UpdateEffectVariables(sceneContext, this);

		m_pChunkMeshFilter->Draw(sceneContext);
	}
	
}

void ChunkMeshComponent::ResetMesh()
{
}

void ChunkMeshComponent::SetMaterial(BaseMaterial* pMaterial)
{
	if (pMaterial == nullptr)
	{
		m_pMaterial = nullptr;
		return;
	}

	if (!pMaterial->HasValidMaterialId())
	{
		Logger::LogWarning(L"BaseMaterial does not have a valid BaseMaterial Id. Make sure to add the material to the material manager first.");
		return;
	}

	m_pMaterial = pMaterial;
	m_IsInitialized = true;
	m_MaterialChanged = true;
}

void ChunkMeshComponent::SetMaterial(UINT materialId)
{
	const auto pMaterial = MaterialManager::Get()->GetMaterial(materialId);
	SetMaterial(pMaterial);
}

bool ChunkMeshComponent::AddFace(XMFLOAT3 chunkPos, XMFLOAT3 localBlockPos, Faces dir, const std::vector<XMFLOAT2>* uv)
{
	if (m_IsInitialized) {


		//Array filled with vertexData
		const std::array<float, 12>* blockFace = nullptr;
		//Determine which side //TODO ADD LIGHT LEVEL FOR EACH BLOCK FACE
		switch (dir)
		{
		case Faces::TOP:
			blockFace = &topFace;
			break;
		case Faces::BOT:
			blockFace = &bottomFace;
			break;
		case Faces::LEFT:
			blockFace = &leftFace;
			break;
		case Faces::RIGHT:
			blockFace = &rightFace;
			break;
		case Faces::FRONT:
			blockFace = &frontFace;
			break;
		case Faces::BACK:
			blockFace = &backFace;
			break;
		default:
			break;
		}
		std::vector<XMFLOAT3> m_vertices;
		m_vertices.reserve(4);

		//Create Vertex Information
		XMFLOAT3 Vertex1Pos = XMFLOAT3{ (*blockFace)[0] + (chunkPos.x + localBlockPos.x),
			(*blockFace)[1] + (chunkPos.y + localBlockPos.y),
			(*blockFace)[2] + (chunkPos.z + localBlockPos.z) };
		m_vertices.push_back(Vertex1Pos);
		//std::cout << Vertex1Pos.x << "  " << Vertex1Pos.y << " " << Vertex1Pos.z << std::endl;
		XMFLOAT3 Vertex2Pos = XMFLOAT3{ (*blockFace)[3] + (chunkPos.x + localBlockPos.x),
			(*blockFace)[4] + (chunkPos.y + localBlockPos.y),
			(*blockFace)[5] + (chunkPos.z + localBlockPos.z) };
		m_vertices.push_back(Vertex2Pos);

		XMFLOAT3 Vertex3Pos = XMFLOAT3{ (*blockFace)[6] + (chunkPos.x + localBlockPos.x),
			(*blockFace)[7] + (chunkPos.y + localBlockPos.y),
			(*blockFace)[8] + (chunkPos.z + localBlockPos.z) };
		m_vertices.push_back(Vertex3Pos);

		XMFLOAT3 Vertex4Pos = XMFLOAT3{ (*blockFace)[9] + chunkPos.x + localBlockPos.x,
			(*blockFace)[10] + (chunkPos.y + localBlockPos.y), (*blockFace)[11] + chunkPos.z + localBlockPos.z };
		m_vertices.push_back(Vertex4Pos);

		m_pChunkMeshFilter->AddFaceToMesh(m_vertices, uv);

		return false;
	}
	return false;
}

void ChunkMeshComponent::BufferMesh(const SceneContext& gameContext)
{
	//Rebuild mesh
	if (m_IsInitialized) {
		m_pChunkMeshFilter->UpdateBuffer(gameContext);

	}
}
