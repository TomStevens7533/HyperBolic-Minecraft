//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"
#include "UberMaterialScene.h"
#include "Prefabs/CubePrefab.h"
#include "Prefabs/SpherePrefab.h"
#include <Materials/DiffuseMaterial.h>


UberMaterialScene::UberMaterialScene() :
	GameScene(L"UberScene") {}

void UberMaterialScene::Initialize()
{


	m_SceneContext.settings.enableOnGUI = true;
	m_SceneContext.settings.drawGrid = false;

	//Create Material
	m_UberMaterial = MaterialManager::Get()->CreateMaterial<UberMaterial>();
	auto matrix = m_SceneContext.pCamera->GetViewProjection();

	m_pSphere = AddChild(new GameObject());
	auto modelComp = m_pSphere->AddComponent(new ModelComponent(L"Meshes/Sphere_round.ovm"));

	modelComp->SetMaterial(m_UberMaterial->GetMaterialId());

	modelComp->GetTransform()->Scale({ 20.f, 20.f, 20.f });

	m_Time.Start();
	
}

void UberMaterialScene::Update()
{
	m_AngleYaxis += 90.f * m_Time.GetElapsed();
	m_Time.Update();
	m_pSphere->GetTransform()->Rotate(0.f, m_AngleYaxis, 0.f, true);
}

void UberMaterialScene::Draw()
{
	//Optional
}

void UberMaterialScene::OnGUI()
{
	m_UberMaterial->DrawImGui();
}