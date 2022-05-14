#include "stdafx.h"
#include "DiffuseMaterial_Shadow_Skinned.h"

DiffuseMaterial_Shadow_Skinned::DiffuseMaterial_Shadow_Skinned():
	Material(L"Effects/Shadow/PosNormTex3D_Shadow_Skinned.fx")
{}

void DiffuseMaterial_Shadow_Skinned::SetDiffuseTexture(const std::wstring& assetFile)
{
	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Texture(L"gDiffuseMap", m_pDiffuseTexture);
}

void DiffuseMaterial_Shadow_Skinned::InitializeEffectVariables()
{
}

void DiffuseMaterial_Shadow_Skinned::OnUpdateModelVariables(const SceneContext& sceneContext, const BaseComponent* pModel) const
{

	
	if (dynamic_cast<const ModelComponent*>(pModel)) {


		const ModelComponent* model = (dynamic_cast<const ModelComponent*>(pModel));
		/*
		* Update The Shader Variables
		* 1. Update the LightWVP > Used to Transform a vertex into Light clipping space
		* 	LightWVP = model_world * light_viewprojection
		* 	(light_viewprojection [LightVP] can be acquired from the ShadowMapRenderer)
		*/

		const auto pShadowMapRenderer = ShadowMapRenderer::Get();

		auto world = XMLoadFloat4x4(&pModel->GetTransform()->GetWorld());
		auto lvp = XMLoadFloat4x4(&pShadowMapRenderer->GetLightVP());
		DirectX::XMMATRIX wvp = world * lvp;
		XMFLOAT4X4 wvpS;
		DirectX::XMStoreFloat4x4(&wvpS, wvp);
		SetVariable_Matrix(L"gWorldViewProj_Light", wvpS);

		//Update shadow texture
		SetVariable_Texture(L"gShadowMap", pShadowMapRenderer->GetShadowMap());
		//Update lightdir
		SetVariable_Vector(L"gLightDirection", sceneContext.pLights->GetDirectionalLight().direction);

		// 3. Update the Light Direction (retrieve the direction from the LightManager > sceneContext)
		SetVariable_Vector(L"gLightDirection", sceneContext.pLights->GetDirectionalLight().direction);


		////Retrieve The Animator from the ModelComponent
		auto* animator = model->GetAnimator();

		////Make sure the animator is not NULL (ASSERT_NULL_)
		//ASSERT_NULL_(animator);
		ASSERT_NULL_(animator);

		////Retrieve the BoneTransforms from the Animator
		auto boneTransforms = animator->GetBoneTransforms();
		////Set the 'gBones' variable of the effect (MatrixArray) > BoneTransforms
		SetVariable_MatrixArray(L"gBones", &animator->GetBoneTransforms()[0]._11, static_cast<UINT>(boneTransforms.size()));
		//Update Shadow Variables
		//const auto pShadowMapRenderer = ShadowMapRenderer::Get();
		//...

		//auto boneTransforms = animator->GetBoneTransforms();
		//
	}


}
