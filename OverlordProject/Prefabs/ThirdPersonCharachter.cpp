#include "stdafx.h"
#include "ThirdPersonCharachter.h"

ThirdPersonCharacter::ThirdPersonCharacter(const CharacterDesc& characterDesc, float maxDistance) :
	m_CharacterDesc{ characterDesc },m_MaxDistanceFromController{maxDistance},
	m_MoveAcceleration(characterDesc.maxMoveSpeed / characterDesc.moveAccelerationTime),
	m_FallAcceleration(characterDesc.maxFallSpeed / characterDesc.fallAccelerationTime)
{}

void ThirdPersonCharacter::Initialize(const SceneContext& /*sceneContext*/)
{
	//Controller
	m_pControllerComponent = AddComponent(new ControllerComponent(m_CharacterDesc.controller));

	//Camera
	m_pCamera = AddChild(new FixedCamera());
	m_pCameraComponent = m_pCamera->GetComponent<CameraComponent>();

	m_pCameraComponent->SetActive(true); //Uncomment to make this camera the active camera

	m_pCamera->GetTransform()->Translate(0.f, m_CharacterDesc.controller.height + 5.f, -20.f);
}

void ThirdPersonCharacter::Update(const SceneContext& sceneContext)
{
	if (m_pCameraComponent->IsActive())
	{

		auto deltaTime = sceneContext.pGameTime->GetElapsed();

		//***************
		//HANDLE INPUT

		//## Input Gathering (move)
		XMFLOAT2 move{ 0,0 }; //Uncomment

		//MoveY
			if (sceneContext.pInput->IsActionTriggered(m_CharacterDesc.actionId_MoveForward)) {
				move.y = 1.f;
			}
			else if (sceneContext.pInput->IsActionTriggered(m_CharacterDesc.actionId_MoveBackward))
				move.y = -1.f;
			else
				move.y = sceneContext.pInput->GetThumbstickPosition().y;


	
		//MoveX
			if (sceneContext.pInput->IsActionTriggered(m_CharacterDesc.actionId_MoveRight))
				move.x = 1.f;
			else if (sceneContext.pInput->IsActionTriggered(m_CharacterDesc.actionId_MoveLeft))
				move.x = -1.f;
			else
				move.x = sceneContext.pInput->GetThumbstickPosition().x;



		//## Input Gathering (look)
		XMFLOAT2 look{ 0.f, 0.f }; //Uncomment
		//Only if the Left Mouse Button is Down >
		// Store the MouseMovement in the local 'look' variable (cast is required)
		//Optional: in case look.x AND look.y are near zero, you could use the Right ThumbStickPosition for look
	
			if (sceneContext.pInput->IsMouseButton(InputState::down, VK_LBUTTON)) {
				POINT newLook = sceneContext.pInput->GetMouseMovement();
				look.x = static_cast<float>(newLook.x);
				look.y = static_cast<float>(newLook.y);
				//***************
				//CAMERA ROTATION
				//Adjust the TotalYaw (m_TotalYaw) & TotalPitch (m_TotalPitch) based on the local 'look' variable
				//Make sure this calculated on a framerate independent way and uses CharacterDesc::rotationSpeed.
				//Rotate this character based on the TotalPitch (X) and TotalYaw (Y)

				float tempYaw = look.x * m_CharacterDesc.rotationSpeed * deltaTime;
				float tempPitch = look.y * m_CharacterDesc.rotationSpeed * deltaTime;

				ControlCameraBound(tempYaw, tempPitch);

				m_TotalPitch += tempPitch;
				m_TotalYaw += tempYaw;

				GetTransform()->Rotate(m_TotalPitch, m_TotalYaw, 0);

			}
			else {
				//ThumbStick
				look = sceneContext.pInput->GetThumbstickPosition(false);
				float tempYaw = look.x * m_CharacterDesc.rotationSpeed * deltaTime;
				float tempPitch = look.y * m_CharacterDesc.rotationSpeed * deltaTime;

				ControlCameraBound(tempYaw, tempPitch);

				m_TotalYaw += tempYaw;
				m_TotalPitch += tempPitch;


				GetTransform()->Rotate(m_TotalPitch, m_TotalYaw, 0);
			}
		
		

		//************************
		//GATHERING TRANSFORM INFO

		//Retrieve the TransformComponent
		//Retrieve the forward & right vector (as XMVECTOR) from the TransformComponent
		//Calculate Transforms
		XMVECTOR currPos = XMLoadFloat3(&GetTransform()->GetPosition());
		XMFLOAT3 forward = GetTransform()->GetForward();
		XMFLOAT3 right = GetTransform()->GetRight();




		//********
		//MOVEMENT

		//## Horizontal Velocity (Forward/Backward/Right/Left)
		//If the character is moving (= input is pressed)
		if (move.x != 0 || move.y != 0)
		{
			//Calculate the current move acceleration for this frame (m_MoveAcceleration * ElapsedTime)
			m_MoveSpeed += m_MoveAcceleration * deltaTime;

			if (m_MoveSpeed >= m_CharacterDesc.maxMoveSpeed)	//Make sure the current MoveSpeed stays below the maximum MoveSpeed (CharacterDesc::maxMoveSpeed)
			{

				m_MoveSpeed = m_CharacterDesc.maxMoveSpeed;

			}

			//Calculate & Store the current direction (m_CurrentDirection) >> based on the forward/right vectors and the pressed inpu
			m_CurrentDirection.x = ((forward.x * move.y) + (right.x * move.x));
			m_CurrentDirection.z = ((forward.z * move.y) + (right.z * move.x));



			//Set the x/z component of m_TotalVelocity (horizontal_velocity x/z)
			//It's important that you don't overwrite the y component of m_TotalVelocity (contains the vertical velocity)		
		}
		else
		{
			//Else (character is not moving, or stopped moving)
			//Decrease the current MoveSpeed with the current Acceleration (m_MoveSpeed)
			//Make sure the current MoveSpeed doesn't get smaller than zero

			m_MoveSpeed -= m_MoveAcceleration * deltaTime;

			if (m_MoveSpeed <= 0.f)	//Make sure the current MoveSpeed stays below the maximum MoveSpeed (CharacterDesc::maxMoveSpeed)
				m_MoveSpeed = 0.f;
		}

		//## Vertical Movement (Jump/Fall)
		if (m_pControllerComponent->GetCollisionFlags().isSet(PxControllerCollisionFlag::eCOLLISION_DOWN))//If the Controller Component is NOT grounded (= freefall)
		{
			//Else If the jump action is triggere
			if (sceneContext.pInput->IsActionTriggered(m_CharacterDesc.actionId_Jump)) {
				//Set m_TotalVelocity.y equal to CharacterDesc::JumpSpeed
				m_TotalVelocity.y = m_CharacterDesc.JumpSpeed;
			}
			else {
				//Else (=Character is grounded, no input pressed)
				//m_TotalVelocity.y is zero
				m_TotalVelocity.y = 0.f;
			}
			//m_State = State::idle;
		}
		else
			//If the Controller Component is NOT grounded (= freefall)
		{
			//Decrease the y component of m_TotalVelocity with a fraction (ElapsedTime) of the Fall Acceleration (m_FallAcceleration)
			m_TotalVelocity.y -= m_FallAcceleration * deltaTime;
			//Make sure that the minimum speed stays above -CharacterDesc::maxFallSpeed (negative!)
			if (m_TotalVelocity.y <= -m_CharacterDesc.maxFallSpeed)
			{
				m_TotalVelocity.y = -m_CharacterDesc.maxFallSpeed;
			}
		}



		//Now we can calculate the Horizontal Velocity which should be stored in m_TotalVelocity.xz
		//Calculate the horizontal velocity (m_CurrentDirection * MoveSpeed)

		m_TotalVelocity.x = (m_CurrentDirection.x * m_MoveSpeed);
		m_TotalVelocity.z = (m_CurrentDirection.z * m_MoveSpeed);
		//************
		//DISPLACEMENT

		//The displacement required to move the Character Controller (ControllerComponent::Move) can be calculated using our TotalVelocity (m/s)
		//Calculate the displacement (m) for the current frame and move the ControllerComponent
		XMFLOAT3 displacement;
		displacement.x = m_TotalVelocity.x * deltaTime;
		displacement.y = m_TotalVelocity.y * deltaTime;
		displacement.z = m_TotalVelocity.z * deltaTime;

		m_pControllerComponent->Move(displacement);
		//The above is a simple implementation of Movement Dynamics, adjust the code to further improve the movement logic and behaviour.
		//Also, it can be usefull to use a seperate RayCast to check if the character is grounded (more responsive)





	}
}

