#pragma once
#include "Prefabs/SpherePrefab.h"
#include "Prefabs/CubePrefab.h"
class PongScene final : public GameScene
{
public:
	PongScene();
	~PongScene() override = default;

	PongScene(const PongScene& other) = delete;
	PongScene(PongScene&& other) noexcept = delete;
	PongScene& operator=(const PongScene& other) = delete;
	PongScene& operator=(PongScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;

	void OnTriggerCallback(GameObject* pTriggerObject, GameObject* pOtherObject, PxTriggerAction action);

private:
	SpherePrefab* m_pSphere;
	CubePrefab* m_pPaddle1;
	CubePrefab* m_pPaddle2;

	
	CubePrefab* m_pStaticTop;
	CubePrefab* m_pStaticBot;

	GameObject* m_pTriggerTop;
	GameObject* m_pTriggerBottom;

	GameObject* m_pTriggerLeft;
	GameObject* m_pTriggerRight;

	std::function<void(GameObject*, GameObject*, PxTriggerAction)> m_TriggeredFunction;

	const float m_PaddleSpeed = 0.1f;
	const float m_BallSpeed = 20.f;
	enum InputID {
		MoveUp1,
		MoveDown1,
		MoveUp2,
		MoveDown2
	};
};