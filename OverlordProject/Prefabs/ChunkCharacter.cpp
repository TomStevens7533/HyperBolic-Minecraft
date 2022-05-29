#include "stdafx.h"
#include "ChunkCharacter.h"
#include "ChunkManager.h"
#include "Components/ParticleEmitterComponent.h"
#include "Materials/Shadow/DiffuseMaterial_Shadow_Skinned.h"

CharacterChunk::CharacterChunk(const CharacterChunkDesc& characterDesc, const ChunkManager* pManager) :
	m_CharacterDesc{ characterDesc },
	m_MoveAcceleration(characterDesc.maxMoveSpeed / characterDesc.moveAccelerationTime),
	m_FallAcceleration(characterDesc.maxFallSpeed / characterDesc.fallAccelerationTime),
	m_pChunkManager{pManager}
{

}



void CharacterChunk::Initialize(const SceneContext&)
{
	//Controller
	m_pControllerComponent = AddComponent(new ControllerComponent(m_CharacterDesc.controller));

	//Camera
	m_pCamera = AddChild(new FixedCamera());
	m_pCameraComponent = m_pCamera->GetComponent<CameraComponent>();

	
	const auto pSkinnedMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow_Skinned>();
	pSkinnedMaterial->SetDiffuseTexture(L"Textures/PeasantGirl_Diffuse.png");
	auto handGO = AddChild(new GameObject());
	auto handComp =handGO->AddComponent(new ModelComponent(L"Meshes/hand_low.ovm"));
	//handComp->GetTransform()->Scale(XMFLOAT3(0.1f, 0.1f, 0.1f));
	handComp->SetMaterial(pSkinnedMaterial);

	handGO->GetTransform()->Scale(0.0025f, 0.0025f, 0.008f);
	m_pCamera->GetTransform()->Translate(XMVECTOR{ 0,0.13f, 0 });
	handGO->GetTransform()->Translate(XMVECTOR{ 0.055f,0.02f, 0.f });
	//handGO->GetTransform()->Rotate(XMVECTOR{ -60, -60, 0 }, false);

	pAnimator = handComp->GetAnimator();
	pAnimator->SetAnimationSpeed(4.f);
	pAnimator->SetPlayOnce(true);

	m_pCameraComponent->SetActive(true); //Uncomment to make this camera the active camera




}

