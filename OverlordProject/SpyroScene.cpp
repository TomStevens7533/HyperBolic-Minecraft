#include "stdafx.h"
#include "SpyroScene.h"
#include "Materials/DiffuseMaterial.h"
#include "Materials/ColorMaterial.h"
#include "Prefabs/Character.h"
#include "Prefabs/ThirdPersonCharachter.h"

SpyroScene::SpyroScene() : GameScene(L"Spyro")
{

}

void SpyroScene::Initialize()
{
	const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);
	auto material = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
	material->SetDiffuseTexture(L"Spyro/level/LevelTex.png");

	//Level
	const auto pLevelObject = AddChild(new GameObject());
	const auto pLevelMesh = pLevelObject->AddComponent(new ModelComponent(L"Spyro/level/stonhill-reset.ovm"));
	pLevelMesh->SetMaterial(material);
	const auto pLevelActor = pLevelObject->AddComponent(new RigidBodyComponent(true));
	const auto pPxTriangleMesh = ContentManager::Load<PxTriangleMesh>(L"Spyro/level/stonhill-reset.ovpt");
	pLevelActor->AddCollider(PxTriangleMeshGeometry(pPxTriangleMesh, PxMeshScale({ .1f,.1f,.1f })), *pDefaultMaterial);
	pLevelObject->GetTransform()->Scale(.1f, .1f, .1f);

	//Skybox
	const auto pSkyBoxObject = AddChild(new GameObject());
	const auto pSkyBoMesh = pSkyBoxObject->AddComponent(new ModelComponent(L"Spyro/level/skybox.ovm"));
	pSkyBoMesh->GetTransform()->Rotate(DirectX::XMFLOAT3{ -190.f, 0.f, 0.f }, false);
	pSkyBoMesh->GetTransform()->Scale(DirectX::XMFLOAT3{ 1.5f, 1.5f, 1.5f });
	pSkyBoMesh->SetMaterial(MaterialManager::Get()->CreateMaterial<ColorMaterial>());

	//Player
	CharacterDesc characterDesc{ pDefaultMaterial };
	characterDesc.actionId_MoveForward = CharacterMoveForward;
	characterDesc.actionId_MoveBackward = CharacterMoveBackward;
	characterDesc.actionId_MoveLeft = CharacterMoveLeft;
	characterDesc.actionId_MoveRight = CharacterMoveRight;
	characterDesc.actionId_Jump = CharacterJump;

	m_pCharacter = AddChild(new ThirdPersonCharacter(characterDesc));
	m_pCharacter->GetTransform()->Translate(0.f, 5.f, 0.f);



	//Input
	auto inputAction = InputAction(CharacterMoveLeft, InputState::down, 'A', -1, XINPUT_GAMEPAD_DPAD_LEFT);
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterMoveRight, InputState::down, 'D', -1, XINPUT_GAMEPAD_DPAD_RIGHT);
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterMoveForward, InputState::down, 'W', XINPUT_GAMEPAD_DPAD_UP);
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterMoveBackward, InputState::down, 'S', XINPUT_GAMEPAD_DPAD_DOWN);
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterJump, InputState::pressed, VK_SPACE, -1, XINPUT_GAMEPAD_A);
	m_SceneContext.pInput->AddInputAction(inputAction);
}

void SpyroScene::Update()
{
	
}

void SpyroScene::Draw()
{

}

void SpyroScene::OnGUI()
{

}
