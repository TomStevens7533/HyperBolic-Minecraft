#include "stdafx.h"
#include "ComponentTestScene.h"

#include "Prefabs/SpherePrefab.h"

void ComponentTestScene::Initialize()
{


	auto pBouncyMaterial = PxGetPhysics().createMaterial(.5f,.5f,.9f);
	GameSceneExt::CreatePhysXGroundPlane(*this, pBouncyMaterial);

	//sphere 1
	auto pSpherObject = AddChild(new SpherePrefab(1.f, 10, XMFLOAT4{Colors::Red}));
	auto pSphereActor = pSpherObject->AddComponent(new RigidBodyComponent(false));
	pSphereActor->AddCollider(PxSphereGeometry{ 1.f }, *pBouncyMaterial);
	pSphereActor->SetCollisionGroup(CollisionGroup::Group1 | CollisionGroup::Group2);
	pSpherObject->GetTransform()->Translate(0.f, 40.f, 0.f);

	//sphere 2
	 pSpherObject = AddChild(new SpherePrefab(1.f, 10, XMFLOAT4{ Colors::Green }));
	 pSphereActor = pSpherObject->AddComponent(new RigidBodyComponent(false));
	pSphereActor->AddCollider(PxSphereGeometry{ 1.f }, *pBouncyMaterial);
	pSphereActor->SetCollisionIgnoreGroups(CollisionGroup::Group1 | CollisionGroup::Group2);
	pSpherObject->GetTransform()->Translate(0.f, 30.f, 0.f);

	//sphere 3
	 pSpherObject = AddChild(new SpherePrefab(1.f, 10, XMFLOAT4{ Colors::Blue }));
	 pSphereActor = pSpherObject->AddComponent(new RigidBodyComponent(false));
	pSphereActor->AddCollider(PxSphereGeometry{ 1.f }, *pBouncyMaterial);
	pSphereActor->SetCollisionIgnoreGroups(CollisionGroup::Group1 | CollisionGroup::Group2);


	pSpherObject->GetTransform()->Translate(0.f, 32.f, 0.f);

}

void ComponentTestScene::Update()
{
	GameScene::Update();
}

void ComponentTestScene::Draw()
{
	GameScene::Draw();
}
