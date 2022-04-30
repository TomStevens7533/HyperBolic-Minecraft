#pragma once
#include "Character.h"
class ThirdPersonCharacter : public GameObject
{
public:
	ThirdPersonCharacter(const CharacterDesc& characterDesc, float maxDistance = 40.f);
	~ThirdPersonCharacter() override = default;

	ThirdPersonCharacter(const ThirdPersonCharacter& other) = delete;
	ThirdPersonCharacter(ThirdPersonCharacter&& other) noexcept = delete;
	ThirdPersonCharacter& operator=(const ThirdPersonCharacter& other) = delete;
	ThirdPersonCharacter& operator=(ThirdPersonCharacter&& other) noexcept = delete;

	void DrawImGui();

protected:
	void Initialize(const SceneContext&) override;
	void Update(const SceneContext&) override;
	void ControlCameraBound(float& yaw, float& pitch);
private:
	CameraComponent* m_pCameraComponent{};
	ControllerComponent* m_pControllerComponent{};
	FixedCamera* m_pCamera;

	float m_MaxDistanceFromController{};
	bool m_IsFirstPerson = false;
	XMFLOAT3 oldPos;
	CharacterDesc m_CharacterDesc;
	float m_TotalPitch{}, m_TotalYaw{};				//Total camera Pitch(X) and Yaw(Y) rotation
	float m_MoveAcceleration{},						//Acceleration required to reach maxMoveVelocity after 1 second (maxMoveVelocity / moveAccelerationTime)
		m_FallAcceleration{},						//Acceleration required to reach maxFallVelocity after 1 second (maxFallVelocity / fallAccelerationTime)
		m_MoveSpeed{};								//MoveSpeed > Horizontal Velocity = MoveDirection * MoveVelocity (= TotalVelocity.xz)

	XMFLOAT3 m_TotalVelocity{};						//TotalVelocity with X/Z for Horizontal Movement AND Y for Vertical Movement (fall/jump)
	XMFLOAT3 m_CurrentDirection{};					//Current/Last Direction based on Camera forward/right (Stored for deacceleration)
};

