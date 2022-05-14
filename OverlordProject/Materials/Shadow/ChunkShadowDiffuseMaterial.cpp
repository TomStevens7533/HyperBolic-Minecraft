#include "stdafx.h"
#include "ChunkShadowDiffuseMaterial.h"

ChunkShadowDifffuseMaterial::ChunkShadowDifffuseMaterial() :
	Material(L"Effects/PosTex3D.fx")
{}

void ChunkShadowDifffuseMaterial::SetDiffuseTexture(const std::wstring& assetFile)
{
	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Texture(L"gDiffuseMap", m_pDiffuseTexture);
}

void ChunkShadowDifffuseMaterial::InitializeEffectVariables()
{
}

void ChunkShadowDifffuseMaterial::OnUpdateModelVariables(const SceneContext& sceneContext, const BaseComponent* pModel) const
{
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

}
