#include "stdafx.h"
#include "HardwareSkinningScene.h"

#include "Materials/DiffuseMaterial_Skinned.h"

HardwareSkinningScene::~HardwareSkinningScene()
{
	for (size_t i = 0; i < m_ClipCount; i++)
	{
		delete[] m_ClipNames[i];
	}
	delete[] m_ClipNames;

}

void HardwareSkinningScene::Initialize()
{
	m_SceneContext.settings.enableOnGUI = true;

	const auto pSkinnedMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Skinned>();
	pSkinnedMaterial->SetDiffuseTexture(L"Spyro/mesh/luigiMixamo/luigi_texture_albedo.png");

	auto pObject = AddChild(new GameObject);
	auto pModel = pObject->AddComponent(new ModelComponent(L"Spyro/mesh/luigi.ovm"));
	pModel->SetMaterial(pSkinnedMaterial);




	pAnimator = pModel->GetAnimator();
	pAnimator->SetAnimation(m_AnimationClipId);
	pAnimator->SetAnimationSpeed(m_AnimationSpeed);

	//Gather Clip Names
	m_ClipCount = pAnimator->GetClipCount();
	m_ClipNames = new char* [m_ClipCount];
	for (auto i{ 0 }; i < m_ClipCount; ++i)
	{
		std::string name = StringUtil::utf8_encode(pAnimator->GetClipNameByID(i));
		m_ClipNames[i] = new char[name.size() + 1];
		strcpy_s(m_ClipNames[i], name.size() + 1, name.c_str());
	}
}

void HardwareSkinningScene::OnGUI()
{
	if (ImGui::Button(pAnimator->IsPlaying() ? "PAUSE" : "PLAY"))
	{
		if (pAnimator->IsPlaying())pAnimator->Pause();
		else pAnimator->Play();
	}

	if (ImGui::Button("RESET"))
	{
		pAnimator->Reset();
	}

	ImGui::Dummy({ 0,5 });

	bool reversed = pAnimator->IsReversed();
	if (ImGui::Checkbox("Play Reversed", &reversed))
	{
		pAnimator->SetPlayReversed(reversed);
	}

	if (ImGui::ListBox("Animation Clip", &m_AnimationClipId, m_ClipNames, m_ClipCount))
	{
		pAnimator->SetAnimation(m_AnimationClipId);
	}

	if (ImGui::SliderFloat("Animation Speed", &m_AnimationSpeed, 0.f, 4.f))
	{
		pAnimator->SetAnimationSpeed(m_AnimationSpeed);
	}
}