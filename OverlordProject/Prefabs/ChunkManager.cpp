#include "stdafx.h"
#include "ChunkManager.h"
#include "ChunkPrefab.h"
#include "Materials/ChunkDiffuseMaterial.h"
#include "Materials/Shadow/ChunkShadowDiffuseMaterial.h"
#include <thread>


ChunkManager::ChunkManager(DirectX::XMFLOAT3 originPos) : m_OriginPos{originPos}
{
	m_IsShutdown = true;
	//Seed calc
	m_Seed = static_cast<unsigned int>(time(NULL));

}

ChunkManager::~ChunkManager()
{
	m_IsShutdown = false;
}

void ChunkManager::DrawImGui()
{

}

void ChunkManager::UpdateChunksAroundPos(const SceneContext& sc)
{
	std::unique_lock lock(m_Mutex);
	
	while (m_IsShutdown) {

		int xEnd = static_cast<int>(m_OriginXPos) - (ChunkSizeX * m_ChunkDistance);
		int zEnd = static_cast<int>(m_OriginZPos) - (ChunkSizeZ * m_ChunkDistance);
		for (int x = 0; x < (m_ChunkDistance * 2); x++)
		{
			for (int z = 0; z < (m_ChunkDistance * 2); z++)
			{
				int xWorldPos = xEnd + (ChunkSizeX * x);
				int zWorldPos = zEnd + (ChunkSizeZ * z);

				//break loop thread if required
				if (m_IsShutdown == false)
					return;

				if (m_ChunkVec.count(std::make_pair(xWorldPos, zWorldPos)) > 0) {
					//if chunk already exist
					if (m_ChunkVec[std::make_pair(xWorldPos, zWorldPos)]->GetDirtyFlag() == true) {
						m_ChunkVec[std::make_pair(xWorldPos, zWorldPos)]->UpdateMesh(sc);
					}

					continue;
				}
				else {
					//Create new chunk
					lock.unlock();
					std::cout << "Creating new chunk: [" << xWorldPos << ", " << zWorldPos << "]\n";
					ChunkPrefab* newChunk = new ChunkPrefab(XMFLOAT3(static_cast<float>(xWorldPos), 0, static_cast<float>(zWorldPos)), this, m_pMaterial, m_Seed);
					newChunk->UpdateMesh(sc);


					//Update previous mesh to exclude not seen faces
					ReloadNeigbourhingChunks(std::make_pair(xWorldPos, zWorldPos));
					lock.lock();
					m_ChunkVec[std::make_pair(xWorldPos, zWorldPos)] = AddChild(newChunk);

				}
			}
		}
	}


}

void ChunkManager::SetNewOriginPos(const XMFLOAT3& newOrigin)
{
	m_OriginXPos = static_cast<float>(static_cast<int>(newOrigin.x) - (static_cast<int>(newOrigin.x) % ChunkSizeX));
	m_OriginYPos = static_cast<float>(static_cast<int>(newOrigin.y) - (static_cast<int>(newOrigin.y) % ChunkSizeY));
	m_OriginZPos = static_cast<float>(static_cast<int>(newOrigin.z) - (static_cast<int>(newOrigin.z) % ChunkSizeZ));

}

uint8_t ChunkManager::RemoveBlock(XMFLOAT3 position)
{
	XMFLOAT3 RemoveChunkPos;

	RemoveChunkPos.x = static_cast<float>(static_cast<int>(position.x) - (static_cast<int>(position.x) % ChunkSizeX));

	int mulX = (position.x < 0 ? static_cast<int>((static_cast<int>(position.x)) / ChunkSizeX) - 1 : (static_cast<int>(position.x) / ChunkSizeX));
	int mulZ = (position.z < 0 ? static_cast<int>((static_cast<int>(position.z)) / ChunkSizeZ) - 1 : (static_cast<int>(position.z) / ChunkSizeZ));
	int Chunkx = ChunkSizeX * mulX;
	int Chunkz = ChunkSizeZ * mulZ;


	std::pair<int, int> Key = std::make_pair(Chunkx, Chunkz);
	if (m_ChunkVec.count(Key) > 0) {
		int localX =  position.x > 0 ? std::abs((static_cast<int>(std::floor(position.x)) % ChunkSizeX)) 
			: ChunkSizeX - std::abs((static_cast<int>(std::floor(position.x)) % ChunkSizeX));

		int localy = position.y <= (ChunkSizeY - 1) ? static_cast<int>(position.y) % ChunkSizeY : (ChunkSizeY - 1);

		int localz = position.z > 0 ? std::abs((static_cast<int>(std::floor(position.z)) % ChunkSizeZ))
			: ChunkSizeZ - std::abs((static_cast<int>(std::floor(position.z)) % ChunkSizeZ));

		uint8_t id = m_ChunkVec[Key]->DeleteBlock(localX, localy, localz);
		if (id != 0) {
			ReloadNeigbourhingChunks(Key);
			return id;

		}
	}

	return 0;
}
bool ChunkManager::IsBlockSolid(XMFLOAT3 position) const
{
	XMFLOAT3 RemoveChunkPos;

	RemoveChunkPos.x = static_cast<float>(static_cast<int>(position.x) - (static_cast<int>(position.x) % ChunkSizeX));

	int mulX = (position.x < 0 ? static_cast<int>((static_cast<int>(position.x)) / ChunkSizeX) - 1 : (static_cast<int>(position.x) / ChunkSizeX));
	int mulZ = (position.z < 0 ? static_cast<int>((static_cast<int>(position.z)) / ChunkSizeZ) - 1 : (static_cast<int>(position.z) / ChunkSizeZ));
	int Chunkx = ChunkSizeX * mulX;
	int Chunkz = ChunkSizeZ * mulZ;


	std::pair<int, int> Key = std::make_pair(Chunkx, Chunkz);
	if (m_ChunkVec.count(Key) > 0) {
		int localX = position.x > 0 ? std::abs((static_cast<int>(std::floor(position.x)) % ChunkSizeX))
			: ChunkSizeX - std::abs((static_cast<int>(std::floor(position.x)) % ChunkSizeX));

		int localy = position.y <= (ChunkSizeY - 1) ? static_cast<int>(std::floor(position.y) -5) % ChunkSizeY : (ChunkSizeY - 1);

		int localz = position.z > 0 ? std::abs((static_cast<int>(std::floor(position.z)) % ChunkSizeZ))
			: ChunkSizeZ - std::abs((static_cast<int>(std::floor(position.z)) % ChunkSizeZ));


		auto it = (m_ChunkVec.find(Key));
		if(it->second->IsBlockSolid(localX, localy, localz)) {
			return true;
		}
		else
			return false;
	}
	return false;
}

