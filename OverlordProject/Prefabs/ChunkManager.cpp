#include "stdafx.h"
#include "ChunkManager.h"
#include "ChunkPrefab.h"
#include "Materials/ChunkDiffuseMaterial.h"
#include "Materials/Shadow/ChunkShadowDiffuseMaterial.h"
#include <thread>
BlockJsonParser ChunkManager::m_LevelJsonParser;

ChunkManager::ChunkManager(DirectX::XMFLOAT3 originPos) : m_OriginPos{originPos}
{
	m_IsShutdown = true;
	//Seed calc
	m_Seed = static_cast<unsigned int>(time(NULL));

}

ChunkManager::~ChunkManager()
{
	m_IsShutdown = false;

	//Wait for update chunk
	//wait till cycle is done
	std::unique_lock<std::mutex> lock1(m_MutexUpdate);
	cond.notify_all();
	cond.wait(lock1, [this]() {return m_IsCycleUpdateDone == true; });
	lock1.unlock();


}

void ChunkManager::DrawImGui()
{

}

void ChunkManager::UpdateChunksAroundPos(const SceneContext& sc)
{

	while (m_IsShutdown) {

		float newDistance = m_ChunkDistance / 1.5f;
		int xEnd = static_cast<int>(m_OriginXPos) - (ChunkSizeX * ((int)newDistance));
		int zEnd = static_cast<int>(m_OriginZPos) - (ChunkSizeZ * ((int)newDistance));

		std::unique_lock<std::mutex> lock1(m_MutexUpdate);

		for (int x = 0; x < (newDistance  *2.f); x++)
		{
			for (int z = 0; z < (newDistance * 2.f); z++)
			{
				int xWorldPos = xEnd + (ChunkSizeX * x);
				int zWorldPos = zEnd + (ChunkSizeZ * z);
				lock1.unlock();
				if (m_IsShutdown == false) {
					cond.notify_one();
					m_IsCycleUpdateDone = true;
					return;
				}
				lock1.lock();
				if (m_IsCycleUpdateDone == false && m_ChunkVec.contains(std::make_pair(xWorldPos, zWorldPos))) {
					m_IsCycleUpdateDone = false;
					if (m_ChunkVec[std::make_pair(xWorldPos, zWorldPos)]->GetDirtyFlag() == true) {
						m_ChunkVec[std::make_pair(xWorldPos, zWorldPos)]->UpdateMesh(sc);
					}
				}

			}
		}
		if (m_IsCycleUpdateDone == false) {
			m_IsCycleUpdateDone = true;
			m_IsCycleCreateDone = false;
		}



	}
}



void ChunkManager::Update(const SceneContext&)
{

	if (m_IsCycleUpdateDone) {
		int xEnd = static_cast<int>(m_OriginXPos) - (ChunkSizeX * m_ChunkDistance);
		int zEnd = static_cast<int>(m_OriginZPos) - (ChunkSizeZ * m_ChunkDistance);

		std::unique_lock<std::mutex> lock1(m_MutexUpdate);
		for (int x = 0; x < (m_ChunkDistance * 2); x++)
		{
			for (int z = 0; z < (m_ChunkDistance * 2); z++)
			{
				lock1.unlock();
				int xWorldPos = xEnd + (ChunkSizeX * x);
				int zWorldPos = zEnd + (ChunkSizeZ * z);
				if (m_IsShutdown == false) {
					cond.notify_one();
					m_IsCycleCreateDone = true;
					return;
				}
				lock1.lock();
				if (m_IsCycleCreateDone == false && m_ChunkVec.contains(std::make_pair(xWorldPos, zWorldPos)) == false) {
					std::cout << "Make chunk\n";
					ChunkPrefab* newChunk = new ChunkPrefab(XMFLOAT3(static_cast<float>(xWorldPos), 0, static_cast<float>(zWorldPos)), this, m_pMaterial, m_Seed);
					m_ChunkVec.insert(std::make_pair(std::make_pair(xWorldPos, zWorldPos), AddChild(newChunk)));

				}


			}
		}
		m_IsCycleCreateDone = true;
		m_IsCycleUpdateDone = false;
		cond.notify_all();
	}




}
void ChunkManager::Initialize(const SceneContext& sc)
{
	m_pMaterial = MaterialManager::Get()->CreateMaterial<ChunkShadowDifffuseMaterial>();
	m_pMaterial->SetDiffuseTexture(L"Textures/newAtlas.png");
	m_LevelJsonParser.ParseFile(L"Resources/Block.json");

#ifdef DEBUG
	std::cout << "Loading chunks in debug is slow consider switching to release\n";
#endif // DEBUG

	 m_UpdateChunkThread = std::jthread(&ChunkManager::UpdateChunksAroundPos, this, std::ref(sc));

}

