#include "stdafx.h"
#include "MinecraftScene.h"
#include "Prefabs/ChunkPrefab.h"
#include "Prefabs/ChunkCharacter.h"
#include <xutility>
#include "Materials/Post/PostGlowGenerator.h"
#include "Materials/Post/PostGlowApply.h"
#include "Materials/Post/DepthMaterial.h"
#include "Materials/SkyboxMaterial.h"


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
	InputManager::ForceMouseToCenter(true);
	InputManager::CursorVisible(false);

	m_ChunkTest = AddChild(new ChunkManager());

	//Skybox
	GameObject* skyBoxGo = AddChild(new GameObject());

	ModelComponent* skyboxModel = skyBoxGo->AddComponent(new ModelComponent(L"Meshes/Cube.ovm"));
	auto SkyBoxmat = MaterialManager::Get()->CreateMaterial<SkyboxMaterial>();
	skyboxModel->SetMaterial(SkyBoxmat);

	//Char creation

	CharacterChunkDesc characterDesc{ pDefaultMaterial, 0.15f, 2.f};
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
	m_pGlowGenerator = MaterialManager::Get()->CreateMaterial<GlowGenerator>();
	m_pGlowApply = MaterialManager::Get()->CreateMaterial<GlowApply>();
	m_pGlowDepth = MaterialManager::Get()->CreateMaterial<PostDepth>();

	AddGlowPass(m_pGlowGenerator);
	AddGlowPass(m_pGlowApply);
	//AddGlowPass(m_pGlowDepth);

	//Particles
	settings.velocity = { 0.f,1.f,0.f };
	settings.minSize = 0.3f;
	settings.maxSize = 0.8f;
	settings.minEnergy = 1.f;
	settings.maxEnergy = 1.2f;
	settings.minScale =.2f;
	settings.maxScale = 0.5f;
	settings.minEmitterRadius = .01f;
	settings.maxEmitterRadius = .05f;
	settings.color = {0.429f,0.148f,0.05f, .3f }; //burnt umber color

	m_pEmitterGo = AddChild(new GameObject());
	m_pEmitter = m_pEmitterGo->AddComponent(new ParticleEmitterComponent(L"Textures/BlockBreak.png", settings, 80));
	m_pFont = ContentManager::Load<SpriteFont>(L"SpriteFonts/Consolas_32.fnt");
	m_pEmitter->Stop();
	m_pEmitter->SetPlayOnce();
	//Add Planks to inv
	m_InventoryMap[5] = 1500;


	//sound
	//========
	auto pFmodSystem = SoundManager::Get()->GetSystem();

	auto fmodResult = pFmodSystem->createStream("Resources/sweden.mp3", FMOD_DEFAULT, nullptr, &m_pSwededMusic);
	HANDLE_ERROR(fmodResult);

	fmodResult = pFmodSystem->createStream("Resources/blockbreak.mp3", FMOD_DEFAULT, nullptr, &m_pFXBreakMusic);
	HANDLE_ERROR(fmodResult);
	fmodResult = pFmodSystem->createStream("Resources/blockplace.mp3", FMOD_DEFAULT, nullptr, &m_pFXPlaceMusic);
	HANDLE_ERROR(fmodResult);

	SoundManager::Get()->GetSystem()->playSound(m_pSwededMusic, nullptr, false, &m_pMusicSoundChannel);
	m_pMusicSoundChannel->setVolume(0.6f);


;

}

