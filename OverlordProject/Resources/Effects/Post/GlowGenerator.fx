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

float Threshold = 0.3;
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



    float width, height;
	gInit.GetDimensions(width, height);

	// Step 2: calculate dx and dy (UV space for 1 pixel)	
	float dx = 1.0f / width;
	float dy = 1.0f / height;
	// Step 3: Create a double for loop (5 iterations each)
	//		   Inside the loop, calculate the offset in each direction. Make sure not to take every pixel but move by 2 pixels each time
	//			Do a texture lookup using your previously calculated uv coordinates + the offset, and add to the final color
	float4 sum;
	for (int xOffset = -2.0; xOffset <= 2.0; xOffset += 2)
	{
		for (int yOffset = -2.0; yOffset <= 2.0; yOffset += 2)
		{
                float4 colorInit = gInit.Sample(samPoint, float2(input.TexCoord + float2(dx * xOffset, dy * yOffset)));
                float4 saturatedColor = saturate((colorInit - Threshold) / (1 - Threshold));
			    sum += saturatedColor;
		}
	}
	sum /= (3*3);

    return sum;
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