void ChunkManager::SetNewOriginPos(const XMFLOAT3& newOrigin)
{
	m_OriginXPos = static_cast<float>(static_cast<int>(newOrigin.x) - (static_cast<int>(newOrigin.x) % ChunkSizeX));
	m_OriginYPos = static_cast<float>(static_cast<int>(newOrigin.y) - (static_cast<int>(newOrigin.y) % ChunkSizeY));
	m_OriginZPos = static_cast<float>(static_cast<int>(newOrigin.z) - (static_cast<int>(newOrigin.z) % ChunkSizeZ));

}

uint8_t ChunkManager::RemoveBlock(XMFLOAT3 position)
{

	auto chunkIndex =  WorldToChunkIndex(position);
	std::pair<int, int> Key = std::make_pair(chunkIndex.first, chunkIndex.second);
	if (m_ChunkVec.count(Key) > 0) {
		
		ChunkPosistion localPos = WorldToLocalChunkPos(position);

		uint8_t id = m_ChunkVec[Key]->DeleteBlock(localPos.x, localPos.y, localPos.z);
		if (id != 0) {
			ReloadNeigbourhingChunks(Key, localPos);
			return id;

		}
	}
	return 0;
}
bool ChunkManager::IsBlockSolid(XMFLOAT3 position) const
{
	auto chunkIndex = WorldToChunkIndex(position);
	std::pair<int, int> Key = std::make_pair(chunkIndex.first, chunkIndex.second);
	if (m_ChunkVec.count(Key) > 0) {
		ChunkPosistion localPos = WorldToLocalChunkPos(position);


		if(m_ChunkVec.at(Key)->IsBlockSolid(localPos.x, localPos.y, localPos.z)) {
			return true;
		}
		else
			return false;
	}
	return false;
}

