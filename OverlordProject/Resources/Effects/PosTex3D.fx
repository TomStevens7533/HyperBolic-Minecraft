float4x4 gWorld : WORLD;
float4x4 gWorldViewProj : WORLDVIEWPROJECTION; 
float4x4 gWorldViewProj_Light;
float3 gLightDirection = float3(-0.577f, -0.577f, 0.577f);
float gShadowMapBias = 0.01f;

Texture2D gDiffuseMap;
Texture2D gShadowMap;
SamplerComparisonState cmpSampler
{
	// sampler state
	Filter = COMPARISON_MIN_MAG_MIP_LINEAR;
	AddressU = MIRROR;
	AddressV = MIRROR;

	// sampler comparison state
	ComparisonFunc = LESS_EQUAL;
};
SamplerState samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;// or Mirror or Clamp or Border
    AddressV = Wrap;// or Mirror or Clamp or Border
};
SamplerState samPoint
{
	Filter = MIN_MAG_MIP_POINT;
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
	float2 texCoord : TEXCOORD;
	float lightLevel : TANGENT;
};
struct VS_OUTPUT{
	float4 pos : SV_POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD;
	float lightLevel : TANGENT;
	float4 lPos : TEXCOORD1;
	

};

DepthStencilState EnableDepth
{
	DepthEnable = TRUE;
	DepthWriteMask = ALL;
};

RasterizerState NoCulling
{
	//FillMode = WIREFRAME;
	CullMode = NONE;
};

BlendState AlphaBlending 
{     
	BlendEnable[0] = TRUE;
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
	output.pos = mul ( float4(input.pos,1.0f), gWorldViewProj );
	// Step 2:	rotate the normal: NO TRANSLATION
	output.normal = input.normal;
	//			this is achieved by clipping the 4x4 to a 3x3 matrix, 
	//			thus removing the postion row of the matrix
	output.lPos = mul(float4(input.pos, 1.0f), gWorldViewProj_Light);
	output.texCoord = input.texCoord;
	output.lightLevel = input.lightLevel;
	return output;
}
float2 texOffset(int u, int v)
{
	//TODO: return offseted value (our shadow map has the following dimensions: 1280 * 720)
	return float2(u / 1280.f,v / 720.f);
}

float EvaluateShadowMap(float3 normal, float4 lpos)
{
	//https://takinginitiative.wordpress.com/2011/05/25/directx10-tutorial-10-shadow-mapping-part-2/
	//re-homogenize position after interpolation
    lpos.xyz /= lpos.w;
 
    //if position is not visible to the light - dont illuminate it
    //results in hard light frustum
	if (lpos.x < -1.0f || lpos.x > 1.0f ||
		lpos.y < -1.0f || lpos.y > 1.0f ||
		lpos.z < 0.0f || lpos.z > 1.0f)
	{		
		return 1.0f;
	}
 
    //transform clip space coords to texture space coords (-1:1 to 0:1)
    lpos.x = (lpos.x/2.f) + 0.5;
    lpos.y = (lpos.y/-2.f) + 0.5;
 
    //apply shadow map bias
    lpos.z -= gShadowMapBias;
 
    //PCF sampling for shadow map
    float sum = 0;
    float x, y;
 
    //perform PCF filtering on a 4 x 4 texel neighborhood
    for (y = -1.5; y <= 1.5; y += 1.0)
    {
        for (x = -1.5; x <= 1.5; x += 1.0)
        {
            sum += gShadowMap.SampleCmpLevelZero( cmpSampler, lpos.xy + texOffset(x,y), lpos.z );
        }
    }
 
    float shadowFactor = sum / 16.0;
 

 	 
	float ndotl = dot( normal, -gLightDirection);
	ndotl  = clamp(ndotl, 0.3f, 1.f);
	return (shadowFactor * ndotl)  + 0.5f;
}
//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_TARGET{

	float shadowValue = EvaluateShadowMap(input.normal, input.lPos);

	float4 diffuseColor = gDiffuseMap.Sample( samLinear,input.texCoord );
	//float4 diffuseColor = float4(1.f, 1.f, 1.f, 1.f);

	float3 color_rgb= float3(diffuseColor.r * input.lightLevel
	, diffuseColor.g * input.lightLevel
	, diffuseColor.b * input.lightLevel);
	float color_a = diffuseColor.a;
	

	if(color_a < 0.1f)
		discard;





	return float4( color_rgb * shadowValue, color_a );
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

