#include "stdafx.h"
#include "MainScreenScene.h"
#include "Scenegraph/GameObject.h"
#include "Components/SpriteComponent.h"
#include "Scenes/MinecraftScene.h"

MainScreenScene::MainScreenScene() : GameScene(L"MainScreen")
{
	m_SceneContext.settings.drawGrid = false;
}

void MainScreenScene::Initialize()
{
	m_pUI = AddChild(new GameObject());
	m_pUI->AddComponent(new SpriteComponent(L"Textures/MainMenu.png"
		, { 0.f, 0.f }, { 1.f,1.f,1.f,1.f }));


	auto inputAction = InputAction(Enter, InputState::pressed, -1, VK_LBUTTON);
	m_SceneContext.pInput->AddInputAction(inputAction);
}

void MainScreenScene::Update()
{
	if (m_SceneContext.pInput->IsActionTriggered(InputIds::Enter)) {
		POINT mousePos = m_SceneContext.pInput->GetMousePosition();

		if (mousePos.x > 550 && mousePos.x < 820 && mousePos.y > 347 && mousePos.y < 455)
		{
			//Play game pressed
			std::cout << "Play" << std::endl;
			m_gameScene = new MinecraftScene();
			SceneManager::Get()->AddGameScene(m_gameScene);
			SceneManager::Get()->NextScene();
		}
		if (mousePos.x > 550 && mousePos.x < 820 && mousePos.y > 479 && mousePos.y < 587)
		{
			//Play game pressed
			std::cout << "Quit" << std::endl;
			
		}

	}
}

void MainScreenScene::OnSceneActivated()
{
	//Remove minecraft scene
	SceneManager::Get()->RemoveGameScene(m_gameScene, true);
}

void MainScreenScene::OnSceneDeactivated()
{
	std::cout << "deactive\n";
}
