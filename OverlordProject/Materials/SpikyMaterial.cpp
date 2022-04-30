#include "stdafx.h"
#include "SpikyMaterial.h"

SpikyMaterial::SpikyMaterial() :
	Material(L"Effects/SpikyShader.fx")
{
}

void SpikyMaterial::InitializeEffectVariables()
{
	SetVariable_Scalar(L"gSpikeLength", 0.3f);
	SetVariable_Vector(L"gColorDiffuse",{1.f, 0.1f, 0.1f});


}




