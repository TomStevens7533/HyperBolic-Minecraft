//=============================================================================
//// Shader uses position and texture
//=============================================================================
SamplerState samPoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Mirror;
    AddressV = Mirror;
};

Texture2D gTexture;
float4x4 gWorldViewProj : WORLDVIEWPROJECTION; 


/// Create Depth Stencil State (ENABLE DEPTH WRITING)
DepthStencilState EnableDepth
{
	DepthEnable = TRUE;
	DepthWriteMask = ALL;
};
/// Create Rasterizer State (Backface culling) 
RasterizerState Back
{
	FillMode = SOLID;
	CullMode = BACK;
};

 

//IN/OUT STRUCTS
//--------------
struct VS_INPUT
{
    float3 Position : POSITION;
	float2 TexCoord : TEXCOORD0;

};

struct PS_INPUT
{
    float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD1;
    float depth : TEXCOORD0;
};

 
//VERTEX SHADER
//-------------
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;
	// Set the Position
    float4 position = float4(input.Position, 1.0f);
	// Set the TexCoord

    output.TexCoord = input.TexCoord;
	output.depth    = position.z / position.w;
	return output;
}


//PIXEL SHADER
//------------
float4 PS(PS_INPUT input): SV_Target
{
   




    // Combine the two images.

    return float4(1.f, 1.f, input.depth, 1.f);


}


//TECHNIQUE
//---------
technique11 Grayscale
{
    pass P0
    {          
        SetRasterizerState(Back);
		SetDepthStencilState(EnableDepth, 0);
        // Set states...
		SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}
