#pragma once
class SkyboxMayerial final : public Material<SkyboxMayerial>
{
public:
	SkyboxMayerial();
	~SkyboxMayerial() override = default;

	SkyboxMayerial(const SkyboxMayerial& other) = delete;
	SkyboxMayerial(SkyboxMayerial&& other) noexcept = delete;
	SkyboxMayerial& operator=(const SkyboxMayerial& other) = delete;
	SkyboxMayerial& operator=(SkyboxMayerial&& other) noexcept = delete;


protected:
	void InitializeEffectVariables() override;
private:
	TextureData* m_CubeTexture;
};