bool ChunkManager::Addblock(XMFLOAT3 position, uint8_t id)
{

	XMFLOAT3 addChunkPos;

	addChunkPos.x = static_cast<float>(static_cast<int>(position.x) - (static_cast<int>(position.x) % ChunkSizeX));

	int mulX = (position.x < 0 ? static_cast<int>((static_cast<int>(position.x)) / ChunkSizeX) - 1 : (static_cast<int>(position.x) / ChunkSizeX));
	int mulZ = (position.z < 0 ? static_cast<int>((static_cast<int>(position.z)) / ChunkSizeZ) - 1 : (static_cast<int>(position.z) / ChunkSizeZ));
	int Chunkx = ChunkSizeX * mulX;
	int Chunkz = ChunkSizeZ * mulZ;


	std::pair<int, int> Key = std::make_pair(Chunkx, Chunkz);
	if (m_ChunkVec.count(Key) > 0) {
		int localX = position.x > 0 ? std::abs((static_cast<int>(position.x) % ChunkSizeX))
			: ChunkSizeX - std::abs((static_cast<int>(position.x) % ChunkSizeX));

		int localy = position.y <= (ChunkSizeY - 1) ? static_cast<int>(position.y) % ChunkSizeY : (ChunkSizeY - 1);

		int localz = position.z > 0 ? std::abs((static_cast<int>(position.z) % ChunkSizeZ))
			: ChunkSizeZ - std::abs((static_cast<int>(position.z) % ChunkSizeZ));

		std::cout << localX << " " << localz << std::endl;

		if (m_ChunkVec[Key]->AddBlock(id, localX, localy, localz)) {
			ReloadNeigbourhingChunks(Key);
			return true;

		}
	}
	return false;
}

bool ChunkManager::IsBlockInChunkSolid(std::pair<int, int> chunkPos, int x, int y, int z) const
{
	std::map<std::pair<int, int>, ChunkPrefab*>::const_iterator it = m_ChunkVec.find(chunkPos);
	if (it != m_ChunkVec.cend()) {
		return it->second->IsBlockSolid(x, y, z);
	}
	else {
		return true;
	}

}

void ChunkManager::ReloadNeigbourhingChunks(std::pair<int, int> chunkPos)
{
	for (size_t i = 0; i < 4; i++)
	{
		std::pair<int, int> lookUpIndex = chunkPos;
		if (i < 2) {
			lookUpIndex.first += (i % 2 == 1) ? ChunkSizeX : -ChunkSizeX;

		}
		else {
			lookUpIndex.second += (i % 2 == 1) ? ChunkSizeZ : -ChunkSizeZ;
		}

		auto it = m_ChunkVec.find(lookUpIndex);
		if (it != m_ChunkVec.end()) //found chunk
		{
			it->second->SetDirty();
		}


	}


}

const std::map< Faces, std::vector<XMFLOAT2>>* ChunkManager::GetUVOfType(uint8_t id) const
{
	return  m_LevelJsonParser.GetUVOfType(id);
}

void ChunkManager::Initialize(const SceneContext& sc)
{
	m_pMaterial = MaterialManager::Get()->CreateMaterial<ChunkShadowDifffuseMaterial>();
	m_pMaterial->SetDiffuseTexture(L"Textures/newAtlas.png");
	m_LevelJsonParser.ParseFile(L"Resources/Block.json");

	 m_UpdateChunkThread = std::jthread(&ChunkManager::UpdateChunksAroundPos, this, std::ref(sc));
}

void ChunkManager::Update(const SceneContext&)
{


}
BlockJsonParser ChunkManager::m_LevelJsonParser;

