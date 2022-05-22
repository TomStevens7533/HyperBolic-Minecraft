#pragma once
class GlowGenerator : public PostProcessingMaterial
{
public:
	GlowGenerator();
	~GlowGenerator() override = default;
	GlowGenerator(const GlowGenerator& other) = delete;
	GlowGenerator(GlowGenerator&& other) noexcept = delete;
	GlowGenerator& operator=(const GlowGenerator& other) = delete;
	GlowGenerator& operator=(GlowGenerator&& other) noexcept = delete;

protected:
	void Initialize(const GameContext& /*gameContext*/) override {}
};