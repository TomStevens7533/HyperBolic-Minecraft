float4x4 gWorld : WORLD;
float4x4 gWorldViewProj : WORLDVIEWPROJECTION; 
float3 gLightDirection = float3(-0.577f, -0.577f, 0.577f);

#define VERTEX_SNAPPING
#define AFFINE_TEX_MAPPING

struct VS_INPUT{
	float3 pos : POSITION;
	float3 normal : NORMAL;
	float4 color : COLOR;
};
struct VS_OUTPUT{
	float4 pos : SV_POSITION;
	float3 normal : NORMAL;
	float4 color : COLOR;
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

BlendState EnableBlending
{
	BlendEnable[0] = TRUE;
	SrcBlend = SRC_ALPHA;
	DestBlend = INV_SRC_ALPHA;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT input){
	VS_OUTPUT output;
	// Step 1:	convert position into float4 and multiply with matWorldViewProj
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
	
	//pos
	// Step 2:	rotate the normal: NO TRANSLATION
	//			this is achieved by clipping the 4x4 to a 3x3 matrix, 
	//			thus removing the postion row of the matrix
	output.normal = normalize(mul(input.normal, (float3x3)gWorld));
	// Step3:	Just copy the color
	output.color=input.color;
	return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_TARGET{

	float3 color_rgb= input.color.rgb;
	float color_a = input.color.a;

	
	return float4( color_rgb , color_a );
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

technique11 TransparencyTech
{
	pass P0
	{
		SetRasterizerState(NoCulling);
		SetDepthStencilState(EnableDepth, 0);
		SetBlendState(EnableBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);

		SetVertexShader(CompileShader(vs_4_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PS()));
	}
}

