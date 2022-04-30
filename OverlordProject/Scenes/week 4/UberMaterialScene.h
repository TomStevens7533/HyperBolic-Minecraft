#pragma once
#include "Materials/UberMaterial.h"
class UberMaterialScene final : public GameScene
{
public:
	UberMaterialScene();
	~UberMaterialScene() override = default;

	UberMaterialScene(const UberMaterialScene& other) = delete;
	UberMaterialScene(UberMaterialScene&& other) noexcept = delete;
	UberMaterialScene& operator=(const UberMaterialScene& other) = delete;
	UberMaterialScene& operator=(UberMaterialScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void OnGUI() override;

private:
	GameObject* m_pSphere{};
	UberMaterial* m_UberMaterial;
	float m_AngleYaxis = 0;
	GameTime m_Time;
private:
	XMFLOAT3 m_FresnelColor;
	XMFLOAT3 m_DiffuseColor;
	XMFLOAT3 m_SpecularColor;

	float m_FresnelPower;
	float m_FresnelMultiplier;
	float m_FresnelHardness;
	float m_Shininness;




};


