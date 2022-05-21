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
Texture2D gInit;

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
// Controls the Intensity of the bloom texture

float BloomIntensity = 1.f;

 

// Controls the Intensity of the original scene texture

float OriginalIntensity = 1.2;

 

// Saturation amount on bloom

float BloomSaturation = 1.8;

 

// Saturation amount on original scene

float OriginalSaturation = 1.6;

 

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
};

float4 AdjustSaturation(float4 color, float saturation)

{

    // We define gray as the same color we used in the grayscale shader

    float grey = dot(color, float3(0.3, 0.59, 0.11));

   

    return lerp(grey, color, saturation);

}

 
//VERTEX SHADER
//-------------
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;
	// Set the Position
    output.Position = float4(input.Position, 1.0f);
	// Set the TexCoord
    output.TexCoord = input.TexCoord;
	
	return output;
}


//PIXEL SHADER
//------------
float4 PS(PS_INPUT input): SV_Target
{
    // Step 1: sample the texture
    float4 colorGlow = gInit.Sample(samPoint, input.TexCoord);
    float4 colorInit = gTexture.Sample(samPoint, input.TexCoord);

        
    colorGlow = AdjustSaturation(colorGlow, BloomSaturation) * BloomIntensity;

    colorInit = AdjustSaturation(colorInit, OriginalSaturation) * OriginalIntensity;

    colorInit *= (1 - saturate(colorGlow));




    // Combine the two images.

    return colorInit + colorGlow;


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