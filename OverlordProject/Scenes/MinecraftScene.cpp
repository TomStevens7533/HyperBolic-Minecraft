#include "stdafx.h"
#include "MinecraftScene.h"
#include "Prefabs/ChunkPrefab.h"
#include "Prefabs/ChunkCharacter.h"


MinecraftScene::MinecraftScene() :
	GameScene(L"MinionScene") {}

void MinecraftScene::Initialize()
{
	const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);

	m_SceneContext.settings.drawGrid = false;
	m_ChunkTest = AddChild(new ChunkManager());

	CharacterChunkDesc characterDesc{ pDefaultMaterial };
	characterDesc.actionId_MoveForward = CharacterMoveForward;
	characterDesc.actionId_MoveBackward = CharacterMoveBackward;
	characterDesc.actionId_MoveLeft = CharacterMoveLeft;
	characterDesc.actionId_MoveRight = CharacterMoveRight;
	characterDesc.actionId_Jump = CharacterJump;
	characterDesc.actionId_Crouch = CharacterCrouch;

	m_pCharacter = AddChild(new CharacterChunk(characterDesc));
	m_pCharacter->GetTransform()->Translate(0.f, 100.f, 0.f);



	//m_SceneContext.pInput->CursorVisible(true);
	m_SceneContext.pInput->ForceMouseToCenter(true);
	//Input
	auto inputAction = InputAction(CharacterMoveLeft, InputState::down, 'A');
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterMoveRight, InputState::down, 'D');
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterMoveForward, InputState::down, 'W');
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterMoveBackward, InputState::down, 'S');
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterCrouch, InputState::down, VK_SHIFT);
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterJump, InputState::down, VK_SPACE);
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(PlaceBlock, InputState::down, -1, VK_LBUTTON);
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(RemoveBlock, InputState::pressed, -1, VK_RBUTTON);
	m_SceneContext.pInput->AddInputAction(inputAction);

}

void MinecraftScene::Update()
{
	//Optional
	m_ChunkTest->SetNewOriginPos(m_pCharacter->GetTransform()->GetPosition());

	if (m_SceneContext.pInput->IsActionTriggered(InputIds::RemoveBlock)) {
		
		//Get world dir from center of screen
		auto pair = m_pCharacter->ScreenSpaceToWorldPosAndDir(m_SceneContext, XMFLOAT2{0.5f, 0.5f});
		for (size_t i = 1; i < m_HitDistance; i++)
		{
			XMFLOAT3 newPos;
			XMVECTOR fullPos = XMLoadFloat3(&pair.first);
			XMVECTOR fullDir = (XMLoadFloat3(&pair.second) * static_cast<float>(i));
			fullDir += fullPos;
			XMStoreFloat3(&newPos, fullDir);

			if (m_ChunkTest->RemoveBlock(newPos))
				break;
		}
	}
	/*else if (m_SceneContext.pInput->IsActionTriggered(InputIds::RemoveBlock)){
		m_ChunkTest->Addblock(XMFLOAT3{0,0,0});

	}*/
}

void MinecraftScene::Draw()
{
	//Optional

}

void MinecraftScene::OnGUI()
{
	ImGui::Text("This only activates if\n SceneSettings.enableOnGUI is True.\n\n");
	ImGui::Text("Use ImGui to add custom\n controllable scene parameters!");
	ImGui::ColorEdit3("Demo ClearColor", &m_SceneContext.settings.clearColor.x, ImGuiColorEditFlags_NoInputs);
}
