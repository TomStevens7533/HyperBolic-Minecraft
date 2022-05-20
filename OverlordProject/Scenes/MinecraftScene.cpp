#include "stdafx.h"
#include "MinecraftScene.h"
#include "Prefabs/ChunkPrefab.h"
#include "Prefabs/ChunkCharacter.h"
#include "Materials/Post/PostBlur.h"
#include "Materials/Post/PostGrayscale.h"
#include <xutility>


MinecraftScene::MinecraftScene() :
	GameScene(L"MinionScene") {
	m_SceneContext.settings.drawPhysXDebug = false;
	m_SceneContext.settings.drawUserDebug = false;

}

MinecraftScene::~MinecraftScene()
{
	if (m_IsPauzed == false) {
		delete m_pBackGround;
		delete m_pButtonUI;
	}
	delete m_Depth;
}

void MinecraftScene::Initialize()
{
	const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.f, 0.f, 0.5f);
	//settings
	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.settings.enableOnGUI = false;
	m_SceneContext.settings.showInfoOverlay = false;


	m_ChunkTest = AddChild(new ChunkManager());

	CharacterChunkDesc characterDesc{ pDefaultMaterial };
	characterDesc.actionId_MoveForward = CharacterMoveForward;
	characterDesc.actionId_MoveBackward = CharacterMoveBackward;
	characterDesc.actionId_MoveLeft = CharacterMoveLeft;
	characterDesc.actionId_MoveRight = CharacterMoveRight;
	characterDesc.actionId_Jump = CharacterJump;
	characterDesc.actionId_Crouch = CharacterCrouch;

	m_pCharacter = AddChild(new CharacterChunk(characterDesc,m_ChunkTest));
	m_pCharacter->GetTransform()->Translate(8.25f, 200.f, 0.f);
	std::srand(static_cast<unsigned int>(std::time(nullptr)));

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

	inputAction = InputAction(CharacterCrouch, InputState::down, 'Z');
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterJump, InputState::down, VK_SPACE);
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(PlaceBlock, InputState::pressed, -1, VK_LBUTTON);
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(RemoveBlock, InputState::pressed, -1, VK_RBUTTON);
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(ScrollInv, InputState::pressed, -1, VK_MBUTTON);
	m_SceneContext.pInput->AddInputAction(inputAction);


	inputAction = InputAction(Pauze, InputState::pressed, -1, VK_ESCAPE);
	m_SceneContext.pInput->AddInputAction(inputAction);
	//Crosshair
	m_pCrosshair = AddChild(new GameObject());
	m_pCrosshair->AddComponent(new SpriteComponent(L"Textures/crosshair.png", { -25.f,-18.5f }, { 1.f,1.f,1.f,1.f }));
	 
	m_pCrosshair->GetTransform()->Scale(0.5f, 0.5f, 1.f);

	//Load pauze ui
	m_pBackGround = new GameObject();
	m_pButtonUI = new GameObject();
	m_pBackGround->AddComponent(new SpriteComponent(L"Textures/PauzeMenu.png"
		, { 0.f, 0.f }, { 1.f,1.f,1.f,1.f }));
	m_pButtonUI->AddComponent(new SpriteComponent(L"Textures/PauzeMenuButtons.png"
		, { 0.f, 0.f }, { 1.f,1.f,1.f,1.f }));

	//light
	m_SceneContext.pLights->SetDirectionalLight({ 0.f,130.f,0.f }, { 0.740129888f, -0.7f, 0.309117377f });
	m_SceneContext.pLights->GetDirectionalLight().intensity = 20.f;
	RENDERTARGET_DESC desc;
	desc.enableColorBuffer = false;
	desc.enableDepthBuffer = true;
	desc.enableDepthSRV = true;
	desc.width = static_cast<UINT>(m_SceneContext.windowWidth);
	desc.height = static_cast<int>(m_SceneContext.windowHeight);
	m_Depth = new RenderTarget(m_SceneContext.d3dContext);
	HANDLE_ERROR(m_Depth->Create(desc));

	//post
	m_pPost = MaterialManager::Get()->CreateMaterial<PostBlur>();
	m_pPostGrey = MaterialManager::Get()->CreateMaterial<PostGrayscale>();


	AddGlowPass(m_pPost);
	AddGlowPass(m_pPostGrey);

	//Particles
	ParticleEmitterSettings settings{};
	settings.velocity = { 0.f,6.f,0.f };
	settings.minSize = 1.f;
	settings.maxSize = 2.f;
	settings.minEnergy = 1.f;
	settings.maxEnergy = 2.f;
	settings.minScale =1.5f;
	settings.maxScale = 2.5f;
	settings.minEmitterRadius = .2f;
	settings.maxEmitterRadius = .5f;
	settings.color = { 1.f,1.f,1.f, .6f };

	m_pEmitter = AddChild(new GameObject())->AddComponent(new ParticleEmitterComponent(L"Textures/Smoke.png", settings, 200));
	m_pFont = ContentManager::Load<SpriteFont>(L"SpriteFonts/Consolas_32.fnt");

	//Add Planks to inv
	m_InventoryMap[5] = 1500;

}

