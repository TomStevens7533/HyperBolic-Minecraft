//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"
#include "ChunkDiffuseMaterial.h"

ChunkDiffuseMaterial::ChunkDiffuseMaterial() : Material(L"Effects/PosTex3D.fx")
{
}

void ChunkDiffuseMaterial::SetDiffuseTexture(const std::wstring& assetFile)
{
	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Texture(L"gDiffuseMap", m_pDiffuseTexture);
}

void ChunkDiffuseMaterial::InitializeEffectVariables()
{
}
