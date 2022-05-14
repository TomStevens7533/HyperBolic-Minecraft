#pragma once
class MainScreenScene final : public GameScene
{
public:
	MainScreenScene();
	~MainScreenScene() override = default;

	MainScreenScene(const MainScreenScene& other) = delete;
	MainScreenScene(MainScreenScene&& other) noexcept = delete;
	MainScreenScene& operator=(const MainScreenScene& other) = delete;
	MainScreenScene& operator=(MainScreenScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void OnGUI() override;

private:
	GameObject* m_pUI;
	enum InputIds
	{
		Enter
	};
};



