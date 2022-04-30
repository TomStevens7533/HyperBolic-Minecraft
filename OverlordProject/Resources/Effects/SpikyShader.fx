//************
// VARIABLES *
//************
float4x4 m_MatrixWorldViewProj : WORLDVIEWPROJECTION;
float4x4 m_MatrixWorld : WORLD;
float3 m_LightDirection : DIRECTION
<
	string Object="TargetLight";
> = float3(0.577f, -0.577f, 0.577f);

float3 gColorDiffuse : COLOR = float3(1,1,1);

float gSpikeLength
<
	string UIWidget="Slider";
	float UIMin=0.0f;
	float UIMax=0.5f;
	float UIStep=0.0001f;
> = 0.2f;
RasterizerState FrontCulling 
{ 
	CullMode = FRONT; 
};

//**********
// STRUCTS *
//**********
struct VS_DATA
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
};

struct GS_DATA
{
    float4 Position : SV_POSITION;
    float3 Normal : NORMAL;
};

//****************
// VERTEX SHADER *
//****************
VS_DATA MainVS(VS_DATA vsData)
{
	//Step 1.
	//Delete this transformation code and just return the VS_DATA parameter (vsData)
	//Don't forget to change the return type!

  

    return vsData;
}

//******************
// GEOMETRY SHADER *
//******************
void CreateVertex(inout TriangleStream<GS_DATA> triStream, float3 pos, float3 normal, float2 texCoord)
{
	GS_DATA temp = (GS_DATA)0;
	temp.Position = mul(float4(pos,1),m_MatrixWorldViewProj);
	temp.Normal = mul(normal, (float3x3)m_MatrixWorld);
	triStream.Append(temp);

	
	//Step 1. Create a GS_DATA object
	//Step 2. Transform the position using the WVP Matrix and assign it to (GS_DATA object).Position (Keep in mind: float3 -> float4)
	//Step 3. Transform the normal using the World Matrix and assign it to (GS_DATA object).Normal (Only Rotation, No translation!)
	//Step 4. Append (GS_DATA object) to the TriangleStream parameter (TriangleStream::Append(...))
}

float3 getnormal(float3 x0, float3 x1, float3 x2)
{
    float3 v0 = x0 - x2;
    float3 v1 = x1 - x2;
    float3 n = cross(v1, v0);

    return normalize(n);
}
[maxvertexcount(9)]
void SpikeGenerator(triangle VS_DATA vertices[3], inout TriangleStream<GS_DATA> triStream)
{
	//Step 1. Calculate CENTER_POINT
	float3 basePoint = (vertices[0].Position + vertices[1].Position + vertices[2].Position) / 3;
	//Step 2. Calculate Face Normal (Original Triangle)
	float3  spikeNormal = (vertices[0].Normal + vertices[1].Normal + vertices[2].Normal) / 3;
	//Step 3. Offset CENTER_POINT (use gSpikeLength)
	float3 SpikeCenterPoint = basePoint + (gSpikeLength * spikeNormal);
	//Step 4 + 5. Calculate Individual Face Normals (Cross Product of Face Edges) & Create Vertices for every face
	

	
	
	//FACE 3
	float3 edge3Normal = -cross((vertices[2].Position-SpikeCenterPoint), (vertices[0].Position - SpikeCenterPoint));
	 
	CreateVertex(triStream, vertices[2].Position, edge3Normal, float2(0,0));
	CreateVertex(triStream, SpikeCenterPoint, edge3Normal, float2(0,0));
	CreateVertex(triStream, vertices[0].Position, edge3Normal, float2(0,0));
	
	
	triStream.RestartStrip();
	
	//Face 2
	float3 edge2Normal = -cross((vertices[1].Position-SpikeCenterPoint), (vertices[2].Position - SpikeCenterPoint));
	 
	CreateVertex(triStream, vertices[1].Position, edge2Normal, float2(0,0));
	CreateVertex(triStream, SpikeCenterPoint , edge2Normal, float2(0,0));
	CreateVertex(triStream, vertices[2].Position, edge2Normal, float2(0,0));
	triStream.RestartStrip();
	
	
	//FACE1
	

	float3 edge1Normal = -cross((vertices[0].Position-SpikeCenterPoint),  (vertices[1].Position - SpikeCenterPoint));
 

	CreateVertex(triStream, vertices[0].Position, edge1Normal, float2(0,0));
	CreateVertex(triStream, SpikeCenterPoint, edge1Normal, float2(0,0));
	CreateVertex(triStream, vertices[1].Position , edge1Normal, float2(0,0));
	triStream.RestartStrip();

}

//***************
// PIXEL SHADER *
//***************
float4 MainPS(GS_DATA input) : SV_TARGET
{
    input.Normal=normalize(input.Normal);
	float diffuseStrength = max(dot(normalize(m_LightDirection),-input.Normal),0.2f); 
	return float4(gColorDiffuse*diffuseStrength,1.0f);
}

//*************
// TECHNIQUES *
//*************
technique10 Default //FXComposer >> Rename to "technique10 Default"
{
    pass p0
    {
        SetVertexShader(CompileShader(vs_4_0, MainVS()));
        SetGeometryShader(CompileShader(gs_4_0, SpikeGenerator()));
        SetPixelShader(CompileShader(ps_4_0, MainPS()));
    }
}