void ThirdPersonCharacter::ControlCameraBound(float& yaw, float& pitch)
{
	XMFLOAT3 rightVec = m_pCameraComponent->GetTransform()->GetRight();
	XMFLOAT3 invRighVec;
	XMStoreFloat3(&invRighVec, XMLoadFloat3(&rightVec) * -1.f);


	//Check right bound
	//Only third person
	if (m_pCameraComponent->RayCast(rightVec, 5.f) && yaw < 0.f && m_IsFirstPerson == false) {
		yaw = 0.f;
		return;
	}
	else if (m_pCameraComponent->RayCast(invRighVec, 5.f) && yaw > 0.f && m_IsFirstPerson == false) {
		yaw = 0.f;
		return;
	}

	XMFLOAT3 upVec = m_pCameraComponent->GetTransform()->GetUp();
	XMFLOAT3 invUpVec;
	XMStoreFloat3(&invUpVec, XMLoadFloat3(&upVec) * -1.f);
	//Check up bound
	if (m_pCameraComponent->RayCast(upVec, 5.f) && pitch > 0.f && m_IsFirstPerson == false) {
		pitch = 0.f;
		return;
	}
	else if (m_pCameraComponent->RayCast(invUpVec, 5.f) && pitch < 0.f && m_IsFirstPerson == false) {
		pitch = 0.f;
		return;
	}
	//Redo pos 
	XMFLOAT3 controllerpos = m_pControllerComponent->GetFootPosition();
	controllerpos.y += m_CharacterDesc.controller.height; //add height of char
	XMVECTOR contollerPosSmd = XMLoadFloat3(&controllerpos);

	XMVECTOR cameraPosSmd = XMLoadFloat3(&m_pCamera->GetTransform()->GetWorldPosition());

	XMVECTOR cameraControllerVecsmd =  cameraPosSmd - contollerPosSmd;
	XMFLOAT3 cameraContollerVec;
	XMStoreFloat3(&cameraContollerVec, cameraControllerVecsmd);

	physx::PxRaycastBuffer hit;
	auto proxy = m_pControllerComponent->GetGameObject()->GetScene()->GetPhysxProxy();


	PxVec3 dirVec;
	dirVec.x = cameraContollerVec.x;
	dirVec.y = cameraContollerVec.y;
	dirVec.z = cameraContollerVec.z;
	dirVec = dirVec.getNormalized();

 
	PxVec3 posVec;
	posVec.x = m_pControllerComponent->GetTransform()->GetWorldPosition().x;
	posVec.y = m_pControllerComponent->GetTransform()->GetWorldPosition().y;
	posVec.z = m_pControllerComponent->GetTransform()->GetWorldPosition().z;
	XMFLOAT3 camPos;
	XMStoreFloat3(&camPos,XMLoadFloat3(&m_pCamera->GetTransform()->GetWorldPosition()) + XMLoadFloat3(&m_pCamera->GetTransform()->GetPosition()));

	float distance = std::sqrtf(std::powf(cameraContollerVec.x, 2) + std::powf(cameraContollerVec.y, 2) + std::powf(cameraContollerVec.z, 2));

	const PxU32 bufferSize = 32;        // [in] size of 'hitBuffer'
	PxRaycastHit hitBuffer[bufferSize];  // [out] User provided buffer for results
	PxRaycastBuffer buf(hitBuffer, bufferSize); // [out] Blocking and touching hits stored here
	//against static only
	PxQueryFilterData filterData(PxQueryFlag::eSTATIC);
	proxy->Raycast(posVec, dirVec.getNormalized(), m_IsFirstPerson ? 40.f : distance, buf, PxHitFlag::eDEFAULT, filterData);

	if (buf.nbTouches > 0 && m_IsFirstPerson == false) {
		//if cam is going through wall place camera at edge
		oldPos =  m_pCamera->GetTransform()->GetPosition();
		m_pCamera->GetTransform()->Translate(0.f, m_CharacterDesc.controller.height + 1.f, 0.f);
		m_IsFirstPerson = true;
	}
	else if(buf.nbTouches == 0 && m_IsFirstPerson == true) {
		//Reset camera to normal pos
		m_pCamera->GetTransform()->Translate(oldPos.x, oldPos.y, oldPos.z);

		m_pCamera->GetTransform()->LookAt(m_pControllerComponent->GetPosition());


		m_IsFirstPerson = false;
	}

	//std::cout << buf.nbTouches << std::endl;



		/*	if (hit.getNbTouches() > 0) {
				physx::PxRaycastHit newHit = hit.getTouch(1);
				std::cout << "wall in the way\n";
				m_pCamera->GetTransform()->Translate(newHit.position.x, newHit.position.y, newHit.position.z);
			}*/
		
	
}

