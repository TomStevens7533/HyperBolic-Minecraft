#include "stdafx.h"
#include "DepthMaterial.h"

PostDepth::PostDepth() 
	: PostProcessingMaterial(L"Effects/Post/Depth.fx")
{

}

void PostDepth::UpdateBaseEffectVariables(const SceneContext& sc,  RenderTarget* pSource)
{
	XMMATRIX viewproj = XMLoadFloat4x4(&sc.pCamera->GetViewProjectionInverse());

	m_pBaseEffect->GetVariableByName("gWorldViewProj")->AsMatrix()->SetMatrix(reinterpret_cast<float*>(&viewproj));
	const auto pSourceSRV = pSource->GetColorShaderResourceView();
	m_pBaseEffect->GetVariableByName("gTexture")->AsShaderResource()->SetResource(pSourceSRV);

}
