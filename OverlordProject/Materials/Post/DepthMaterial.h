#pragma once
class PostDepth : public PostProcessingMaterial
{
public:
	PostDepth();
	~PostDepth() override = default;
	PostDepth(const PostDepth& other) = delete;
	PostDepth(PostDepth&& other) noexcept = delete;
	PostDepth& operator=(const PostDepth& other) = delete;
	PostDepth& operator=(PostDepth&& other) noexcept = delete;

protected:
	void Initialize(const GameContext& /*gameContext*/) override {}

	void UpdateBaseEffectVariables(const SceneContext& sceneContext, RenderTarget* pSource) override;

};