void MinecraftScene::Update()
{
	//Optional
	if (!m_IsPauzed) {
		m_ChunkTest->SetNewOriginPos(m_pCharacter->GetTransform()->GetPosition());
		XMFLOAT4 lightPos = XMFLOAT4{ m_pCharacter->GetTransform()->GetWorldPosition().x - 200.f  , 220.f , m_pCharacter->GetTransform()->GetWorldPosition().z - 75 , 0.f };
		m_SceneContext.pLights->GetDirectionalLight().position = lightPos;
		if (m_SceneContext.pInput->IsActionTriggered(InputIds::RemoveBlock) ) {


			//Get world dir from center of screen
			auto pair = m_pCharacter->ScreenSpaceToWorldPosAndDir(m_SceneContext, XMFLOAT2{ 0.5f, 0.5f });
			for (size_t i = 1; i < m_HitDistance; i++)
			{
				XMFLOAT3 newPos;
				XMVECTOR fullPos = XMLoadFloat3(&pair.first);
				XMVECTOR fullDir = (XMLoadFloat3(&pair.second) * static_cast<float>(i));
				fullDir += fullPos;
				XMStoreFloat3(&newPos, fullDir);

				std::tuple<int, int, int> blockPos;
				uint8_t id = m_ChunkTest->RemoveBlock(newPos, blockPos);
				if (id != 0) {
					auto particlePosition = m_pEmitter->GetTransform()->GetPosition();
					particlePosition = XMFLOAT3((float)std::get<0>(blockPos), (float)std::get<1>(blockPos), (float)std::get<2>(blockPos));
					m_pCharacter->PlayAnimatation();
					m_pEmitter->GetTransform()->Translate(particlePosition);
					m_InventoryMap[id]++;
					break;

				}
			}
		}
		if (m_SceneContext.pInput->IsKeyboardKey(InputState::pressed, VK_SHIFT) ) {

			//Get world dir from center of screen
			auto pair = m_pCharacter->ScreenSpaceToWorldPosAndDir(m_SceneContext, XMFLOAT2{ 0.5f, 0.5f });
			for (size_t i = m_HitDistance - 1; i >= 0; i--)
			{
				XMFLOAT3 newPos;
				XMVECTOR fullPos = XMLoadFloat3(&pair.first);
				XMVECTOR fullDir = (XMLoadFloat3(&pair.second) * static_cast<float>(i));
				fullDir += fullPos;
				XMStoreFloat3(&newPos, fullDir);
				m_pCharacter->PlayAnimatation();

				auto it = m_InventoryMap.begin();
				std::advance(it, m_SelectedIdx);
				if (it->second > 0 && m_ChunkTest->Addblock(newPos, it->first))
				{
					it->second--;

					//Remove from inv if 0
					if (it->second <= 0) {
						m_InventoryMap.erase(it);
						m_SelectedIdx = (m_SelectedIdx) % m_InventoryMap.size();
					}
					break;
				}
			}

		}

		if (m_SceneContext.pInput->IsActionTriggered(InputIds::ScrollInv)) {

			m_SelectedIdx = (m_SelectedIdx + 1) % m_InventoryMap.size();
		}
		//Inventory
		XMFLOAT2 InvPos{ 0.f, 0.f };

		for (auto mapIt = m_InventoryMap.begin(); mapIt != m_InventoryMap.end(); mapIt++)
		{
			uint8_t id = mapIt->first;
			int amount = mapIt->second;
			std::string name = m_ChunkTest->GetNameOfID(id);
			name += " : ";
			name += std::to_string(amount);

			int distance = static_cast<int>(std::distance(m_InventoryMap.begin(), mapIt));

			if (m_SelectedIdx == distance)
				TextRenderer::Get()->DrawText(m_pFont, StringUtil::utf8_decode(name), InvPos, DirectX::XMFLOAT4(Colors::Chartreuse));
			else
				TextRenderer::Get()->DrawText(m_pFont, StringUtil::utf8_decode(name), InvPos, DirectX::XMFLOAT4(Colors::White));

			InvPos.y += m_pFont->GetSize();
		}
	}
	if (m_SceneContext.pInput->IsKeyboardKey(InputState::pressed, 'C'))
	{
		std::cout << "Creative\n";
		m_pCharacter->SwitchCreativeMode();
	}
	//Pauze
	if (m_SceneContext.pInput->IsKeyboardKey(InputState::pressed, VK_ESCAPE))
	{
		m_IsPauzed = !m_IsPauzed;
		if (m_IsPauzed == true) {
			AddChild(m_pButtonUI);
			AddChild(m_pBackGround);
			m_pCharacter->SetDisable();
		}
		else {
			RemoveChild(m_pButtonUI);
			RemoveChild(m_pBackGround);
			m_pCharacter->SetDisable();

		}
	}

	if (m_IsPauzed && m_SceneContext.pInput->IsMouseButton(InputState::down, VK_LBUTTON)) {
		POINT mousePos = m_SceneContext.pInput->GetMousePosition();
		if (mousePos.x > 81 && mousePos.x < 354 && mousePos.y > 94 && mousePos.y < 151)
		{
			m_IsPauzed = false;
			RemoveChild(m_pButtonUI);
			RemoveChild(m_pBackGround);
			m_pCharacter->SetDisable();
		}
		else if (mousePos.x > 81 && mousePos.x < 354 && mousePos.y > 320 && mousePos.y < 471)
		{
			m_pCharacter->GetTransform()->Translate(8.25f, 200.f, 0.f);
			m_IsPauzed = false;
			RemoveChild(m_pButtonUI);
			RemoveChild(m_pBackGround);
			m_pCharacter->SetDisable();
		}
		else if (mousePos.x > 81 && mousePos.x < 354 && mousePos.y > 550 && mousePos.y < 671)
		{
			SceneManager::Get()->PreviousScene();

		}
	}
	

}

void MinecraftScene::PostDraw()
{
	//Optional

}

void MinecraftScene::OnGUI()
{
	ImGui::Text("This only activates if\n SceneSettings.enableOnGUI is True.\n\n");
	ImGui::Text("Use ImGui to add custom\n controllable scene parameters!");
	ImGui::ColorEdit3("Demo ClearColor", &m_SceneContext.settings.clearColor.x, ImGuiColorEditFlags_NoInputs);
}
