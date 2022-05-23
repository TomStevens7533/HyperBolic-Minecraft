#include "stdafx.h"
#include "DepthMaterial.h"

PostDepth::PostDepth() 
	: Material<PostDepth>(L"Effects/Post/Depth.fx")
{

}


void PostDepth::InitializeEffectVariables()
{

}

void PostDepth::OnUpdateModelVariables(const SceneContext&, const BaseComponent*) const
{

}

