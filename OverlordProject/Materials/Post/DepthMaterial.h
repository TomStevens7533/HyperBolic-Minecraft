#pragma once
class PostDepth final : public Material<PostDepth>
{
public:
	PostDepth();
	~PostDepth() override = default;

	PostDepth(const PostDepth& other) = delete;
	PostDepth(PostDepth&& other) noexcept = delete;
	PostDepth& operator=(const PostDepth& other) = delete;
	PostDepth& operator=(PostDepth&& other) noexcept = delete;

protected:



	void InitializeEffectVariables() override;


	void OnUpdateModelVariables(const SceneContext&, const BaseComponent*) const override;

};

