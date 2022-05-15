#include "stdafx.h"
#include "PostProcessingScene.h"

//Uncomment for shadow variants
//#include "Materials/Shadow/DiffuseMaterial_Shadow.h"
//#include "Materials/Shadow/DiffuseMaterial_Shadow_Skinned.h"

#include "Materials/DiffuseMaterial.h"
#include "Materials/DiffuseMaterial_Skinned.h"

#include "Materials/Post/PostGrayscale.h"
#include "Materials/Post/PostBlur.h"
//#include "Materials/Post/PostBlur.h"

void PostProcessingScene::Initialize()
{
	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.settings.enableOnGUI = true;

	m_SceneContext.pLights->SetDirectionalLight({ -95.6139526f,66.1346436f,-41.1850471f }, { 0.740129888f, -0.597205281f, 0.309117377f });

	//Materials
	//*********
	//const auto pPeasantMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow_Skinned>(); //Shadow variant
	const auto pPeasantMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Skinned>();

	//const auto pGroundMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>(); //Shadow variant

	//Ground Mesh
	//***********


	//Character Mesh
	//**************
	const auto pObject = AddChild(new GameObject);
	const auto pModel = pObject->AddComponent(new ModelComponent(L"Meshes/PeasantGirl.ovm"));
	pModel->SetMaterial(pPeasantMaterial);

	pObject->GetTransform()->Scale(0.1f, 0.1f, 0.1f);

	if (const auto pAnimator = pModel->GetAnimator())
	{
		pAnimator->SetAnimation(2);
		pAnimator->Play();
	}

	//Post Processing Stack
	//=====================
	m_pPostGrayscale = MaterialManager::Get()->CreateMaterial<PostGrayscale>();
	m_pPostBlur = MaterialManager::Get()->CreateMaterial<PostBlur>();

	//m_pPostBlur = ...

	AddPostProcessingEffect(m_pPostGrayscale);
	AddPostProcessingEffect(m_pPostBlur);

	//TODO_W10
	//Create and add a PostBlur material class (using Blur.fx)
	//Add the material to the PostProcessing Stack + Uncomment the corresponding ImGui code below
	//If you already have shadow mapping working you can change the code above to use the shadow variant shaders
}

void PostProcessingScene::OnGUI()
{
	bool isEnabled = m_pPostGrayscale->IsEnabled();
	ImGui::Checkbox("Grayscale PP", &isEnabled);
	m_pPostGrayscale->SetIsEnabled(isEnabled);
	m_pPostGrayscale	->SetIsEnabled(isEnabled);

	bool isBlurEnabled = m_pPostBlur->IsEnabled();
	ImGui::Checkbox("Blur PP", &isBlurEnabled);
	m_pPostBlur->SetIsEnabled(isBlurEnabled);
}
