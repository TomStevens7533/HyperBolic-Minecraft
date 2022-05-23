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
//Pos
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
//Normal
const std::array<float, 12> frontNormal{
	0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1,
};

const std::array<float, 12> backNormal{
	0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1,
};

const std::array<float, 12> leftNormal{
	-1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0,
};

const std::array<float, 12> rightNormal{
	1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0,
};

const std::array<float, 12> topNormal{
	0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0,
};
const std::array<float, 12> bottomNormal{
	0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0,

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
	//2. Make sure to set m_enableShadowMapDraw to true, otherwise BaseComponent::ShadowMapDraw is not called
	m_enableShadowMapDraw = true;
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

void ChunkMeshComponent::ShadowMapDraw(const SceneContext& sc)
{
	//We only draw this Mesh to the ShadowMap if it casts shadows
	if (!m_CastShadows)
		return;

	if (!m_enableShadowMapDraw)
		return;

	//This function is only called during the ShadowPass (and if m_enableShadowMapDraw is true)
//Here we want to Draw this Mesh to the ShadowMap, using the ShadowMapRenderer::DrawMesh function
	m_pChunkMeshFilter->DrawShadows(sc, m_pGameObject->GetTransform()->GetWorld());

	//1. Call ShadowMapRenderer::DrawMesh with the required function arguments BUT boneTransforms are only required for skinned meshes of course..
}

void ChunkMeshComponent::PostDraw(const SceneContext&) 
{
	
}

void ChunkMeshComponent::ResetMesh()
{
	m_pChunkMeshFilter->SetDirty();
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

bool ChunkMeshComponent::AddFace(XMFLOAT3 chunkPos, XMFLOAT3 localBlockPos, Faces dir, const std::vector<XMFLOAT2>* uv, bool isCube)
{
	if (m_IsInitialized) {


		//Array filled with vertexData
		const std::array<float, 12>* blockFace = nullptr;
		const std::array<float, 12>* blockNormal = nullptr;
		//Determine which side //TODO ADD LIGHT LEVEL FOR EACH BLOCK FACE
		float LightLevel = 1.f;
		int loopCount = 1;
		switch (dir)
		{
		case Faces::TOP:
			LightLevel = 1.0f;
			blockFace = &topFace;
			blockNormal = &topNormal;
			break;
		case Faces::BOT:
			LightLevel = 0.4f;
			blockFace = &bottomFace;
			blockNormal = &bottomNormal;
			break;
		case Faces::LEFT:
			LightLevel = 0.8f;
			blockFace = &leftFace;
			blockNormal = &leftNormal;
			break;
		case Faces::RIGHT:
			LightLevel = 0.8f;
			blockFace = &rightFace;
			blockNormal = &rightNormal;
			break;
		case Faces::FRONT:
			LightLevel = 0.6f;
			blockFace = &frontFace;
			blockNormal = &frontNormal;
			break;
		case Faces::BACK:
			LightLevel = 0.6f;
			blockFace = &backFace;
			blockNormal = &backNormal;
			break;
		case Faces::NONE:
			break;
		default:
			break;
		}

		if (!isCube) {
			blockFace = &xFace1;
			LightLevel = 1.f;
			loopCount = 2;
		}

		for (size_t i = 0; i < loopCount; i++)
		{
			std::vector<XMFLOAT3> m_vertices;
			m_vertices.reserve(4);

			std::vector<XMFLOAT3> m_Normals;

			for (int in = 0; in < 11; in += 3)
			{
				float x = (*blockNormal)[in];
				float y = (*blockNormal)[in + 1];
				float z = (*blockNormal)[in + 2];

				XMFLOAT3 normal = XMFLOAT3(x, y, z);
				m_Normals.push_back(normal);

			}


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



			m_pChunkMeshFilter->AddFaceToMesh(m_vertices, uv, LightLevel, m_Normals);

			if (loopCount > 1)
				blockFace = &xFace2;

		}



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
