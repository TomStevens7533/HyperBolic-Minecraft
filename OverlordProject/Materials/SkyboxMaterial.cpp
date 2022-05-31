#include "stdafx.h"
#include "SkyboxMaterial.h"

SkyboxMaterial::SkyboxMaterial() :
	Material(L"Effects/SkyBox.fx")
{
}

void SkyboxMaterial::InitializeEffectVariables()
{
	m_CubeTexture = ContentManager::Load<TextureData>(L"Textures/skybox.dds");
	SetVariable_Texture(L"gDiffuseMap", m_CubeTexture);


}

