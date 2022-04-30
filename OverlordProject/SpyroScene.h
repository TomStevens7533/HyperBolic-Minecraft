#pragma  once

class ThirdPersonCharacter;
class SpyroScene final : public GameScene
{
public:
	SpyroScene();
	~SpyroScene() override = default;

	SpyroScene(const SpyroScene& other) = delete;
	SpyroScene(SpyroScene&& other) noexcept = delete;
	SpyroScene& operator=(const SpyroScene& other) = delete;
	SpyroScene& operator=(SpyroScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void OnGUI() override;

private:
	GameObject* m_pObject{};
	RigidBodyComponent* m_pRigidBody{};
	FreeCamera* m_pFixedCamera{};

	enum InputIds
	{
		CharacterMoveLeft,
		CharacterMoveRight,
		CharacterMoveForward,
		CharacterMoveBackward,
		CharacterJump
	};

	ThirdPersonCharacter* m_pCharacter{};

};



