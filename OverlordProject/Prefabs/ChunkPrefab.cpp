#include "stdafx.h"
#include "ChunkPrefab.h"
#include "Materials/ChunkDiffuseMaterial.h"
#include "Component/ChunkMeshComponent.h"
#include "Content/PerlinNoise.hpp"

ChunkPrefab::ChunkPrefab(XMFLOAT3 chunkPos, ChunkManager* pchunkmanger, BaseMaterial* pMaterial, const unsigned int seed) : m_ChunkPosition{chunkPos}, m_pChunkManager{pchunkmanger}, m_pBaseMaterial{pMaterial}
{
	//Create Material
	//Generate Chunk

	//Build Chunk Mesh
	m_pChunkComponent = AddComponent(new ChunkMeshComponent(m_pBaseMaterial));
	m_pChunkComponent->SetMaterial(m_pBaseMaterial);


	const siv::PerlinNoise perlin{ seed };

	std::vector<ChunkPosistion> m_BuildTreePos;
	//Generate chunk
		//Chunk generation perlin nosie
	for (int xIndex = ChunkSizeX - 1; xIndex >= 0; xIndex--)
	{
		for (int zIndex = ChunkSizeZ - 1; zIndex >= 0 ; zIndex--)
		{
			
		

			float value = static_cast<float>((perlin.octave2D_01((m_ChunkPosition.x + xIndex) / 64.f, (m_ChunkPosition.z + zIndex) / 64.f, 8)));
			float value2 = static_cast<float>((perlin.octave2D_01((m_ChunkPosition.x + xIndex) / 128.f, (m_ChunkPosition.z + zIndex) / 128.f, 8)));
			float value3 = static_cast<float>((perlin.octave2D_01((m_ChunkPosition.x + xIndex) / 256.f, (m_ChunkPosition.z + zIndex) / 256.f, 8)));

			float totalValue = static_cast<float>((value * value2 * value3 * value3 - 0) / (1 - 0));
			int height = static_cast<int>(std::lerp(ChunkBaseTerrainHeight, ChunkMaxHeightGeneration, totalValue)) + 1;

			for (int yIndex = height ; yIndex >= 0 ; yIndex--)
			{
				
				cubeArray[yIndex][xIndex][zIndex] = GenerateBlockType(xIndex, yIndex, zIndex, height, m_BuildTreePos);
			}

		}
	}
	for (size_t i = 0; i < m_BuildTreePos.size(); i++)
	{
		BuildTree(m_BuildTreePos[i].x, m_BuildTreePos[i].y, m_BuildTreePos[i].z);
	}

}

