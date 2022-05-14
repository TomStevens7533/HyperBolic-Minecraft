#include "stdafx.h"
#include "CameraComponent.h"

CameraComponent::CameraComponent() :
	m_FarPlane(2500.0f),
	m_NearPlane(0.1f),
	m_FOV(XM_PIDIV4),
	m_Size(25.0f),
	m_PerspectiveProjection(true)
{
	XMStoreFloat4x4(&m_Projection, XMMatrixIdentity());
	XMStoreFloat4x4(&m_View, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ViewInverse, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ViewProjection, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ViewProjectionInverse, XMMatrixIdentity());
}

void CameraComponent::Update(const SceneContext& sceneContext)
{
	// see https://stackoverflow.com/questions/21688529/binary-directxxmvector-does-not-define-this-operator-or-a-conversion
	using namespace DirectX;

	XMMATRIX projection{};

	if (m_PerspectiveProjection)
	{
		projection = XMMatrixPerspectiveFovLH(m_FOV, sceneContext.aspectRatio, m_NearPlane, m_FarPlane);
	}
	else
	{
		const float viewWidth = (m_Size > 0) ? m_Size * sceneContext.aspectRatio : sceneContext.windowWidth;
		const float viewHeight = (m_Size > 0) ? m_Size : sceneContext.windowHeight;
		projection = XMMatrixOrthographicLH(viewWidth, viewHeight, m_NearPlane, m_FarPlane);
	}

	const XMVECTOR worldPosition = XMLoadFloat3(&GetTransform()->GetWorldPosition());
	const XMVECTOR lookAt = XMLoadFloat3(&GetTransform()->GetForward());
	const XMVECTOR upVec = XMLoadFloat3(&GetTransform()->GetUp());

	const XMMATRIX view = XMMatrixLookAtLH(worldPosition, worldPosition + lookAt, upVec);
	const XMMATRIX viewInv = XMMatrixInverse(nullptr, view);
	const XMMATRIX viewProjectionInv = XMMatrixInverse(nullptr, view * projection);

	XMStoreFloat4x4(&m_Projection, projection);
	XMStoreFloat4x4(&m_View, view);
	XMStoreFloat4x4(&m_ViewInverse, viewInv);
	XMStoreFloat4x4(&m_ViewProjection, view * projection);
	XMStoreFloat4x4(&m_ViewProjectionInverse, viewProjectionInv);
}

void CameraComponent::SetActive(bool active)
{
	if (m_IsActive == active) return;

	const auto pGameObject = GetGameObject();
	ASSERT_IF(!pGameObject, L"Failed to set active camera. Parent game object is null");

	if (!pGameObject) return; //help the compiler... (C6011)
	const auto pScene = pGameObject->GetScene();
	ASSERT_IF(!pScene, L"Failed to set active camera. Parent game scene is null");

	m_IsActive = active;
	pScene->SetActiveCamera(active?this:nullptr); //Switch to default camera if active==false
}

GameObject* CameraComponent::Pick(const SceneContext& sceneContext, CollisionGroup ignoreGroups) const
{
	float mousePosX = static_cast<float>(sceneContext.pInput->GetMousePosition().x);
	float mousePosY = static_cast<float>(sceneContext.pInput->GetMousePosition().y);
	XMFLOAT3 forwardVec = GetTransform()->GetForward();

	//convert to ndc
	mousePosX = (mousePosX - (sceneContext.windowWidth / 2.0f)) / (sceneContext.windowWidth / 2.0f);
	mousePosY = ((sceneContext.windowHeight / 2.0f) - mousePosY) / (sceneContext.windowHeight / 2.0f);

	//transform with viewProj near point
	XMMATRIX viewproj = DirectX::XMLoadFloat4x4(&GetViewProjectionInverse());
	XMFLOAT3 newNearMousePos = XMFLOAT3{ mousePosX,mousePosY, 0.f };
	XMVECTOR smNewNearMousePos = XMLoadFloat3(&newNearMousePos);
	smNewNearMousePos = XMVector3TransformCoord(smNewNearMousePos, viewproj);
	XMStoreFloat3(&newNearMousePos, smNewNearMousePos);

	//transfrom with viewproj far point
	XMFLOAT3 newFarMousePos = XMFLOAT3{ mousePosX, mousePosY, 1.f };
	XMVECTOR smNewFarMousePos = XMLoadFloat3(&newFarMousePos);
	smNewFarMousePos = XMVector3TransformCoord(smNewFarMousePos, viewproj);
	XMStoreFloat3(&newFarMousePos, smNewFarMousePos);

	//Get dir between near and far
	XMFLOAT3 rayDir;
	XMStoreFloat3(&rayDir, XMLoadFloat3(&newFarMousePos) - XMLoadFloat3(&newNearMousePos));
	PxRaycastHit hitBuffer[m_RayCastBufferSize]; 
	PxRaycastBuffer buf(hitBuffer, m_RayCastBufferSize);

	if (RayCast(newNearMousePos, rayDir, PX_MAX_F32, &buf, ignoreGroups)) {
		physx::PxRaycastHit hit1 = buf.getAnyHit(0);
		if (hit1.actor->userData)
		{
			//transformcomponent because every gameobject has one so its 100% sure this one is gona have one too
			BaseComponent* baseComp = static_cast<BaseComponent*>(hit1.actor->userData);
			if (baseComp != nullptr) {
				return baseComp->GetGameObject();
			}
		}
	}

	TODO_W5(L"Implement Picking Logic")
	return nullptr;
}
bool CameraComponent::RayCast(XMFLOAT3 dir, float distance, physx::PxRaycastBuffer* hit) const
{
	auto proxy = GetGameObject()->GetScene()->GetPhysxProxy();
	PxVec3 dirVec;
	dirVec.x = dir.x;
	dirVec.y = dir.y;
	dirVec.z = dir.z;
	PxVec3 posVec;
	posVec.x = GetTransform()->GetWorldPosition().x;
	posVec.y = GetTransform()->GetWorldPosition().y;
	posVec.z = GetTransform()->GetWorldPosition().z;

	physx::PxQueryFilterData filterData;
	physx::PxRaycastBuffer rayhit;
	if (proxy->Raycast(posVec, dirVec.getNormalized(), distance, rayhit, physx::PxHitFlag::eDEFAULT)) {
		if (hit != nullptr)
			*hit = rayhit;
		return true;
	}
	return false;
}

bool CameraComponent::RayCast(XMFLOAT3 pos, XMFLOAT3 dir, float distance, physx::PxRaycastBuffer* hit, CollisionGroup ignoreGroups) const
{
	auto proxy = GetGameObject()->GetScene()->GetPhysxProxy();
	PxVec3 dirVec;
	dirVec.x = dir.x;
	dirVec.y = dir.y;
	dirVec.z = dir.z;
	PxVec3 posVec;
	posVec.x = pos.x;
	posVec.y = pos.y;
	posVec.z = pos.z;

	physx::PxQueryFilterData filterData;
	filterData.data.word0 = ~UINT(ignoreGroups);

	physx::PxRaycastBuffer rayhit;
	if (proxy->Raycast(posVec, dirVec.getNormalized(), distance, rayhit, physx::PxHitFlag::eDEFAULT, filterData)) {
		if (hit != nullptr)
			*hit = rayhit;
		return true;
	}
	return false;
}
