float4x4 gWorld : WORLD;
float4x4 gWorldViewProj : WORLDVIEWPROJECTION; 
float3 gLightDirection = float3(-0.577f, -0.577f, 0.577f);

TextureCube gDiffuseMap;
SamplerState samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;// or Mirror or Clamp or Border
    AddressV = Wrap;// or Mirror or Clamp or Border
};

RasterizerState Solid
{
	FillMode = WIREFRAME;
	CullMode = FRONT;
};

struct VS_INPUT{
	float3 pos : POSITION;
    float3 normal : NORMAL;
	float4 color : COLOR;
};
struct VS_OUTPUT{
	float4 pos : POSITION;
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

BlendState AlphaBlending 
{     
	BlendEnable[0] = FALSE;
	SrcBlend = SRC_ALPHA;
    DestBlend = INV_SRC_ALPHA;
	BlendOp = ADD;
	SrcBlendAlpha = ONE;
	DestBlendAlpha = ZERO;
	BlendOpAlpha = ADD;
	RenderTargetWriteMask[0] = 0x0f;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT input){

	VS_OUTPUT output;
	// Step 1:	convert position into float4 and multiply with matWorldViewProj
	output.pos = mul( float4(input.pos,1.0f), gWorldViewProj).xyww;
	output.texCoord = input.pos;
	// Step 2:	rotate the normal: NO TRANSLATION
	//			this is achieved by clipping the 4x4 to a 3x3 matrix, 
	//			thus removing the postion row of the matrix
	return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_TARGET{

//	float4 diffuseColor = gDiffuseMap.Sample( samLinear,input.texCoord );
	//float4 diffuseColor = float4(1.f, 1.f, 1.f, 1.f);



	return float4(1.f, 1.f, 1.f, 1.f);
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
		SetBlendState(AlphaBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);

		SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}

