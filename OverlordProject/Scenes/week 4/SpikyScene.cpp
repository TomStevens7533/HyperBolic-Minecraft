//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"
#include "SpikyScene.h"
#include "Materials/UberMaterial.h"
#include "Prefabs/CubePrefab.h"
#include "Prefabs/SpherePrefab.h"


SpikyScene::SpikyScene() :
	GameScene(L"SpikyScene") {}

void SpikyScene::Initialize()
{


	m_SceneContext.settings.enableOnGUI = true;
	m_SceneContext.settings.drawGrid = false;


	//Create Material
	m_pSpikyMaterial = MaterialManager::Get()->CreateMaterial<SpikyMaterial>();
	m_pSphere = AddChild(new GameObject());
	auto modelComp = m_pSphere->AddComponent(new ModelComponent(L"Meshes/Sphere_Octa.ovm"));

	modelComp->SetMaterial(m_pSpikyMaterial->GetMaterialId());

	modelComp->GetTransform()->Scale({ 15.f, 15.f, 15.f });
	m_Time.Start();

}

void SpikyScene::Update()
{
	m_AngleYaxis += 20.f * m_Time.GetElapsed();
	m_Time.Update();
	m_pSphere->GetTransform()->Rotate(0.f,m_AngleYaxis, 0.f, true);

}

void SpikyScene::Draw()
{
	//Optional

}

void SpikyScene::OnGUI()
{
	ImGui::Text("This only activates if\n SceneSettings.enableOnGUI is True.\n\n");
	ImGui::Text("Use ImGui to add custom\n controllable scene parameters!");
	ImGui::ColorEdit3("Demo ClearColor", &m_SceneContext.settings.clearColor.x, ImGuiColorEditFlags_NoInputs);

	m_pSpikyMaterial->DrawImGui();

}
