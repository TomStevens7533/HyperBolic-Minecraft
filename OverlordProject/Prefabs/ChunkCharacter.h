#pragma once
struct CharacterChunkDesc
{
	CharacterChunkDesc(PxMaterial* pMaterial,
		float radius = .5f,
		float height = 2.f)
	{
		controller.setToDefault();
		controller.radius = radius;
		controller.height = height;
		controller.material = pMaterial;
	}

	float maxMoveSpeed{ 15.f };
	float maxFallSpeed{ 15.f };

	float JumpSpeed{ 10.f };

	float moveAccelerationTime{ .3f };
	float fallAccelerationTime{ .3f };

	PxCapsuleControllerDesc controller{};

	float rotationSpeed{ 60.f };

	int actionId_MoveLeft{ -1 };
	int actionId_MoveRight{ -1 };
	int actionId_MoveForward{ -1 };
	int actionId_MoveBackward{ -1 };
	int actionId_Jump{ -1 };
	int actionId_Crouch{ -1 };
};

class ChunkManager;
class CharacterChunk : public GameObject
{
public:
	CharacterChunk(const CharacterChunkDesc& characterDesc, const ChunkManager * pManager);
	~CharacterChunk() override = default;

	CharacterChunk(const CharacterChunk& other) = delete;
	CharacterChunk(CharacterChunk&& other) noexcept = delete;
	CharacterChunk& operator=(const CharacterChunk& other) = delete;
	CharacterChunk& operator=(CharacterChunk&& other) noexcept = delete;

	void DrawImGui();
	std::pair<XMFLOAT3, XMFLOAT3> ScreenSpaceToWorldPosAndDir(const SceneContext& sceneContext, XMFLOAT2 pointScale);

protected:
	void Initialize(const SceneContext&) override;
	void Update(const SceneContext&) override;

private:
	CameraComponent* m_pCameraComponent{};
	ControllerComponent* m_pControllerComponent{};

	CharacterChunkDesc m_CharacterDesc;
	float m_TotalPitch{}, m_TotalYaw{};				//Total camera Pitch(X) and Yaw(Y) rotation
	float m_MoveAcceleration{},						//Acceleration required to reach maxMoveVelocity after 1 second (maxMoveVelocity / moveAccelerationTime)
		m_FallAcceleration{},						//Acceleration required to reach maxFallVelocity after 1 second (maxFallVelocity / fallAccelerationTime)
		m_MoveSpeed{};								//MoveSpeed > Horizontal Velocity = MoveDirection * MoveVelocity (= TotalVelocity.xz)

	const ChunkManager* m_pChunkManager;
	XMFLOAT3 m_TotalVelocity{};						//TotalVelocity with X/Z for Horizontal Movement AND Y for Vertical Movement (fall/jump)
	XMFLOAT3 m_CurrentDirection{};					//Current/Last Direction based on Camera forward/right (Stored for deacceleration)
	bool m_IsCreative = true;
	float m_sensitivity = 5.f;
	float m_RayCastDistance = 0.1f;
};