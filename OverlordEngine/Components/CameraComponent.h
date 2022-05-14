#pragma once

class CameraComponent: public BaseComponent
{
public:
	CameraComponent();
	~CameraComponent() override = default;

	CameraComponent(const CameraComponent& other) = delete;
	CameraComponent(CameraComponent&& other) noexcept = delete;
	CameraComponent& operator=(const CameraComponent& other) = delete;
	CameraComponent& operator=(CameraComponent&& other) noexcept = delete;

	void SetFieldOfView(float fov) { m_FOV = fov; }
	void SetOrthoSize(float size) { m_Size = size; }
	void SetNearClippingPlane(float nearPlane) { m_NearPlane = nearPlane; }
	void SetFarClippingPlane(float farPlane) { m_FarPlane = farPlane; }

	void UsePerspectiveProjection() { m_PerspectiveProjection = true; }
	void UseOrthographicProjection() { m_PerspectiveProjection = false; }

	void SetActive(bool active = true);
	bool IsActive() const {return m_IsActive;}

	const XMFLOAT4X4& GetView() const {return m_View;}
	const XMFLOAT4X4& GetProjection() const {return m_Projection;}
	const XMFLOAT4X4& GetViewProjection() const {return m_ViewProjection;}
	const XMFLOAT4X4& GetViewInverse() const {return m_ViewInverse;}
	const XMFLOAT4X4& GetViewProjectionInverse() const {return m_ViewProjectionInverse;}

	GameObject* Pick(const SceneContext& sceneContext, CollisionGroup ignoreGroups = CollisionGroup::None) const;
	bool RayCast(XMFLOAT3 dir, float distance, physx::PxRaycastBuffer* hit = nullptr) const;


protected:

	void Initialize(const SceneContext& /*sceneContext*/) override {};
	void Update(const SceneContext& sceneContext) override;
private:
	bool RayCast(XMFLOAT3 pos, XMFLOAT3 dir, float distance, physx::PxRaycastBuffer* hit = nullptr, CollisionGroup ignoreGroups = CollisionGroup::None) const;
private:
	XMFLOAT4X4 m_View{};
	XMFLOAT4X4 m_Projection{};
	XMFLOAT4X4 m_ViewInverse{};
	XMFLOAT4X4 m_ViewProjection{};
	XMFLOAT4X4 m_ViewProjectionInverse{};

	float m_FarPlane{}, m_NearPlane{}, m_FOV{}, m_Size{};
	bool m_IsActive{}, m_PerspectiveProjection{};
	static constexpr PxU32 m_RayCastBufferSize = 32;

};