uint8_t ChunkPrefab::GenerateBlockType(int x, int y, int z, int maxHeight, std::vector<ChunkPosistion>& buildTreePos)
{
	if (y == maxHeight) {
		//OnGroundGeneration

		if (((rand() % 1000) / 10.f) <= FlowerChance) {
			int randFlower = rand() % 4;
			switch (randFlower)
			{
			case 0:
				return 6;
				break;
			case 1:
				return 7;
				break;
			case 2:
				return 8;
				break;
			default:
				return 6;
				break;
			}
		}
		if (((rand() % 1000) / 10.f) <= TreeChance) {
			buildTreePos.push_back(ChunkPosistion(x, y, z));
			return 3;
		}

	}
	int depth = rand() % RandDirtDepth;
	if (y < (maxHeight)) {
		if (y == (maxHeight - 1))
			return 1;
		if (y > (maxHeight - 1 - depth)) //dirt
			return 9;
		else
			return 2;
	}
	return 0;
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



void ChunkPrefab::BuildTree(int x, int y, int z)
{
	//build log
	//int currentMaxLeafHeight = (leaveLength + MinLeavesHeight + (rand() % (MaxLeavesHeight - MinLeavesHeight)));

	int treeLenght = 0;
	for (size_t i = 0; i < (BaseTreeLength - 1); i++)
	{
		++treeLenght;
		cubeArray[y + treeLenght][x][z] = 3;
	}
	int extraLength = rand() % MaxTreeLength;
	for (size_t i = 0; i < extraLength; i++)
	{
		++treeLenght;
		cubeArray[y + treeLenght][x][z] = 3;

	}
	extraLength -= 1;
	int baseLeafPos = (((BaseTreeLength - 1) + (extraLength - 1)));

	for (int yIndex = ((y + (treeLenght - 1))); yIndex < ((y + treeLenght + 5)); yIndex++)
	{
		int extreLeafHeight = MinLeavesWidth +  (rand() % MaxLeavesWidth)  + ( ((y + treeLenght + 5) - yIndex) / 2);
		int extreLeafDepth = MinLeavesWidth + (rand() % MaxLeavesWidth) +(((y + treeLenght + 5) - yIndex) / 2);

		for (int i = ((x - extreLeafHeight)); i < (x + extreLeafHeight); i++)
		{
			

			for (int zLeaf = ((z - extreLeafDepth)); zLeaf < (z + extreLeafDepth); zLeaf++)
			{
				//int xLeafWidth;
				if (IsIndexInBounds(i, yIndex, zLeaf) && (cubeArray[yIndex][i][zLeaf] != 3))
					cubeArray[yIndex][i][zLeaf] = 4;
				else {
					//Build on other chunk
					XMFLOAT3 toWorldPose = XMFLOAT3(yIndex + baseLeafPos + m_ChunkPosition.x, i + m_ChunkPosition.y, z + m_ChunkPosition.z);
					m_pChunkManager->Addblock(toWorldPose, 5);
				}
			}
		}
	}
	

}

bool ChunkPrefab::IsIndexInBounds(int x, int y, int z) const
{
	if (x < 0 || x >(ChunkSizeX - 1))
		return false;
	if (y < 0 || y >(ChunkSizeY - 1))
		return false;
	if (z < 0 || z >(ChunkSizeZ - 1))
		return false;

	return true;
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

					bool isCube = m_pChunkManager->m_LevelJsonParser.IsCube(currentLookUpType);
					const std::map< Faces, std::vector<XMFLOAT2>>* uvCoords = m_pChunkManager->GetUVOfType(currentLookUpType);

					//Check if block under or above is solid
					if (yIndex != ChunkSizeY - 1) {
						if (yIndex != 0) { //Bottom Block Check
							int botIndex = yIndex - 1;
							if (!IsBlockSolid(xIndex,botIndex,zIndex)) {
								//is solid
								if (m_pChunkComponent->AddFace(m_ChunkPosition, XMFLOAT3{ static_cast<float>(xIndex), static_cast<float>(yIndex), static_cast<float>(zIndex) }, Faces::BOT, &(*uvCoords).at(Faces::BOT), isCube))
									continue;
							}
						}
						int TopIndex = yIndex + 1;
						if (!IsBlockSolid(xIndex,TopIndex,zIndex))
						{
							if (m_pChunkComponent->AddFace(m_ChunkPosition,XMFLOAT3{ static_cast<float>(xIndex), static_cast<float>(yIndex), static_cast<float>(zIndex) }, Faces::TOP, &(*uvCoords).at(Faces::TOP), isCube))
									continue;

						}
					}
					if (yIndex == (ChunkSizeY - 1)) //Always render faces at top of chunk
					{
						if (!IsBlockSolid(xIndex, yIndex, zIndex)) {
							if (m_pChunkComponent->AddFace(m_ChunkPosition, XMFLOAT3{ static_cast<float>(xIndex), static_cast<float>(yIndex), static_cast<float>(zIndex) }, Faces::TOP, &(*uvCoords).at(Faces::TOP), isCube))
								continue;
						}
				
					}
					
					//WE DONT RENDER BOTTOM OF CHUNK

					//check left/right X
					if (xIndex != (ChunkSizeX)) { //left check
						int leftIndex = xIndex > 0 ? xIndex - 1 : 0;
						if (!IsBlockSolid(leftIndex, yIndex, zIndex))
							if (m_pChunkComponent->AddFace(m_ChunkPosition, XMFLOAT3{ static_cast<float>(xIndex), static_cast<float>(yIndex), static_cast<float>(zIndex) }, Faces::LEFT, &(*uvCoords).at(Faces::LEFT), isCube))
								continue;

						// right check
						int rightIndex = xIndex < (ChunkSizeX - 1) ? xIndex + 1 : (ChunkSizeX - 1);
						if (!IsBlockSolid(rightIndex, yIndex, zIndex))
							if (m_pChunkComponent->AddFace(m_ChunkPosition, XMFLOAT3{ static_cast<float>(xIndex), static_cast<float>(yIndex), static_cast<float>(zIndex) }, Faces::RIGHT, &(*uvCoords).at(Faces::RIGHT), isCube))
								continue;
					}
					//Check neighbouring chunks for now render it
					if (xIndex == 0) {
						if (!m_pChunkManager->IsBlockInChunkSolid(std::make_pair(static_cast<int>(m_ChunkPosition.x) - ChunkSizeX, static_cast<int>(m_ChunkPosition.z)), (ChunkSizeX - 1) - xIndex, yIndex, zIndex)) {
							if (m_pChunkComponent->AddFace(m_ChunkPosition, XMFLOAT3{ static_cast<float>(xIndex), static_cast<float>(yIndex), static_cast<float>(zIndex) }, Faces::LEFT, &(*uvCoords).at(Faces::LEFT), isCube))
								continue;
						}
						
					}
					if (xIndex == (ChunkSizeX - 1)) {
						if (!m_pChunkManager->IsBlockInChunkSolid(std::make_pair(static_cast<int>(m_ChunkPosition.x) + ChunkSizeX, static_cast<int>(m_ChunkPosition.z)), 0, yIndex, zIndex)) {
							if (m_pChunkComponent->AddFace(m_ChunkPosition, XMFLOAT3{ static_cast<float>(xIndex), static_cast<float>(yIndex), static_cast<float>(zIndex) }, Faces::RIGHT, &(*uvCoords).at(Faces::RIGHT), isCube))
								continue;
						}
			
					}
					//ZCheck
					if (zIndex != (ChunkSizeZ)) { //z is -1 is back +1 is front in directx rasterization
						//Render in chunk
						int backIndex = zIndex > 0 ? zIndex - 1 : 0;
						if (!IsBlockSolid(xIndex, yIndex, backIndex))
							if (m_pChunkComponent->AddFace(m_ChunkPosition, XMFLOAT3{ static_cast<float>(xIndex), static_cast<float>(yIndex), static_cast<float>(zIndex) }, Faces::BACK, &(*uvCoords).at(Faces::BACK), isCube))
								continue;

						//front check
						int frontIndex = zIndex < (ChunkSizeZ - 1) ? zIndex + 1 : (ChunkSizeZ - 1);
						if (!IsBlockSolid(xIndex, yIndex, frontIndex))
							if (m_pChunkComponent->AddFace(m_ChunkPosition, XMFLOAT3{ static_cast<float>(xIndex), static_cast<float>(yIndex), static_cast<float>(zIndex) }, Faces::FRONT, &(*uvCoords).at(Faces::FRONT), isCube))
								continue;
						
					}
					//Check neighbouring chunks for now render it
					if(zIndex == 0) {
						if (!m_pChunkManager->IsBlockInChunkSolid(std::make_pair(static_cast<int>(m_ChunkPosition.x), static_cast<int>(m_ChunkPosition.z) - ChunkSizeZ), xIndex, yIndex, (ChunkSizeZ - 1))) {
							if (m_pChunkComponent->AddFace(m_ChunkPosition, XMFLOAT3{ static_cast<float>(xIndex), static_cast<float>(yIndex), static_cast<float>(zIndex) }, Faces::BACK, &(*uvCoords).at(Faces::BACK), isCube))
								continue;
						}
					}
					if (zIndex == (ChunkSizeZ - 1)) {
						if (!m_pChunkManager->IsBlockInChunkSolid(std::make_pair(static_cast<int>(m_ChunkPosition.x), static_cast<int>(m_ChunkPosition.z) + ChunkSizeZ), xIndex, yIndex, 0)) {
							if (m_pChunkComponent->AddFace(m_ChunkPosition, XMFLOAT3{ static_cast<float>(xIndex), static_cast<float>(yIndex), static_cast<float>(zIndex) }, Faces::FRONT, &(*uvCoords).at(Faces::FRONT), isCube))
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

uint8_t ChunkPrefab::DeleteBlock(int x, int y, int z)
{
	if (cubeArray[y][x][z] != 0) {
		uint8_t id = cubeArray[y][x][z];
		cubeArray[y][x][z] = 0;
		m_NeedUpdate = true;
		return id;
	}
	return 0;
}

bool ChunkPrefab::AddBlock(uint8_t id, int x, int y, int z)
{
	if (cubeArray[y][x][z] == 0) {
		cubeArray[y][x][z] = id;
		m_NeedUpdate = true;
		return true;

	}
	return false;
}

bool ChunkPrefab::IsBlockSolid(int x, int y, int z) const
{
	if (m_pChunkManager->m_LevelJsonParser.IsSolid(cubeArray[y][x][z]))
		return true;
	else
		return false;
}
