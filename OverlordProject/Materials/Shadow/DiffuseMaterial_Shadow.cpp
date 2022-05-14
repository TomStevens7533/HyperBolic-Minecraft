#include "stdafx.h"
#include "DiffuseMaterial_Shadow.h"

DiffuseMaterial_Shadow::DiffuseMaterial_Shadow():
	Material(L"Effects/Shadow/PosNormTex3D_Shadow.fx")
{}

void DiffuseMaterial_Shadow::SetDiffuseTexture(const std::wstring& assetFile)
{
	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Texture(L"gDiffuseMap", m_pDiffuseTexture);
}

void DiffuseMaterial_Shadow::InitializeEffectVariables()
{
}

void DiffuseMaterial_Shadow::OnUpdateModelVariables(const SceneContext& sceneContext, const BaseComponent* pModel) const
{
	/*
	 * TODO_W8
	 * Update The Shader Variables
	 * 1. Update the LightWVP > Used to Transform a vertex into Light clipping space
	 * 	LightWVP = model_world * light_viewprojection
	 * 	(light_viewprojection [LightVP] can be acquired from the ShadowMapRenderer)
	 *
	 * 2. Update the ShadowMap texture
	 *
	 * 3. Update the Light Direction (retrieve the direction from the LightManager > sceneContext)
	*/
	const auto pShadowMapRenderer = ShadowMapRenderer::Get();

	auto world = XMLoadFloat4x4(&pModel->GetTransform()->GetWorld());
	auto lvp = XMLoadFloat4x4(&pShadowMapRenderer->GetLightVP());
	auto wvp = world * lvp;
	XMFLOAT4X4 wvpS;
	DirectX::XMStoreFloat4x4(&wvpS, wvp);
	SetVariable_Matrix(L"gWorldViewProj_Light", wvpS);

	//Update shadow texture
	SetVariable_Texture(L"gShadowMap", pShadowMapRenderer->GetShadowMap());
	//Update lightdir
	SetVariable_Vector(L"gLightDirection", sceneContext.pLights->GetDirectionalLight().direction);
	
	//Update Shadow Variables
	//const auto pShadowMapRenderer = ShadowMapRenderer::Get();
	//...


}
