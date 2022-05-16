#pragma once
#include "Content/BlockJsonParser.h"
class ChunkManager;
class CharacterChunk;
class RenderTarget;
class PostBlur;
class PostGrayscale;
class ParticleEmitterComponent;
class CameraComponent;


class MinecraftScene final : public GameScene
{
public:
	MinecraftScene();
	~MinecraftScene() override = default;

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
		CharacterJump,
		CharacterCrouch,
		PlaceBlock,
		RemoveBlock,
		Pauze,
		ScrollInv
	};

	//Pauze
	GameObject* m_pButtonUI;
	RenderTarget* m_Depth;
	PostBlur* m_pPost;
	PostGrayscale* m_pPostGrey;
	ParticleEmitterComponent* m_pEmitter;
	CameraComponent* m_pController;
	SpriteFont* m_pFont{};

	std::map<uint8_t, int> m_InventoryMap;
	int m_SelectedIdx{0};

	GameObject* m_pBackGround;
	bool m_IsPauzed = false;
};



