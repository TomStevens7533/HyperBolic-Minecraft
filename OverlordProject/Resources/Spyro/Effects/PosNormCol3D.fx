float4x4 gWorld : WORLD;
float4x4 gWorldViewProj : WORLDVIEWPROJECTION; 
float4x4 gWorldView : WORLDVIEW; 

float3 gLightDirection = float3(-0.577f, -0.577f, 0.577f);

// comment this to disable certain effects
#define VERTEX_SNAPPING
#define AFFINE_TEX_MAPPING

Texture2D gDiffuseMap;
SamplerState samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;// or Mirror or Clamp or Border
    AddressV = Wrap;// or Mirror or Clamp or Border
};

RasterizerState Solid
{
	FillMode = SOLID;
	CullMode = FRONT;
};

struct VS_INPUT{
	float3 pos : POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD0;
	float4 color : COLOR0;
};
struct VS_OUTPUT{
	float4 pos : SV_POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD;
	float2 texCoordLM : TEXCOORD;

	float4 color : COLOR0;
};

DepthStencilState EnableDepth
{
	DepthEnable = TRUE;
	DepthWriteMask = ALL;
};

RasterizerState NoCulling
{
	CullMode = NONE;
};

BlendState NoBlending
{
	BlendEnable[0] = FALSE;
};
#define TRANSFORM_TEX(tex,name) (tex.xy * name##.xy + name##.zw)

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT input){

	VS_OUTPUT output;
	// Step 1:	convert position into float4 and multiply with matWorldViewProj
	output.pos = mul(float4(input.pos,1.0f),gWorldViewProj );
	
	//Vertex Snapping ps1 graphics emulation
	float4 	vertex = output.pos;
	#ifdef VERTEX_SNAPPING
		vertex.xyz = vertex.xyz / vertex.w;
		vertex.x = floor(160 * vertex.x) / 160;
		vertex.y = floor(120 * vertex.y) / 120;
		vertex.xyz *= vertex.w;
		#else
	#endif
	
	//pos
	output.pos = vertex;

	//normal
	output.normal = normalize(mul(input.normal, (float3x3)gWorld));
	output.texCoord = input.texCoord;

	//affine texture mapping ( to reverse perspective divide * vertex.w);
	#ifdef AFFINE_TEX_MAPPING
		output.pos *= 1.f / length(mul(vertex, gWorldViewProj));
	#endif
	

	// float distance = length(mul(gWorldView,float4(output.pos)));
	// output.texCoord  *= distance + (vertex.w * 8) / distance / 2;		
	// output.texCoordLM =  distance + (vertex.w * 8) / distance / 2;
	return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_TARGET{

	//affine texture mapping ( to reverse perspective divide / vertex.w);
	float4 diffuseColor = gDiffuseMap.Sample( samLinear,input.texCoord);
	float3 color_rgb= diffuseColor.rgb;
	float color_a = diffuseColor.a;
	
	//return (input.color*diffuseColor)*1.6f; //1.6 is an empirically found value used to boost the lighting as in the original game
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
		SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);

		SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}