void MinecraftScene::Update()
{
	//Optional
	if (!m_IsPauzed) {

		InputManager::ForceMouseToCenter(true);

		std::pair<int, int> newChunksPos = m_ChunkTest->GetChunkIdx(m_pCharacter->GetFootPos());
		if (m_previousChunkPos != newChunksPos) {
			XMFLOAT4 lightPos = XMFLOAT4{ m_pCharacter->GetTransform()->GetWorldPosition().x - 200.f  , 300.f , m_pCharacter->GetTransform()->GetWorldPosition().z - 75 , 0.f };
			m_SceneContext.pLights->GetDirectionalLight().position = lightPos;
		}
		m_previousChunkPos = newChunksPos;
		m_ChunkTest->SetNewOriginPos(m_pCharacter->GetTransform()->GetPosition());

		if (m_SceneContext.pInput->IsActionTriggered(InputIds::RemoveBlock) ) {


			DestroyBlock();
		}
		if (m_SceneContext.pInput->IsActionTriggered(InputIds::PlaceBlock)) {

			//Get world dir from center of screen
			AddBlock();

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
			InputManager::CursorVisible(true);
		}
		else {
			RemoveChild(m_pButtonUI);
			RemoveChild(m_pBackGround);
			m_pCharacter->SetDisable();
			InputManager::CursorVisible(false);


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
			InputManager::CursorVisible(false);

		}
		else if (mousePos.x > 81 && mousePos.x < 354 && mousePos.y > 320 && mousePos.y < 471)
		{
			m_pCharacter->GetTransform()->Translate(8.25f, 200.f, 0.f);
			m_IsPauzed = false;
			RemoveChild(m_pButtonUI);
			RemoveChild(m_pBackGround);
			m_pCharacter->SetDisable();
			InputManager::CursorVisible(false);

		}
		else if (mousePos.x > 81 && mousePos.x < 354 && mousePos.y > 550 && mousePos.y < 671)
		{
			SceneManager::Get()->PreviousScene();
			m_pMusicSoundChannel->stop();


		}
	}
	
	//See if music has stopped playing
	bool isPlaying;
	m_pMusicSoundChannel->isPlaying(&isPlaying);
	if (isPlaying == false) {
		SoundManager::Get()->GetSystem()->playSound(m_pSwededMusic, nullptr, false, &m_pMusicSoundChannel);
		m_pMusicSoundChannel->setVolume(1.f);
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

void MinecraftScene::OnSceneDeactivated()
{

}

void MinecraftScene::AddBlock()
{
	auto pair = m_pCharacter->ScreenSpaceToWorldPosAndDir(m_SceneContext, XMFLOAT2{ 0.5f, 0.5f });
	//Find block to place on
	for (float i = 0; i < (float)m_HitDistance; i += 0.1f)
	{
		XMFLOAT3 newPos;
		XMVECTOR fullPos = XMVECTOR{ pair.first.x, pair.first.y, pair.first.z };
		XMVECTOR fullDir = (XMLoadFloat3(&pair.second) * static_cast<float>(i));
		fullDir += fullPos;
		XMStoreFloat3(&newPos, fullDir);
		m_pCharacter->PlayAnimatation();

		auto it = m_InventoryMap.begin();
		std::advance(it, m_SelectedIdx);
		XMFLOAT3 chunkPos{};

	
		if (it->second > 0 && (m_ChunkTest->IsBlockInChunkSolid(newPos) == true))
		{
			XMFLOAT3 normal{0,0,0};
			XMFLOAT3 centerToCamera;


			chunkPos.x = (m_ChunkTest->WorldToChunkIndex((newPos)).first + m_ChunkTest->WorldToLocalChunkPos(newPos).x + 0.5f);
			chunkPos.z = (m_ChunkTest->WorldToChunkIndex(newPos).second + m_ChunkTest->WorldToLocalChunkPos(newPos).z + 0.5f);
			chunkPos.y = (m_ChunkTest->WorldToLocalChunkPos(newPos).y + 0.5f);

			//Calculate face normal


			XMStoreFloat3(&centerToCamera, XMVector3Normalize(XMLoadFloat3(&newPos) -  XMLoadFloat3(&chunkPos)));
			

			////std::cout << (centerToCamera.x) << " " << centerToCamera.y << " " << (centerToCamera.z) << std::endl;
			//std::cout << " \n";
			//std::cout << (chunkPos.x) << " - " << chunkPos.y << " - " << (chunkPos.z) << std::endl;
			//std::cout << (newPos.x) << " - " << newPos.y << " - " << (newPos.z) << std::endl;
	

			if (std::abs(centerToCamera.y) > std::abs(centerToCamera.x) && std::abs(centerToCamera.y) > std::abs(centerToCamera.z)) {
				if (std::signbit(centerToCamera.y))
					normal.y = -1;
				else
					normal.y = 1;
			}
			else if (std::abs(centerToCamera.x) > std::abs(centerToCamera.y) && std::abs(centerToCamera.x) > std::abs(centerToCamera.z)) {
				if (std::signbit(centerToCamera.x))
					normal.x = -1;
				else
					normal.x = 1;
			}
			else if (std::abs(centerToCamera.z) > std::abs(centerToCamera.x) && std::abs(centerToCamera.z) > std::abs(centerToCamera.y)) {
				if (std::signbit(centerToCamera.z))
					normal.z = -1;
				else
					normal.z = 1;
			}

			//std::cout << normal.x << " " << normal.y << " " << normal.z << std::endl;
			//std::cout << "---------------------------------------\n";

			for (int x = 1; x < 2; x++)
			{
				fullPos = XMVECTOR{ newPos.x + (normal.x  * x), newPos.y + (normal.y * x), newPos.z + (normal.z * x) };
				//fullPos = XMVECTOR{ newPos.x , newPos.y  +  1.f , newPos.z  };

				XMStoreFloat3(&newPos, fullPos);
				if (m_ChunkTest->Addblock(newPos, it->first)) {

					it->second--;
					m_pFXPlaceChannel->stop();
					SoundManager::Get()->GetSystem()->playSound(m_pFXPlaceMusic, nullptr, false, &m_pFXPlaceChannel);

					//Remove from inv if 0
					if (it->second <= 0) {
						m_InventoryMap.erase(it);
						m_SelectedIdx = (m_SelectedIdx) % m_InventoryMap.size();
						//exit all for loops
						return;
					}
					return;

				}
				
			}
			return;
			//Place block in direction of normal;
		


		}
	}
}

void MinecraftScene::DestroyBlock()
{
	//Get world dir from center of screen
	auto pair = m_pCharacter->ScreenSpaceToWorldPosAndDir(m_SceneContext, XMFLOAT2{ 0.5f, 0.5f });
	for (size_t i = 1; i < m_HitDistance; i++)
	{
		XMFLOAT3 newPos;
		XMVECTOR fullPos = XMLoadFloat3(&pair.first);
		XMVECTOR fullDir = (XMLoadFloat3(&pair.second) * static_cast<float>(i));
		fullDir += fullPos;
		XMStoreFloat3(&newPos, fullDir);

		uint8_t id = m_ChunkTest->RemoveBlock(newPos);
		if (id != 0) {
			XMFLOAT3 pos;
			XMStoreFloat3(&pos, XMLoadFloat3(&newPos) - XMLoadFloat3(&m_pEmitterGo->GetTransform()->GetWorldPosition()));
			m_pEmitterGo->GetTransform()->Translate(pos);
			m_pEmitter->Play();

			m_pCharacter->PlayAnimatation();
			m_pFXBreakChannel->stop();
			SoundManager::Get()->GetSystem()->playSound(m_pFXBreakMusic, nullptr, false, &m_pFXBreakChannel);
			m_InventoryMap[id]++;
			break;

		}
	}
}
