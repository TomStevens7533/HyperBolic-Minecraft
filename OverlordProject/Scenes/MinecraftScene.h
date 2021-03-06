#pragma once
#include "Content/BlockJsonParser.h"
class ChunkManager;
class CharacterChunk;
class RenderTarget;
class GlowGenerator;
class GlowApply;
class PostDepth;

class ParticleEmitterComponent;
class CameraComponent;


class MinecraftScene final : public GameScene
{
public:
	MinecraftScene();
	~MinecraftScene() override;

	MinecraftScene(const MinecraftScene& other) = delete;
	MinecraftScene(MinecraftScene&& other) noexcept = delete;
	MinecraftScene& operator=(const MinecraftScene& other) = delete;
	MinecraftScene& operator=(MinecraftScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override{};

	void PostDraw() override;
	void OnGUI() override;

	void OnSceneDeactivated() override;
private:
	void AddBlock();
	void DestroyBlock();
private:
	ChunkManager* m_ChunkTest;
	CharacterChunk* m_pCharacter;


	int m_HitDistance{ 15 };
	GameObject* m_pCrosshair;
	enum InputIds
	{
		CharacterMoveLeft,
		CharacterMoveRight,
		CharacterMoveForward,
		CharacterMoveBackward,
		CharacterCrouch,
		CharacterJump,
		PlaceBlock,
		RemoveBlock,
		CreativeMode,
		Pauze,
		ScrollInv
	};

	//Pauze
	GameObject* m_pButtonUI;
	RenderTarget* m_Depth;
	GlowGenerator* m_pGlowGenerator;
	GlowApply* m_pGlowApply;
	PostDepth* m_pGlowDepth;

	ParticleEmitterComponent* m_pEmitter = nullptr;

	GameObject* m_pEmitterGo = nullptr;
	ParticleEmitterSettings m_Settings{};


	CameraComponent* m_pController;
	SpriteFont* m_pFont{};

	std::map<uint8_t, int> m_InventoryMap;
	int m_SelectedIdx{0};
	ParticleEmitterSettings settings{};


	GameObject* m_pBackGround;
	bool m_IsPauzed = false;

	//Sound
	FMOD::Channel* m_pMusicSoundChannel{};
	FMOD::Sound* m_pSwededMusic;

	FMOD::Channel* m_pFXBreakChannel{};
	FMOD::Sound* m_pFXBreakMusic;
	FMOD::Channel* m_pFXPlaceChannel{};

	FMOD::Sound* m_pFXPlaceMusic;
	std::pair<int, int>m_previousChunkPos{};

};



