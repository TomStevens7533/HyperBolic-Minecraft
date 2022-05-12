#include "stdafx.h"
#include "ChunkPrefab.h"
#include "Materials/ChunkDiffuseMaterial.h"
#include "Component/ChunkMeshComponent.h"
#include "Content/PerlinNoise.hpp"

ChunkPrefab::ChunkPrefab(XMFLOAT3 chunkPos, ChunkManager* pchunkmanger, BaseMaterial* pMaterial) : m_ChunkPosition{chunkPos}, m_pChunkManager{pchunkmanger}, m_pBaseMaterial{pMaterial}
{
	//Create Material
	//Generate Chunk

	//Build Chunk Mesh
	m_pChunkComponent = new ChunkMeshComponent(m_pBaseMaterial);
	m_pChunkComponent->SetMaterial(m_pBaseMaterial);
	AddComponent(m_pChunkComponent);


	const siv::PerlinNoise::seed_type seed = 123456u;
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

			for (int yIndex = height; yIndex >= 0 ; yIndex--)
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
	if (y == (maxHeight)) {


		if (((rand() % 1000) / 10.f) <= TreeChance) {
			buildTreePos.push_back(ChunkPosistion(x, y, z));
			return 4;
		}
		return 1;
	}
	else {
		int depth = rand() % RandDirtDepth;
		if (y >= (maxHeight - depth))
			return 1;

	}
		return 2;
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


	for (size_t i = 0; i < (BaseTreeLength - 1); i++)
	{
		cubeArray[y + i][x][z] = 3;
	}
	int extraLength = rand() % MaxTreeLength;
	for (size_t i = 0; i < extraLength; i++)
	{
		cubeArray[y + i + (BaseTreeLength - 1)][x][z] = 3;
	}
	extraLength -= 1;
	int baseLeafPos = ((BaseTreeLength + extraLength) / 2);

	int extreLeafHeight = ((y + extraLength)) + (rand() % MaxLeavesHeight);
	for (int yIndex = ((y + baseLeafPos)); yIndex < ((y + (BaseTreeLength - 1) + extraLength)); yIndex++)
	{
		for (int i = ((x - MinLeavesWidth)); i < (extreLeafHeight); i++)
		{
			//int xLeafWidth;
			if (IsIndexInBounds(i, yIndex + baseLeafPos, z) && (cubeArray[yIndex + baseLeafPos][i][z] != 3))
				cubeArray[yIndex + baseLeafPos][i][z] = 4;
			else {
				//Build on other chunk
				XMFLOAT3 toWorldPose = XMFLOAT3(yIndex + baseLeafPos + m_ChunkPosition.x, i + m_ChunkPosition.y, z + m_ChunkPosition.z);
				m_pChunkManager->Addblock(toWorldPose);
			}

			for (int zLeaf = ((z - MinLeavesHeight)); zLeaf < (z + MinLeavesHeight + 1); zLeaf++)
			{
				//int xLeafWidth;
				if (IsIndexInBounds(i, yIndex + baseLeafPos, zLeaf) && (cubeArray[yIndex + baseLeafPos][i][zLeaf] == 0))
					cubeArray[yIndex + baseLeafPos][i][zLeaf] = 4;
				else {
					//Build on other chunk
					XMFLOAT3 toWorldPose = XMFLOAT3(yIndex + baseLeafPos + m_ChunkPosition.x, i + m_ChunkPosition.y, zLeaf + m_ChunkPosition.z);
					m_pChunkManager->Addblock(toWorldPose);
				}
			}
		}
	}
	

	//	int XLeafDifference = LeafDifference;
	//	for (int xIndex = -XLeafDifference; xIndex <= LeafDifference; xIndex++)
	//	{
	//		int ZLeafDifference = XLeafDifference + MinLeavesHeight;

	//		for (int zIndex = -ZLeafDifference; zIndex <= LeafDifference; zIndex++)
	//		{
	//			if (true) {
	//				Fill inn this chunk
	//				cubeArray[(x + xIndex)][y + leaveLength - (currentMaxLeafHeight - YIndex)][(z + zIndex)] = 4;


	//			}
	//			else {
	//				//fill in in neighbouring chunk 
	//				m_pChunkManager->AddBlockAtPos({ m_ChunkPos.x + (x + xIndex), m_ChunkPos.y +
	//					(y + leaveLength - (currentMaxLeafHeight - YIndex)) ,  m_ChunkPos.z + (z + zIndex) }, BlockTypes::LEAVES);
	//			}
	//		}

	//	}
	//for (int YIndex = leaveLength; YIndex <= currentMaxLeafHeight; YIndex++)
	//{

	//	int LeafDifference = 1 + (YIndex % (leaveLength / 4));
	//	int XLeafDifference = LeafDifference;
	//	for (int xIndex = -XLeafDifference; xIndex <= LeafDifference; xIndex++)
	//	{
	//		int ZLeafDifference = XLeafDifference + MinLeavesHeight;

	//		for (int zIndex = -ZLeafDifference; zIndex <= LeafDifference; zIndex++)
	//		{
	//			if (true) {
	//				Fill inn this chunk
	//				cubeArray[(x + xIndex)][y + leaveLength - (currentMaxLeafHeight - YIndex)][(z + zIndex)] = 4;


	//			}
	//			else {
	//				//fill in in neighbouring chunk 
	//				m_pChunkManager->AddBlockAtPos({ m_ChunkPos.x + (x + xIndex), m_ChunkPos.y +
	//					(y + leaveLength - (currentMaxLeafHeight - YIndex)) ,  m_ChunkPos.z + (z + zIndex) }, BlockTypes::LEAVES);
	//			}
	//		}

	//	}

	//}
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

					const std::map< Faces, std::vector<XMFLOAT2>>* uvCoords = m_pChunkManager->GetUVOfType(currentLookUpType);

					//Check if block under or above is solid
					if (yIndex != ChunkSizeY - 1) {
						if (yIndex != 0) { //Bottom Block Check
							int botIndex = yIndex - 1;
							if (!IsBlockSolid(xIndex,botIndex,zIndex)) {
								//is solid
								if (m_pChunkComponent->AddFace(m_ChunkPosition, XMFLOAT3{ static_cast<float>(xIndex), static_cast<float>(yIndex), static_cast<float>(zIndex) }, Faces::BOT, &(*uvCoords).at(Faces::BOT)))
									continue;
							}
						}
						int TopIndex = yIndex + 1;
						if (!IsBlockSolid(xIndex,TopIndex,zIndex))
						{
							if (m_pChunkComponent->AddFace(m_ChunkPosition,XMFLOAT3{ static_cast<float>(xIndex), static_cast<float>(yIndex), static_cast<float>(zIndex) }, Faces::TOP, &(*uvCoords).at(Faces::TOP)))
									continue;

						}
					}
					if (yIndex == (ChunkSizeY - 1)) //Always render faces at top of chunk
					{
						if (!IsBlockSolid(xIndex, yIndex, zIndex)) {
							if (m_pChunkComponent->AddFace(m_ChunkPosition, XMFLOAT3{ static_cast<float>(xIndex), static_cast<float>(yIndex), static_cast<float>(zIndex) }, Faces::TOP, &(*uvCoords).at(Faces::TOP)))
								continue;
						}
				
					}
					
					//WE DONT RENDER BOTTOM OF CHUNK

					//check left/right X
					if (xIndex != (ChunkSizeX)) { //left check
						int leftIndex = xIndex > 0 ? xIndex - 1 : 0;
						if (!IsBlockSolid(leftIndex, yIndex, zIndex))
							if (m_pChunkComponent->AddFace(m_ChunkPosition, XMFLOAT3{ static_cast<float>(xIndex), static_cast<float>(yIndex), static_cast<float>(zIndex) }, Faces::LEFT, &(*uvCoords).at(Faces::LEFT)))
								continue;

						// right check
						int rightIndex = xIndex < (ChunkSizeX - 1) ? xIndex + 1 : (ChunkSizeX - 1);
						if (!IsBlockSolid(rightIndex, yIndex, zIndex))
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
						if (!IsBlockSolid(xIndex, yIndex, backIndex))
							if (m_pChunkComponent->AddFace(m_ChunkPosition, XMFLOAT3{ static_cast<float>(xIndex), static_cast<float>(yIndex), static_cast<float>(zIndex) }, Faces::BACK, &(*uvCoords).at(Faces::BACK)))
								continue;

						//front check
						int frontIndex = zIndex < (ChunkSizeZ - 1) ? zIndex + 1 : (ChunkSizeZ - 1);
						if (!IsBlockSolid(xIndex, yIndex, frontIndex))
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
