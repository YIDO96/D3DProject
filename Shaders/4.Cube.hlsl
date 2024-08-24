#include "Common.hlsl"

struct VertexInput
{
    float4 Position : POSITION0;
	float2 Uv : UV0;
	float3 Normal : NORMAL0;
	float3 Tangent : TANGENT0;
	float4 Indices : INDICES0;
	float4 Weights : WEIGHTS0;
	
	
};
struct PixelInput
{
    float4 Position : SV_POSITION;
	float2 Uv : UV0;
	float4 wPosition : POSITION0;
	float3 Normal : NORMAL0;
	float3 Tangent : TANGENT;
	float3 Binormal : BINORMAL;
};

PixelInput VS(VertexInput input)
{
   
    PixelInput output;
	output.Uv = input.Uv;
    //  o           =  i X W
  
	Matrix world;
    [branch]
	if (input.Weights.x)
		world = SkinWorld(input.Indices, input.Weights);
	else
		world = World;
		
	output.Position = mul(input.Position, world);
	
	
	output.wPosition = output.Position;
	output.Position = mul(output.Position, View);
	output.Position = mul(output.Position, Proj);
	output.Normal = mul(input.Normal, (float3x3) world);
	output.Tangent = mul(input.Tangent, (float3x3) world);
	output.Binormal = cross(output.Normal.xyz, output.Tangent.xyz);
    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
	float4 BaseColor = Lighting(DiffuseMapping(input.Uv), input.Uv,
	NormalMapping(input.Normal, input.Tangent, input.Binormal, input.Uv),
	input.wPosition.xyz);
	
	
	return BaseColor;
}