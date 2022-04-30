#include "stdafx.h"
#include "ChunkPrefab.h"
#include "Materials/ChunkDiffuseMaterial.h"
#include "Component/ChunkMeshComponent.h"
#include "Content/PerlinNoise.hpp"

ChunkPrefab::ChunkPrefab(XMFLOAT3 chunkPos, const ChunkManager* pchunkmanger, BaseMaterial* pMaterial) : m_ChunkPosition{chunkPos}, m_pChunkManager{pchunkmanger}, m_pBaseMaterial{pMaterial}
{
	//Create Material
	//Generate Chunk

	//Build Chunk Mesh
	m_pChunkComponent = new ChunkMeshComponent();
	m_pChunkComponent->SetMaterial(m_pBaseMaterial);
	AddComponent(m_pChunkComponent);


	const siv::PerlinNoise::seed_type seed = 123456u;
	const siv::PerlinNoise perlin{ seed };
	//Generate chunk
		//Chunk generation perlin nosie
	for (int xIndex = 0; xIndex < ChunkSizeX; xIndex++)
	{
		for (int zIndex = 0; zIndex < ChunkSizeZ; zIndex++)
		{

			float value = static_cast<float>((perlin.octave2D_01((m_ChunkPosition.x + xIndex) / 64.f, (m_ChunkPosition.z + zIndex) / 64.f, 8)));
			float value2 = static_cast<float>((perlin.octave2D_01((m_ChunkPosition.x + xIndex) / 128.f, (m_ChunkPosition.z + zIndex) / 128.f, 8)));
			float value3 = static_cast<float>((perlin.octave2D_01((m_ChunkPosition.x + xIndex) / 256.f, (m_ChunkPosition.z + zIndex) / 256.f, 8)));

			float totalValue = static_cast<float>((value * value2 * value3 * value3 - 0) / (1 - 0));
			int height = static_cast<int>(std::lerp(ChunkBaseTerrainHeight, ChunkMaxHeightGeneration, totalValue));
			std::cout << height << std::endl;
			for (int yIndex = 0; yIndex < height; yIndex++)
			{

				if (xIndex == 0 || xIndex == (ChunkSizeX - 1))
					cubeArray[yIndex][xIndex][zIndex] = 1;
				else if (zIndex == 0 || zIndex == (ChunkSizeZ - 1))
					cubeArray[yIndex][xIndex][zIndex] = 1;
				else
					cubeArray[yIndex][xIndex][zIndex] = 2;
			}


		}
	}

}

void ChunkPrefab::DrawImGui()
{
	
}


void ChunkPrefab::Initialize(const SceneContext&)
{
	
}

void ChunkPrefab::Update(const SceneContext&)
{

}



