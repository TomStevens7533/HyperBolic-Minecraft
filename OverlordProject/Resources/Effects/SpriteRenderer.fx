float4x4 gTransform : WorldViewProjection;
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

DepthStencilState NoDepth
{
	DepthEnable = FALSE;
};

RasterizerState BackCulling 
{ 
	CullMode = BACK; 
};

//SHADER STRUCTS
//**************
struct VS_DATA
{
	uint TextureId: TEXCOORD0;
	float4 TransformData : POSITION; //PosX, PosY, Depth (PosZ), Rotation
	float4 TransformData2 : POSITION1; //PivotX, PivotY, ScaleX, ScaleY
	float4 Color: COLOR;	
};

struct GS_DATA
{
	float4 Position : SV_POSITION;
	float4 Color: COLOR;
	float2 TexCoord: TEXCOORD0;
};

//VERTEX SHADER
//*************
VS_DATA MainVS(VS_DATA input)
{
	return input;
}

//GEOMETRY SHADER
//***************
void CreateVertex(inout TriangleStream<GS_DATA> triStream, float3 pos, float4 col, float2 texCoord, float rotation, float2 rotCosSin, float2 offset, float2 pivotOffset)
{
	if (rotation != 0)
	{
		//Step 3.
		//Do rotation calculations
		//Transform to origin
		float3 translatedPos = pos;
		translatedPos.x -= offset + pivotOffset;
		translatedPos.y -= offset + pivotOffset;
		//Rotate
		pos.x = (translatedPos.x * rotCosSin.x) - (translatedPos.y * rotCosSin.y);
		pos.y = (translatedPos.y * rotCosSin.x) + (translatedPos.x * rotCosSin.y);
		//Retransform to initial position
		pos.xy += offset;

	}
	else
	{
		//Step 2.
		//No rotation calculations (no need to do the rotation calculations if there is no rotation applied > redundant operations)
		//Just apply the pivot offset
		pos.x -= pivotOffset;
		pos.y -= pivotOffset;
	}

	//Geometry Vertex Output
	GS_DATA geomData = (GS_DATA) 0;
	geomData.Position = mul(float4(pos, 1.0f), gTransform);
	geomData.Color = col;
	geomData.TexCoord = texCoord;
	triStream.Append(geomData);
}

[maxvertexcount(4)]
void MainGS(point VS_DATA vertex[1], inout TriangleStream<GS_DATA> triStream)
{

	// LT----------RT //TringleStrip (LT > RT > LB, LB > RB > RT)
	// |          / |
	// |       /    |
	// |    /       |
	// | /          |
	// LB----------RB

	//Given Data (Vertex Data)
	//Extract the position data from the VS_DATA vertex struct
	float3 position = float3(vertex[0].TransformData.x, vertex[0].TransformData.y, vertex[0].TransformData.z); 
	//Extract the offset data from the VS_DATA vertex struct (initial X and Y position)
	float2 offset = float2(vertex[0].TransformData.x, vertex[0].TransformData.y);
	//Extract the rotation data from the VS_DATA vertex struct
	float rotation = vertex[0].TransformData.w;
	//Extract the pivot data from the VS_DATA vertex struct
	float2 pivot = float2(vertex[0].TransformData2.x, vertex[0].TransformData2.y); 
	//Extract the scale data from the VS_DATA vertex struct
	float2 scale = float2(vertex[0].TransformData2.z, vertex[0].TransformData2.w);
	//Initial Texture Coordinate
	float2 texCoord = vertex[0].TextureId;

	float2 rotCosSin;
	if (rotation == 0)
		rotCosSin = float2(0, 0);
	else
		rotCosSin = float2(cos(rotation), sin(rotation));

	float2 pivotOffset = gTextureSize * scale * pivot;

	//LT
	texCoord = float2(0,0);
	CreateVertex(triStream, position, vertex[0].Color, texCoord, rotation, rotCosSin, offset, pivotOffset); 

	//RT
	texCoord = float2(1, 0);
	position.x += gTextureSize.x * scale.x;
	CreateVertex(triStream, position, vertex[0].Color, texCoord, rotation, rotCosSin, offset, pivotOffset); 

	//LB
	texCoord = float2(0, 1);
	position.x -= gTextureSize.x * scale.x;
	position.y += gTextureSize.y * scale.y;
	CreateVertex(triStream, position, vertex[0].Color, texCoord, rotation, rotCosSin, offset, pivotOffset); 

	//RB
	texCoord = float2(1, 1);
	position.x += gTextureSize.x * scale.x;
	CreateVertex(triStream, position, vertex[0].Color, texCoord, rotation, rotCosSin, offset, gTextureSize * scale * pivot); 
}

//PIXEL SHADER
//************
float4 MainPS(GS_DATA input) : SV_TARGET {

	return gSpriteTexture.Sample(samPoint, input.TexCoord) * input.Color;	
}

// Default Technique
technique11 Default {

	pass p0 {
		SetRasterizerState(BackCulling);
		SetBlendState(EnableBlending,float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetVertexShader(CompileShader(vs_4_0, MainVS()));
		SetGeometryShader(CompileShader(gs_4_0, MainGS()));
		SetPixelShader(CompileShader(ps_4_0, MainPS()));
	}
}
