float4x4 gWorldViewProj : WORLDVIEWPROJECTION; 

TextureCube gDiffuseMap : CubeMap;
SamplerState samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;// or Mirror or Clamp or Border
    AddressV = Wrap;// or Mirror or Clamp or Border
	AddressW = WRAP;
};



struct VS_INPUT{
	float3 pos : POSITION;
};
struct VS_OUTPUT{
	float4 pos : SV_POSITION;
	float3 texCoord : TEXCOORD;
};

DepthStencilState EnableDepth
{
	DepthEnable = TRUE;
	DepthFunc = LESS_EQUAL;
	DepthWriteMask = ALL;
};

RasterizerState NoCulling
{
	//FillMode = WIREFRAME;
	CullMode = NONE;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT input){

	VS_OUTPUT output;
	// Step 1:	convert position into float4 and multiply with matWorldViewProj
	output.pos = mul( float4(input.pos,0.0f), gWorldViewProj).xyww;
	output.texCoord = input.pos;
	return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_TARGET{

	float4 diffuseColor = gDiffuseMap.Sample( samLinear,input.texCoord );
	//float4 diffuseColor = float4(1.f, 1.f, 1.f, 1.f);



	return diffuseColor;
}

//--------------------------------------------------------------------------------------
// Technique
//--------------------------------------------------------------------------------------
technique11 Default
{
    pass P0
    {
		SetRasterizerState(NoCulling);
		SetDepthStencilState(EnableDepth, 0);

		SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}

