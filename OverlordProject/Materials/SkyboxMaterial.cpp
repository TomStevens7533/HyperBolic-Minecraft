#include "stdafx.h"
#include "SkyboxMaterial.h"

SkyboxMayerial::SkyboxMayerial() :
	Material(L"Effects/SkyBox.fx")
{
}

void SkyboxMayerial::InitializeEffectVariables()
{
	m_CubeTexture = ContentManager::Load<TextureData>(L"Textures/Sunol_Cubemap.dds");
	SetVariable_Texture(L"gDiffuseMap", m_CubeTexture);


}

