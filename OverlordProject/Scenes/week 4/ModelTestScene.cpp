//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"
#include "ModelTestScene.h"
#include "Materials/ColorMaterial.h"
#include "Materials/DiffuseMaterial.h"
#include "Prefabs/CubePrefab.h"
#include "Prefabs/SpherePrefab.h"


ModelTestScene::ModelTestScene() :
	GameScene(L"MinionScene") {}

void ModelTestScene::Initialize()
{

	auto pMaterial = PxGetPhysics().createMaterial(.5f, .5f, .3f);
	GameSceneExt::CreatePhysXGroundPlane(*this, pMaterial);

	m_SceneContext.settings.enableOnGUI = true;
	m_SceneContext.settings.drawPhysXDebug = true;

	//Create Material
	auto material = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
	material->SetDiffuseTexture(L"Textures/Chair_Dark.dds");

	auto modelGo = m_pChair = AddChild(new GameObject());
	auto modelComp = modelGo->AddComponent(new ModelComponent(L"Meshes/Chair.ovm"));
	modelComp->SetMaterial(material->GetMaterialId());

	//Add physics
	auto pModelActor = modelGo->AddComponent(new RigidBodyComponent(false));
	const auto pTriangleMesh = ContentManager::Load<PxConvexMesh>(L"Meshes/Chair.ovpc");
	pModelActor->AddCollider(PxConvexMeshGeometry{ pTriangleMesh }, *pMaterial);

	m_pChair->GetTransform()->Translate(0, 10, 0);
}

void ModelTestScene::Update()
{
	//Optional
}

void ModelTestScene::Draw()
{
	//Optional
	
}

void ModelTestScene::OnGUI()
{
	ImGui::Text("This only activates if\n SceneSettings.enableOnGUI is True.\n\n");
	ImGui::Text("Use ImGui to add custom\n controllable scene parameters!");
	ImGui::ColorEdit3("Demo ClearColor", &m_SceneContext.settings.clearColor.x, ImGuiColorEditFlags_NoInputs);
}
