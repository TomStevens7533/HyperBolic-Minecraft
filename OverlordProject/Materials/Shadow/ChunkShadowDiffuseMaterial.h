#pragma once
class ChunkShadowDifffuseMaterial final : public Material<ChunkShadowDifffuseMaterial>
{
public:
	ChunkShadowDifffuseMaterial();
	~ChunkShadowDifffuseMaterial() override = default;

	ChunkShadowDifffuseMaterial(const ChunkShadowDifffuseMaterial& other) = delete;
	ChunkShadowDifffuseMaterial(ChunkShadowDifffuseMaterial&& other) noexcept = delete;
	ChunkShadowDifffuseMaterial& operator=(const ChunkShadowDifffuseMaterial& other) = delete;
	ChunkShadowDifffuseMaterial& operator=(ChunkShadowDifffuseMaterial&& other) noexcept = delete;

	void SetDiffuseTexture(const std::wstring& assetFile);

protected:
	void InitializeEffectVariables() override;
	void OnUpdateModelVariables(const SceneContext& sceneContext, const BaseComponent* pModel) const override;

private:
	TextureData* m_pDiffuseTexture{};
};

