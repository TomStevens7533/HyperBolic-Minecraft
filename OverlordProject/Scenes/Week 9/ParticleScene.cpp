#include "stdafx.h"
#include "ParticleScene.h"

#include "Materials/ColorMaterial.h"

void ParticleScene::Initialize()
{
	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.settings.enableOnGUI = true;

	//Particle System
	ParticleEmitterSettings settings{};
	settings.velocity = { 0.f,6.f,0.f };
	settings.minSize = 1.f;
	settings.maxSize = 2.f;
	settings.minEnergy = 1.f;
	settings.maxEnergy = 2.f;
	settings.minScale = 3.5f;
	settings.maxScale = 5.5f;
	settings.minEmitterRadius = .2f;
	settings.maxEmitterRadius = .5f;
	settings.color = { 1.f,1.f,1.f, .6f };

	const auto pObject = AddChild(new GameObject);
	m_pEmitter = pObject->AddComponent(new ParticleEmitterComponent(L"Textures/Smoke.png", settings, 200));

	//Teapot

}

void ParticleScene::Update()
{
	//Rotate Teapot

	//Move Particle System
	auto particlePosition = m_pEmitter->GetTransform()->GetPosition();
	const auto moveSpeed = 10.f * m_SceneContext.pGameTime->GetElapsed();

	if(m_AutoMove)
		particlePosition.x += moveSpeed * sin(m_SceneContext.pGameTime->GetTotal());

	if(InputManager::IsKeyboardKey(InputState::down, VK_RIGHT))
	{
		particlePosition.x += moveSpeed;
	}
	if (InputManager::IsKeyboardKey(InputState::down, VK_LEFT))
	{
		particlePosition.x -= moveSpeed;
	}
	if (InputManager::IsKeyboardKey(InputState::down, VK_UP))
	{
		particlePosition.z += moveSpeed;
	}
	if (InputManager::IsKeyboardKey(InputState::down, VK_DOWN))
	{
		particlePosition.z -= moveSpeed;
	}

	m_pEmitter->GetTransform()->Translate(particlePosition);
}

void ParticleScene::OnGUI()
{
	ImGui::Checkbox("Auto Move", &m_AutoMove);
	m_pEmitter->DrawImGui();
}
