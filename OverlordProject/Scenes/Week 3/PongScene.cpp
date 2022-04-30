//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"
#include "PongScene.h"

#include "Prefabs/CubePrefab.h"
#include "Prefabs/SpherePrefab.h"


PongScene::PongScene()
	:GameScene(L"PongScene")
{
	std::cout << "Use arrow keys for movement\n";
}
void PongScene::Initialize()
{
	m_SceneContext.settings.enableOnGUI = true;

	auto pBouncyMaterial = PxGetPhysics().createMaterial(.1f, .1f, 1.f);

	//Sphere 1
	m_pSphere = AddChild(new SpherePrefab(1.f, 10, XMFLOAT4{ Colors::Red }));
	auto pSphereActor = m_pSphere->AddComponent(new RigidBodyComponent());
	pSphereActor->SetConstraint(RigidBodyConstraint::TransY, false);
	pSphereActor->AddCollider(PxSphereGeometry{ 1.f }, *pBouncyMaterial);

	m_pSphere->GetTransform()->Translate(0.f, 0.f, -80.f);
	pSphereActor->AddForce(XMFLOAT3{ m_BallSpeed, 0.f, m_BallSpeed }, PxForceMode::eIMPULSE);

	static_cast<FreeCamera*>(m_SceneContext.pCamera->GetGameObject())->SetRotation(90, 0);


	//Paddles
	m_pPaddle1 = AddChild(new CubePrefab(XMFLOAT3{ 1.f,1.f,5.f }, XMFLOAT4{ Colors::HotPink }));
	auto pPaddle1Actor = m_pPaddle1->AddComponent(new RigidBodyComponent());
	pPaddle1Actor->SetConstraint(RigidBodyConstraint::TransY, false);
	pPaddle1Actor->AddCollider(PxBoxGeometry{ .5f,.5f,2.5f }, *pBouncyMaterial);
	pPaddle1Actor->SetKinematic(true);
	m_pPaddle1->GetTransform()->Translate(-20.f, 0.f, -80.f);

	m_pPaddle2 = AddChild(new CubePrefab(XMFLOAT3{ 1.f,1.f,5.f }, XMFLOAT4{ Colors::PaleGoldenrod }));
	auto pPaddle2Actor = m_pPaddle2->AddComponent(new RigidBodyComponent());
	pPaddle2Actor->SetConstraint(RigidBodyConstraint::TransY, false);
	pPaddle2Actor->AddCollider(PxBoxGeometry{ .5f,.5f,2.5f }, *pBouncyMaterial);
	pPaddle2Actor->SetKinematic(true);
	m_pPaddle2->GetTransform()->Translate(20.f, 0.f, -80.f);
	
	//topbot
	m_pStaticBot = AddChild(new CubePrefab(XMFLOAT3{ 80.f,1.f,1.f }, XMFLOAT4{ Colors::HotPink }));
	auto pTerrainActor = m_pStaticBot->AddComponent(new RigidBodyComponent(true));
	pTerrainActor->AddCollider(PxBoxGeometry{ 80.f,1.f,1.f }, *pBouncyMaterial);
	m_pStaticBot->GetTransform()->Translate(-0.f, 0.f, -100.f);

	m_pStaticTop = AddChild(new CubePrefab(XMFLOAT3{ 80.f,1.f,1.f }, XMFLOAT4{ Colors::HotPink }));
	pTerrainActor = m_pStaticTop->AddComponent(new RigidBodyComponent(true));
	pTerrainActor->AddCollider(PxBoxGeometry{ 80.f,1.f,1.f }, *pBouncyMaterial);
	m_pStaticTop->GetTransform()->Translate(-0.f, 0.f, -60.f);
	pTerrainActor->GetTransform()->Translate(-0.f, 0.f, -60.f);

	InputAction inputAction1Down(InputID::MoveDown1, InputState::down, VK_DOWN, -1, XINPUT_GAMEPAD_DPAD_LEFT);
	InputAction inputAction1Up(InputID::MoveUp1, InputState::down, VK_UP, -1, XINPUT_GAMEPAD_DPAD_LEFT);

	InputAction inputAction2Down(InputID::MoveDown2, InputState::down, VK_LEFT, -1, XINPUT_GAMEPAD_DPAD_LEFT); //w key
	InputAction inputAction2Up(InputID::MoveUp2, InputState::down, VK_RIGHT, -1, XINPUT_GAMEPAD_DPAD_LEFT);

	m_SceneContext.pInput->AddInputAction(inputAction1Down);
	m_SceneContext.pInput->AddInputAction(inputAction1Up);
	m_SceneContext.pInput->AddInputAction(inputAction2Down);
	m_SceneContext.pInput->AddInputAction(inputAction2Up);



	///TriggerLeft
	m_pTriggerLeft = AddChild(new CubePrefab({ 5.f, 1.f, 50.f }, XMFLOAT4{ Colors::Black }));
	auto pTriggerActor = m_pTriggerLeft->AddComponent(new RigidBodyComponent(true));
	pTriggerActor->AddCollider(PxBoxGeometry{ 2.5f, 0.5f, 50.f }, *pBouncyMaterial, true);
	m_pTriggerLeft->GetTransform()->Translate(-35.f, 0.f, -80.f);

	///TriggerRight
	m_pTriggerRight = AddChild(new CubePrefab({ 5.f, 1.f, 50.f }, XMFLOAT4{ Colors::Black }));
	pTriggerActor = m_pTriggerRight->AddComponent(new RigidBodyComponent(true));
	pTriggerActor->AddCollider(PxBoxGeometry{ 2.5f, 1.f, 50.f }, *pBouncyMaterial, true);
	m_pTriggerRight->GetTransform()->Translate(35.f, 0.f, -80.f);



	m_pTriggerLeft->SetOnTriggerCallBack(std::bind(&PongScene::OnTriggerCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	m_pTriggerRight->SetOnTriggerCallBack(std::bind(&PongScene::OnTriggerCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}


void PongScene::Update()
{

	if (m_SceneContext.pInput->IsActionTriggered(InputID::MoveDown1)) {
		//left paddle
		auto paddlePos = m_pPaddle1->GetTransform()->GetPosition();
		m_pPaddle1->GetTransform()->Translate(paddlePos.x, 0.f, paddlePos.z + (-1.f * m_PaddleSpeed));
	}
	else if (m_SceneContext.pInput->IsActionTriggered(InputID::MoveUp1)) {
		auto paddlePos = m_pPaddle1->GetTransform()->GetPosition();

		m_pPaddle1->GetTransform()->Translate(paddlePos.x, 0.f, paddlePos.z + (1.f * m_PaddleSpeed));
	}
	else if (m_SceneContext.pInput->IsActionTriggered(InputID::MoveDown2)) {
		//right paddle
		auto paddlePos = m_pPaddle2->GetTransform()->GetPosition();
		m_pPaddle2->GetTransform()->Translate(paddlePos.x, 0.f, paddlePos.z + (-1.f * m_PaddleSpeed));
	}
	else if (m_SceneContext.pInput->IsActionTriggered(InputID::MoveUp2)) {
		auto paddlePos = m_pPaddle2->GetTransform()->GetPosition();
		m_pPaddle2->GetTransform()->Translate(paddlePos.x, 0.f, paddlePos.z + (1.f * m_PaddleSpeed));
	}
}

void PongScene::OnTriggerCallback(GameObject*, GameObject*, PxTriggerAction)
{

	m_pSphere->GetTransform()->Translate(0.f, 0.f, -80.f);
	int a = rand() % 2;
	XMFLOAT3 ballDirection;
	ballDirection.z =  a == 0 ? m_BallSpeed : -m_BallSpeed;
	ballDirection.x = a == 0 ? m_BallSpeed : -m_BallSpeed;

	m_pSphere->GetComponent<RigidBodyComponent>()->AddForce(ballDirection, PxForceMode::eIMPULSE);


}