void ChunkPrefab::UpdateMesh(const SceneContext& scenContext)
{
	m_pChunkComponent->ResetMesh();
	for (int yIndex = 0; yIndex < ChunkSizeY; yIndex++)
	{
		for (int xIndex = 0; xIndex < ChunkSizeX; xIndex++)
		{
			for (int zIndex = 0; zIndex < ChunkSizeZ; zIndex++)
			{
				uint8_t currentLookUpType = cubeArray[yIndex][xIndex][zIndex];
				if (currentLookUpType != 0) { //if AIR/Seetrouh skip no faces need to be added

					const std::map< Faces, std::vector<XMFLOAT2>>* uvCoords = m_pChunkManager->GetUVOfType(currentLookUpType);

					//Check if block under or above is solid
					if (yIndex != ChunkSizeY - 1) {
						if (yIndex != 0) { //Bottom Block Check
							int botIndex = yIndex - 1;
							if (cubeArray[botIndex][xIndex][zIndex] == 0) {
								//is solid
								if (m_pChunkComponent->AddFace(m_ChunkPosition, XMFLOAT3{ static_cast<float>(xIndex), static_cast<float>(yIndex), static_cast<float>(zIndex) }, Faces::BOT, &(*uvCoords).at(Faces::BOT)))
									continue;
							}
						}
						int TopIndex = yIndex + 1;
						if (cubeArray[TopIndex][xIndex][zIndex] == 0)
						{
							if (m_pChunkComponent->AddFace(m_ChunkPosition,XMFLOAT3{ static_cast<float>(xIndex), static_cast<float>(yIndex), static_cast<float>(zIndex) }, Faces::TOP, &(*uvCoords).at(Faces::TOP)))
									continue;

						}
					}
					if (yIndex == (ChunkSizeY - 1)) //Always render faces at top of chunk
					{
						if (cubeArray[yIndex][xIndex][zIndex] != 0) {
							if (m_pChunkComponent->AddFace(m_ChunkPosition, XMFLOAT3{ static_cast<float>(xIndex), static_cast<float>(yIndex), static_cast<float>(zIndex) }, Faces::TOP, &(*uvCoords).at(Faces::TOP)))
								continue;
						}
				
					}
					
					//WE DONT RENDER BOTTOM OF CHUNK

					//check left/right X
					if (xIndex != (ChunkSizeX)) { //left check
						int leftIndex = xIndex > 0 ? xIndex - 1 : 0;
						if (cubeArray[yIndex][leftIndex][zIndex] == 0)
							if (m_pChunkComponent->AddFace(m_ChunkPosition, XMFLOAT3{ static_cast<float>(xIndex), static_cast<float>(yIndex), static_cast<float>(zIndex) }, Faces::LEFT, &(*uvCoords).at(Faces::LEFT)))
								continue;

						// right check
						int rightIndex = xIndex < (ChunkSizeX - 1) ? xIndex + 1 : (ChunkSizeX - 1);
						if (cubeArray[yIndex][rightIndex][zIndex] == 0)
							if (m_pChunkComponent->AddFace(m_ChunkPosition, XMFLOAT3{ static_cast<float>(xIndex), static_cast<float>(yIndex), static_cast<float>(zIndex) }, Faces::RIGHT, &(*uvCoords).at(Faces::RIGHT)))
								continue;
					}
					//Check neighbouring chunks for now render it
					if (xIndex == 0) {
						if (!m_pChunkManager->IsBlockInChunkSolid(std::make_pair(static_cast<int>(m_ChunkPosition.x) - ChunkSizeX, static_cast<int>(m_ChunkPosition.z)), (ChunkSizeX - 1) - xIndex, yIndex, zIndex)) {
							if (m_pChunkComponent->AddFace(m_ChunkPosition, XMFLOAT3{ static_cast<float>(xIndex), static_cast<float>(yIndex), static_cast<float>(zIndex) }, Faces::LEFT, &(*uvCoords).at(Faces::LEFT)))
								continue;
						}
						
					}
					if (xIndex == (ChunkSizeX - 1)) {
						if (!m_pChunkManager->IsBlockInChunkSolid(std::make_pair(static_cast<int>(m_ChunkPosition.x) + ChunkSizeX, static_cast<int>(m_ChunkPosition.z)), 0, yIndex, zIndex)) {
							if (m_pChunkComponent->AddFace(m_ChunkPosition, XMFLOAT3{ static_cast<float>(xIndex), static_cast<float>(yIndex), static_cast<float>(zIndex) }, Faces::RIGHT, &(*uvCoords).at(Faces::RIGHT)))
								continue;
						}
			
					}
					//ZCheck
					if (zIndex != (ChunkSizeZ)) { //z is -1 is back +1 is front in directx rasterization
						//Render in chunk
						int backIndex = zIndex > 0 ? zIndex - 1 : 0;
						if (cubeArray[yIndex][xIndex][backIndex] == 0)
							if (m_pChunkComponent->AddFace(m_ChunkPosition, XMFLOAT3{ static_cast<float>(xIndex), static_cast<float>(yIndex), static_cast<float>(zIndex) }, Faces::BACK, &(*uvCoords).at(Faces::BACK)))
								continue;

						//front check
						int frontIndex = zIndex < (ChunkSizeZ - 1) ? zIndex + 1 : (ChunkSizeZ - 1);
						if (cubeArray[yIndex][xIndex][frontIndex] == 0)
							if (m_pChunkComponent->AddFace(m_ChunkPosition, XMFLOAT3{ static_cast<float>(xIndex), static_cast<float>(yIndex), static_cast<float>(zIndex) }, Faces::FRONT, &(*uvCoords).at(Faces::FRONT)))
								continue;
						
					}
					//Check neighbouring chunks for now render it
					if(zIndex == 0) {
						if (!m_pChunkManager->IsBlockInChunkSolid(std::make_pair(static_cast<int>(m_ChunkPosition.x), static_cast<int>(m_ChunkPosition.z) - ChunkSizeZ), xIndex, yIndex, (ChunkSizeZ - 1))) {
							if (m_pChunkComponent->AddFace(m_ChunkPosition, XMFLOAT3{ static_cast<float>(xIndex), static_cast<float>(yIndex), static_cast<float>(zIndex) }, Faces::BACK, &(*uvCoords).at(Faces::BACK)))
								continue;
						}
					}
					if (zIndex == (ChunkSizeZ - 1)) {
						if (!m_pChunkManager->IsBlockInChunkSolid(std::make_pair(static_cast<int>(m_ChunkPosition.x), static_cast<int>(m_ChunkPosition.z) + ChunkSizeZ), xIndex, yIndex, 0)) {
							if (m_pChunkComponent->AddFace(m_ChunkPosition, XMFLOAT3{ static_cast<float>(xIndex), static_cast<float>(yIndex), static_cast<float>(zIndex) }, Faces::FRONT, &(*uvCoords).at(Faces::FRONT)))
								continue;
						}
						
					}
				}



			}


		}
	}
	m_pChunkComponent->BufferMesh(scenContext);
	m_NeedUpdate = false;
}

bool ChunkPrefab::DeleteBlock(int x, int y, int z)
{
	if (cubeArray[y][x][z] != 0) {
		cubeArray[y][x][z] = 0;
		m_NeedUpdate = true;
		return true;
	}
	return false;
}

bool ChunkPrefab::IsBlockSolid(int x, int y, int z) const
{
	if (cubeArray[y][x][z] != 0)
		return true;
	else
		return false;
}
