//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"
#include "UberMaterial.h"

UberMaterial::UberMaterial() : Material(L"Effects/UberShader.fx")
{
	//InitializeEffectVariables();
}

UberMaterial::~UberMaterial()
{



}

void UberMaterial::SetDiffuseTexture(const std::wstring& assetFile)
{
	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Texture(L"gTextureDiffuse", m_pDiffuseTexture);
}




void UberMaterial::InitializeEffectVariables()
{
	SetDiffuseTexture(L"Textures/Skulls_Tex/Skulls_Diffusemap.tga");
	SetVariable_Scalar(L"gUseTextureDiffuse", true);
	SetVariable_Vector(L"gColorDiffuse", { 0.8f, 0.f, 0.2f });
	

	//Specular
	SetVariable_Scalar(L"gShininess", 15.f);
	SetVariable_Vector(L"gColorSpecular", { 0.349f, 0.79f, 1.f });
	SetVariable_Scalar(L"gUseSpecularBlinn", true);
	SetVariable_Scalar(L"gUseSpecularPhong", true);

	//Fresnel
	SetVariable_Scalar(L"gUseFresnelFalloff", true);
	SetVariable_Scalar(L"gFresnelPower", 1.f);
	SetVariable_Scalar(L"gFresnelMultiplier", 1.f);
	SetVariable_Scalar(L"gFresnelHardness", 5.f);
	SetVariable_Vector(L"gColorFresnel", { 0.849f, 0.145f, 1.f });






	//Normal
	m_pNormalTexture = ContentManager::Load<TextureData>(L"Textures/Skulls_Tex/Skulls_Normalmap.tga");
	SetVariable_Texture(L"gTextureNormal", m_pNormalTexture);
	SetVariable_Scalar(L"gUseTextureNormal", true);

	//enviroment
	SetVariable_Scalar(L"gUseEnvironmentMapping", true);
	m_pCubeTexture = ContentManager::Load<TextureData>(L"Textures/Sunol_Cubemap.dds");
	SetVariable_Texture(L"gCubeEnvironment", m_pCubeTexture);


	////specular





}
