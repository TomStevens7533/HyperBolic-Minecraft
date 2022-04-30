#pragma once
#include <string>
class ChunkDiffuseMaterial final : public Material<ChunkDiffuseMaterial>
{
public:
	ChunkDiffuseMaterial();
	~ChunkDiffuseMaterial() override = default;

	ChunkDiffuseMaterial(const ChunkDiffuseMaterial& other) = delete;
	ChunkDiffuseMaterial(ChunkDiffuseMaterial&& other) noexcept = delete;
	ChunkDiffuseMaterial& operator=(const ChunkDiffuseMaterial& other) = delete;
	ChunkDiffuseMaterial& operator=(ChunkDiffuseMaterial&& other) noexcept = delete;

	void SetDiffuseTexture(const std::wstring& assetFile);

protected:
	void InitializeEffectVariables() override;
private:
	TextureData* m_pDiffuseTexture;
};