bool ChunkManager::Addblock(XMFLOAT3 position, uint8_t id)
{
	
	auto chunkIdx = WorldToChunkIndex(position);
	std::pair<int, int> Key = std::make_pair(chunkIdx.first, chunkIdx.second);
	if (m_ChunkVec.count(Key) > 0) {
		
		auto pos = WorldToLocalChunkPos(position);


		if (m_ChunkVec[Key]->AddBlock(id, pos.x, pos.y, pos.z)) {
			ReloadNeigbourhingChunks(Key, pos);
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

bool ChunkManager::IsBlockInChunkSolid(XMFLOAT3 pos) const
{
	int mulX = (pos.x < 0 ? static_cast<int>((static_cast<int>(pos.x)) / ChunkSizeX) - 1 : (static_cast<int>(pos.x) / ChunkSizeX));
	int mulZ = (pos.z < 0 ? static_cast<int>((static_cast<int>(pos.z)) / ChunkSizeZ) - 1 : (static_cast<int>(pos.z) / ChunkSizeZ));
	int Chunkx = ChunkSizeX * mulX;
	int Chunkz = ChunkSizeZ * mulZ;


	std::pair<int, int> Key = std::make_pair(Chunkx, Chunkz);
	if (m_ChunkVec.count(Key) > 0) {
		int localX = pos.x > 0 ? std::abs(((static_cast<int>((pos.x)))) % ChunkSizeX)
			: ChunkSizeX - std::abs((static_cast<int>((pos.x))) % ChunkSizeX);

		int localy = pos.y <= (ChunkSizeY - 1) ? (static_cast<int>((pos.y)) % ChunkSizeY) : (ChunkSizeY - 1);

		int localz = pos.z > 0 ? std::abs((static_cast<int>((pos.z))) % ChunkSizeZ)
			: ChunkSizeZ - std::abs((static_cast<int>((pos.z)) % ChunkSizeZ));

		return m_ChunkVec.at(Key)->IsBlockSolid(localX, localy, localz);
	}
	return false;
}

void ChunkManager::ReloadNeigbourhingChunks(std::pair<int, int> chunkPos, ChunkPosistion pos)
{
	if (pos.x == (ChunkSizeX - 1)) {
		chunkPos.first += ChunkSizeX;
		m_ChunkVec.at(chunkPos)->SetDirty();;
	}
	else if (pos.x == 0) {
		chunkPos.first -= ChunkSizeX;
		m_ChunkVec.at(chunkPos)->SetDirty();;
	}
	if (pos.z == (ChunkSizeZ - 1)) {
		chunkPos.second += ChunkSizeZ;
		m_ChunkVec.at(chunkPos)->SetDirty();;
	}
	else if (pos.z == 0) {
		chunkPos.second -= ChunkSizeZ;
		m_ChunkVec.at(chunkPos)->SetDirty();
	}

}

const std::map< Faces, std::vector<XMFLOAT2>>* ChunkManager::GetUVOfType(uint8_t id) const
{
	return  m_LevelJsonParser.GetUVOfType(id);
}



std::pair<int, int> ChunkManager::GetChunkIdx(XMFLOAT3 pos) const
{
	int mulX = (pos.x < 0 ? static_cast<int>((static_cast<int>(pos.x)) / ChunkSizeX) - 1 : (static_cast<int>(pos.x) / ChunkSizeX));
	int mulZ = (pos.z < 0 ? static_cast<int>((static_cast<int>(pos.z)) / ChunkSizeZ) - 1 : (static_cast<int>(pos.z) / ChunkSizeZ));
	int Chunkx = ChunkSizeX * mulX;
	int Chunkz = ChunkSizeZ * mulZ;


	std::pair<int, int> Key = std::make_pair(Chunkx, Chunkz);
	return Key;
}

ChunkPosistion ChunkManager::WorldToLocalChunkPos(XMFLOAT3 position) const
{
	auto chunkIndex = WorldToChunkIndex(position);

	int dif = (int)position.x - (chunkIndex.first + (chunkIndex.first >= 0 ? 0 : 1));
	int localX = std::abs(dif) % ChunkSizeX;

	int localy = std::abs(((int)(position.y) % ChunkSizeY));

	dif = (int)position.z - (chunkIndex.second + (chunkIndex.second >= 0 ? 0 : 1));
	int localz = std::abs(dif) % ChunkSizeZ;

	return ChunkPosistion(localX, localy, localz);

}

std::pair<int, int> ChunkManager::WorldToChunkIndex(XMFLOAT3 position) const
{
	int mulX = (position.x < 0 ? static_cast<int>((static_cast<int>(position.x)) / ChunkSizeX) - 1 : (static_cast<int>(position.x) / ChunkSizeX));
	int mulZ = (position.z < 0 ? static_cast<int>((static_cast<int>(position.z)) / ChunkSizeZ) - 1 : (static_cast<int>(position.z) / ChunkSizeZ));
	int Chunkx = ChunkSizeX * mulX;
	int Chunkz = ChunkSizeZ * mulZ;

	return std::make_pair(Chunkx, Chunkz);
}

