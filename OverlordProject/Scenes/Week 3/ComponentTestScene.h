#pragma once
class ComponentTestScene final : public GameScene
{
public:
	ComponentTestScene() : GameScene(L"ComponentScene") {};
protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;
};

