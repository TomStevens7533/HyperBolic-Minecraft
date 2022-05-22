#pragma once
//Resharper Disable All

	class GlowApply : public PostProcessingMaterial
	{
	public:
		GlowApply();
		~GlowApply() override = default;
		GlowApply(const GlowApply& other) = delete;
		GlowApply(GlowApply&& other) noexcept = delete;
		GlowApply& operator=(const GlowApply& other) = delete;
		GlowApply& operator=(GlowApply&& other) noexcept = delete;
		
	protected:
		void Initialize(const GameContext& /*gameContext*/) override {}
	};

