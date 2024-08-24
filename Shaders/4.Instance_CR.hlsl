#include "Common.hlsl"

struct VertexInput
{
    float4 Position : POSITION0;
	float2 Uv : UV0;
	float3 Normal : NORMAL0;
	float3 Tangent : TANGENT0;
	float4 Indices : INDICES0;
	float4 Weights : WEIGHTS0;
	
	matrix transform : INSTANCE_TRANSFORM;
};
struct GeometryInput
{
    float4 Position : SV_POSITION;
	float2 Uv : UV0;
	float4 wPosition : POSITION0;
	float3 Normal : NORMAL0;
	float3 Tangent : TANGENT;
	float3 Binormal : BINORMAL;
};
struct PixelInput
{
	uint TargetIndex : SV_RenderTargetArrayIndex;
	float4 Position : SV_POSITION;
	float2 Uv : UV0;
	float4 wPosition : POSITION0;
	float3 Normal : NORMAL0;
	float3 Tangent : TANGENT;
	float3 Binormal : BINORMAL;
};

GeometryInput VS(VertexInput input)
{
   
	GeometryInput output;
	output.Uv = input.Uv;
    //  o           =  i X W
  
	//output.Position = mul(input.Position, input.transform);
	
	Matrix world;
    [branch]
	if (input.Weights.x)
		world = SkinWorld(input.Indices, input.Weights);
	else
		world = World;
		
	output.Position = mul(input.Position, world);
	
	output.Position.x += input.transform._41;
	output.Position.y += input.transform._42;
	output.Position.z += input.transform._43;
	
	
	output.wPosition = output.Position;
	//output.Position = mul(output.Position, View);
	//output.Position = mul(output.Position, Proj);
	output.Normal = mul(input.Normal, (float3x3) world);
	output.Tangent = mul(input.Tangent, (float3x3) world);
	output.Binormal = cross(output.Normal.xyz, output.Tangent.xyz);
    return output;
}

[maxvertexcount(18)]
void GS(triangle GeometryInput input[3], inout TriangleStream<PixelInput> stream)
{
	int vertex = 0;
	PixelInput output;
    
    [unroll(6)]
	for (int i = 0; i < 6; i++)
	{
		output.TargetIndex = i;
        [unroll(3)]
		for (vertex = 0; vertex < 3; vertex++)
		{
			output.Position = input[vertex].Position;
			output.Position = mul(output.Position, CubeViews[i]);
			output.Position = mul(output.Position, CubeProjection);
			output.Uv = input[vertex].Uv;
			
			output.wPosition = input[vertex].wPosition;
			output.Normal = input[vertex].Normal;
			output.Tangent = input[vertex].Tangent;
			output.Binormal = input[vertex].Binormal;
			stream.Append(output);
		}
		stream.RestartStrip();
	}
}

float4 PS(PixelInput input) : SV_TARGET
{
	float4 BaseColor = Lighting(DiffuseMapping(input.Uv), input.Uv,
	NormalMapping(input.Normal, input.Tangent, input.Binormal, input.Uv),
	input.wPosition.xyz);
	return BaseColor;
}