void ThirdPersonCharacter::DrawImGui()
{
	if (ImGui::CollapsingHeader("Character"))
	{
		ImGui::Text(std::format("Move Speed: {:0.1f} m/s", m_MoveSpeed).c_str());
		ImGui::Text(std::format("Fall Speed: {:0.1f} m/s", m_TotalVelocity.y).c_str());

		ImGui::Text(std::format("Move Acceleration: {:0.1f} m/s2", m_MoveAcceleration).c_str());
		ImGui::Text(std::format("Fall Acceleration: {:0.1f} m/s2", m_FallAcceleration).c_str());

		const float jumpMaxTime = m_CharacterDesc.JumpSpeed / m_FallAcceleration;
		const float jumpMaxHeight = (m_CharacterDesc.JumpSpeed * jumpMaxTime) - (0.5f * (m_FallAcceleration * powf(jumpMaxTime, 2)));
		ImGui::Text(std::format("Jump Height: {:0.1f} m", jumpMaxHeight).c_str());

		ImGui::Dummy({ 0.f,5.f });
		if (ImGui::DragFloat("Max Move Speed (m/s)", &m_CharacterDesc.maxMoveSpeed, 0.1f, 0.f, 0.f, "%.1f") ||
			ImGui::DragFloat("Move Acceleration Time (s)", &m_CharacterDesc.moveAccelerationTime, 0.1f, 0.f, 0.f, "%.1f"))
		{
			m_MoveAcceleration = m_CharacterDesc.maxMoveSpeed / m_CharacterDesc.moveAccelerationTime;
		}

		ImGui::Dummy({ 0.f,5.f });
		if (ImGui::DragFloat("Max Fall Speed (m/s)", &m_CharacterDesc.maxFallSpeed, 0.1f, 0.f, 0.f, "%.1f") ||
			ImGui::DragFloat("Fall Acceleration Time (s)", &m_CharacterDesc.fallAccelerationTime, 0.1f, 0.f, 0.f, "%.1f"))
		{
			m_FallAcceleration = m_CharacterDesc.maxFallSpeed / m_CharacterDesc.fallAccelerationTime;
		}

		ImGui::Dummy({ 0.f,5.f });
		ImGui::DragFloat("Jump Speed", &m_CharacterDesc.JumpSpeed, 0.1f, 0.f, 0.f, "%.1f");
		ImGui::DragFloat("Rotation Speed (deg/s)", &m_CharacterDesc.rotationSpeed, 0.1f, 0.f, 0.f, "%.1f");

		bool isActive = m_pCameraComponent->IsActive();
		if (ImGui::Checkbox("Character Camera", &isActive))
		{
			m_pCameraComponent->SetActive(isActive);
		}
	}
}