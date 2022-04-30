float4x4 gTransform : WORLDVIEWPROJECTION;
Texture2D gSpriteTexture;
float2 gTextureSize;

SamplerState samPoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = WRAP;
    AddressV = WRAP;
};

BlendState EnableBlending 
{     
	BlendEnable[0] = TRUE;
	SrcBlend = SRC_ALPHA;
    DestBlend = INV_SRC_ALPHA;
};

RasterizerState BackCulling 
{ 
	CullMode = BACK; 
};

//SHADER STRUCTS
//**************
struct VS_DATA
{
	int Channel : TEXCOORD2; //Texture Channel
	float3 Position : POSITION; //Left-Top Character Quad Starting Position
	float4 Color: COLOR; //Color of the vertex
	float2 TexCoord: TEXCOORD0; //Left-Top Character Texture Coordinate on Texture
	float2 CharSize: TEXCOORD1; //Size of the character (in screenspace)
};

struct GS_DATA
{
	float4 Position : SV_POSITION; //HOMOGENEOUS clipping space pos
	float4 Color: COLOR; //Color of the vertex
	float2 TexCoord: TEXCOORD0; //Texcoord of the vertex
	int Channel: TEXCOORD1; //Channel of the vertex
};

//VERTEX SHADER
//*************
VS_DATA MainVS(VS_DATA input)
{
	return input;
}

//GEOMETRY SHADER
//***************
void CreateVertex(inout TriangleStream<GS_DATA> triStream, float3 pos, float4 col, float2 texCoord, int channel)
{
	//Create a new GS_DATA object
	//Fill in all the fields
    GS_DATA newData = (GS_DATA) 0;
    newData.Position = mul(float4(pos, 1.0f), gTransform);
    newData.Color = col;
    newData.TexCoord = texCoord;
    newData.Channel = channel;

	//Append it to the TriangleStream
    triStream.Append(newData);
}

[maxvertexcount(4)]
void MainGS(point VS_DATA vertex[1], inout TriangleStream<GS_DATA> triStream)
{
	//extract data
    float3 pos = float3(vertex[0].Position);
    float2 texCoord = float2(vertex[0].TexCoord); 
    float4 color = float4(vertex[0].Color);
    int channel = vertex[0].Channel;
    float2 charSize = vertex[0].CharSize;


	//Create a Quad using the character information of the given vertex
	//Note that the Vertex.CharSize is in screenspace, TextureCoordinates aren't ;) [Range 0 > 1]


   	//1. Vertex Left-Top
	//CreateVertex(...);
    CreateVertex(triStream, pos, color, texCoord, channel);
	//2. Vertex Right-Top
	//...
    float3 topLeftPos =  pos;
	topLeftPos.x +=	charSize.x;
	
    float2 topLeftPosTexCoord = texCoord;
	topLeftPosTexCoord.x += (charSize.x / gTextureSize.x);
    CreateVertex(triStream, topLeftPos, color, topLeftPosTexCoord, channel);
	//3. Vertex Left-Bottom
	//...
    float3 bottomLeft = pos;
	bottomLeft.y += charSize.y;
    float2 bottomLeftTexCoord = texCoord;
	bottomLeftTexCoord.y += (charSize.y / gTextureSize.y);
    CreateVertex(triStream, bottomLeft, color, bottomLeftTexCoord, channel);
	//4. Vertex Right-Bottom
	//...
    float3 bottomRight = pos;
	bottomRight.x +=  charSize.x;
	bottomRight.y +=  charSize.y;

    float2 bottomRightTexCoord = texCoord;
	bottomRightTexCoord.x +=  (charSize.x / gTextureSize.x);
	bottomRightTexCoord.y +=  (charSize.y / gTextureSize.y);
    CreateVertex(triStream, bottomRight, color, bottomRightTexCoord, channel);
}

//PIXEL SHADER
//************
float4 MainPS(GS_DATA input) : SV_TARGET {
	
	//Sample the texture and return the correct channel [Vertex.Channel]
	//You can iterate a float4 just like an array, using the index operator
	//Also, don't forget to colorize ;) [Vertex.Color]
    return gSpriteTexture.Sample(samPoint, input.TexCoord)[input.Channel] * input.Color;

}

// Default Technique
technique10 Default {

	pass p0 {
		SetRasterizerState(BackCulling);
		SetBlendState(EnableBlending,float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetVertexShader(CompileShader(vs_4_0, MainVS()));
		SetGeometryShader(CompileShader(gs_4_0, MainGS()));
		SetPixelShader(CompileShader(ps_4_0, MainPS()));
	}
}