void CharacterChunk::Update(const SceneContext& sceneContext)
{
	if (m_IsDisabled == false) {
		if (m_pCameraComponent->IsActive())
		{

			constexpr float epsilon{ 0.01f }; //Constant that can be used to compare if a float is near zero
			auto deltaTime = sceneContext.pGameTime->GetElapsed();

			//***************
			//HANDLE INPUT

			//## Input Gathering (move)
			XMFLOAT2 move{ 0,0 }; //Uncomment

			//move.y should contain a 1 (Forward) or -1 (Backward) based on the active input (check corresponding actionId in m_CharacterDesc)
			//Optional: if move.y is near zero (abs(move.y) < epsilon), you could use the ThumbStickPosition.y for movement
			if ((abs(move.x) < epsilon)) {
				if (sceneContext.pInput->IsActionTriggered(m_CharacterDesc.actionId_MoveForward)) {
					move.y = 1.f;
				}
				else if (sceneContext.pInput->IsActionTriggered(m_CharacterDesc.actionId_MoveBackward))
					move.y = -1.f;
			}
			else
				move.y = sceneContext.pInput->GetThumbstickPosition().y;




			//move.x should contain a 1 (Right) or -1 (Left) based on the active input (check corresponding actionId in m_CharacterDesc)
			//Optional: if move.x is near zero (abs(move.x) < epsilon), you could use the Left ThumbStickPosition.x for movement
			if ((abs(move.x) < epsilon)) {
				if (sceneContext.pInput->IsActionTriggered(m_CharacterDesc.actionId_MoveRight))
					move.x = 1.f;
				else if (sceneContext.pInput->IsActionTriggered(m_CharacterDesc.actionId_MoveLeft))
					move.x = -1.f;
			}
			else
				move.x = sceneContext.pInput->GetThumbstickPosition().x;



			//## Input Gathering (look)
			XMFLOAT2 look{ 0.f, 0.f }; //Uncomment
			//Only if the Left Mouse Button is Down >
			// Store the MouseMovement in the local 'look' variable (cast is required)
			//Optional: in case look.x AND look.y are near zero, you could use the Right ThumbStickPosition for look

			
				POINT newLook = sceneContext.pInput->GetMouseMovement();
				look.x = static_cast<float>(newLook.x / m_sensitivity);
				look.y = static_cast<float>(newLook.y) / m_sensitivity;


				//***************
				//CAMERA ROTATION

				//Adjust the TotalYaw (m_TotalYaw) & TotalPitch (m_TotalPitch) based on the local 'look' variable
				//Make sure this calculated on a framerate independent way and uses CharacterDesc::rotationSpeed.
				//Rotate this character based on the TotalPitch (X) and TotalYaw (Y)

				m_TotalYaw += look.x * m_CharacterDesc.rotationSpeed * deltaTime;
				m_TotalPitch += look.y * m_CharacterDesc.rotationSpeed * deltaTime;

				GetTransform()->Rotate(m_TotalPitch, m_TotalYaw, 0);


			

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

				bool isForwardClear = false;
				bool isBackwardsClear = false;
				bool isRightClear = false;
				bool isLeftClear = false;

				//rotate local vectors to player for raycastDetection
				DirectX::XMMATRIX rotMat;
				rotMat = XMMatrixRotationRollPitchYaw(XMConvertToRadians(0),
					XMConvertToRadians(m_TotalYaw),
					XMConvertToRadians(0));

				{	//down
					XMFLOAT3 lookupPos = m_pControllerComponent->GetFootPosition();
					lookupPos.y += m_CharacterDesc.controller.height / 2.f;
					XMVECTOR newPos = XMLoadFloat3(&lookupPos);

					XMFLOAT3 backVec = XMFLOAT3{ 0,0,-m_CharacterDesc.controller.radius };
					XMVECTOR backVecSmd;

					backVecSmd = XMVector3TransformNormal(XMLoadFloat3(&backVec), rotMat);
					XMVECTOR fullDir = (backVecSmd);
					newPos += fullDir;
					XMStoreFloat3(&lookupPos, newPos);
					if (m_pChunkManager->IsBlockSolid(lookupPos))
					{
						m_TotalVelocity.y = 0.f;
						isBackwardsClear = true;

					}
				}

				{	//up
					XMFLOAT3 lookupPos = m_pControllerComponent->GetFootPosition();
					lookupPos.y += m_CharacterDesc.controller.height / 2.f;

					XMVECTOR newPos = XMLoadFloat3(&lookupPos);

					XMFLOAT3 upVec = XMFLOAT3{ 0,0,m_CharacterDesc.controller.radius };
					XMVECTOR UpVecsmd;

					UpVecsmd = XMVector3TransformNormal(XMLoadFloat3(&upVec), rotMat);
					XMVECTOR fullDir = (UpVecsmd);
					newPos += fullDir;
					XMStoreFloat3(&lookupPos, newPos);
					if (m_pChunkManager->IsBlockSolid(lookupPos))
					{
						m_TotalVelocity.y = 0.f;
						isForwardClear = true;

					}
				}

				{	//left
					XMFLOAT3 lookupPos = m_pControllerComponent->GetFootPosition();
					lookupPos.y += m_CharacterDesc.controller.height / 2.f;

					XMVECTOR newPos = XMLoadFloat3(&lookupPos);
					XMFLOAT3 leftVec = XMFLOAT3{ -m_CharacterDesc.controller.radius,0, 0 };
					XMVECTOR leftVecsmd;

					leftVecsmd = XMVector3TransformNormal(XMLoadFloat3(&leftVec), rotMat);
					XMVECTOR fullDir = (leftVecsmd);
					newPos -= fullDir;
					DirectX::XMStoreFloat3(&lookupPos, newPos);
					if (m_pChunkManager->IsBlockSolid(lookupPos))
					{
						m_TotalVelocity.y = 0.f;
						isLeftClear = true;

					}
				}
				{ //right
					XMFLOAT3 lookupPos = m_pControllerComponent->GetFootPosition();
					lookupPos.y += m_CharacterDesc.controller.height / 2.f;

					XMVECTOR newPos = XMLoadFloat3(&lookupPos);
					XMFLOAT3 rightVec = XMFLOAT3{ m_CharacterDesc.controller.radius,0, 0 };
					XMVECTOR rightVecsmd;

					rightVecsmd = XMVector3TransformNormal(XMLoadFloat3(&rightVec), rotMat);
					XMVECTOR fullDir = (rightVecsmd);
					newPos += fullDir;
					XMStoreFloat3(&lookupPos, newPos);
					if (m_pChunkManager->IsBlockSolid(lookupPos))
					{
						m_TotalVelocity.y = 0.f;
						isRightClear = true;

					}

				}
				if (move.y > 0.f && isForwardClear)
					move.y = 0.f;
				else if (move.y < 0.f && isBackwardsClear)
					move.y = 0.f;

				if (move.x > 0.f && isRightClear)
					move.x = 0.f;
				else if (move.x < 0.f && isLeftClear)
					move.x = 0.f;

				m_CurrentDirection.x = ((forward.x * move.y) + (right.x * move.x));
				m_CurrentDirection.z = ((forward.z * move.y) + (right.z * move.x));





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
			if (m_IsCreative)//If the Controller Component is NOT grounded (= freefall)
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

				if (sceneContext.pInput->IsActionTriggered(m_CharacterDesc.actionId_Crouch))
				{
					if (m_TotalVelocity.y >= -m_CharacterDesc.maxFallSpeed)
					{
						m_TotalVelocity.y = -m_CharacterDesc.maxFallSpeed;
					}
				}
			}
			else 	//If the Controller Component is NOT grounded (= freefall)
			{
				bool isOnGround = false;
					XMFLOAT3 lookupPos = m_pControllerComponent->GetFootPosition();
					XMVECTOR newPos = XMLoadFloat3(&lookupPos);

					XMFLOAT3 downVec = XMFLOAT3{ 0,-0.1f,0 };
					XMVECTOR fullDir = (XMLoadFloat3(&downVec));
					newPos += fullDir;
					DirectX::XMStoreFloat3(&lookupPos, newPos);
					if (m_pChunkManager->IsBlockSolid(lookupPos))
					{
						m_TotalVelocity.y = 0.f;
						isOnGround = true;

					}

				if (isOnGround && sceneContext.pInput->IsActionTriggered(m_CharacterDesc.actionId_Jump)) {
					//Set m_TotalVelocity.y equal to CharacterDesc::JumpSpeed
					m_TotalVelocity.y = m_CharacterDesc.JumpSpeed;
				}
				else if (isOnGround == false)
				{
					m_TotalVelocity.y -= 0.2f;
					if (m_TotalVelocity.y <= -m_CharacterDesc.maxFallSpeed)
					{
						m_TotalVelocity.y = -m_CharacterDesc.maxFallSpeed;
					}





					//Else (=Character is grounded, no input pressed)
					//m_TotalVelocity.y is zero

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
	
}

void CharacterChunk::DrawImGui()
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

std::pair<XMFLOAT3, XMFLOAT3> CharacterChunk::ScreenSpaceToWorldPosAndDir(const SceneContext& sceneContext, XMFLOAT2 pointScale)
{
	float mousePosX = static_cast<float>(sceneContext.windowWidth * pointScale.x);
	float mousePosY = static_cast<float>(sceneContext.windowHeight * pointScale.y);
	XMFLOAT3 forwardVec = m_pCameraComponent->GetTransform()->GetForward();

	//convert to ndc
	mousePosX = (mousePosX - (sceneContext.windowWidth / 2.0f)) / (sceneContext.windowWidth / 2.0f);
	mousePosY = ((sceneContext.windowHeight / 2.0f) - mousePosY) / (sceneContext.windowHeight / 2.0f);

	//transform with viewProj near point
	XMMATRIX viewproj = DirectX::XMLoadFloat4x4(&m_pCameraComponent->GetViewProjectionInverse());
	XMFLOAT3 newNearMousePos = XMFLOAT3{ mousePosX,mousePosY, 0.f };
	XMVECTOR smNewNearMousePos = XMLoadFloat3(&newNearMousePos);
	smNewNearMousePos = XMVector3TransformCoord(smNewNearMousePos, viewproj);
	DirectX::XMStoreFloat3(&newNearMousePos, smNewNearMousePos);


	//transfrom with viewproj far point
	XMFLOAT3 newFarMousePos = XMFLOAT3{ mousePosX, mousePosY, 1.f };
	XMVECTOR smNewFarMousePos = XMLoadFloat3(&newFarMousePos);
	smNewFarMousePos = XMVector3TransformCoord(smNewFarMousePos, viewproj);
	DirectX::XMStoreFloat3(&newFarMousePos, smNewFarMousePos);

	//Get dir between near and far
	XMFLOAT3 rayDir;
	DirectX::XMStoreFloat3(&rayDir, XMVector3Normalize(XMLoadFloat3(&newFarMousePos) - XMLoadFloat3(&newNearMousePos)));
	return std::make_pair(newNearMousePos, rayDir);
}

void CharacterChunk::PlayAnimatation()
{
	pAnimator->Reset(true);
	pAnimator->Play();
}
