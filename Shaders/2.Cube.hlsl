#include "Common.hlsl"

struct VertexInput
{
    float4 Position : POSITION0;
	float4 Color : COLOR0;
	float3 Normal : NORMAL0;
};
struct PixelInput
{
    float4 Position : SV_POSITION;
	float4 Color : COLOR0;
    float4 wPosition : POSITION0;
	float3 Normal : NORMAL0;
};

PixelInput VS(VertexInput input)
{
   
    PixelInput output;
	output.Color = input.Color;
    //  o           =  i X W
    output.Position = mul(input.Position, World);
	output.wPosition = output.Position;
	output.Position = mul(output.Position, View);
	output.Position = mul(output.Position, Proj);
	output.Normal = mul(input.Normal, (float3x3) World);
    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
	float4 BaseColor = Lighting(input.Color, float2(1, 1), input.Normal, input.wPosition.xyz);
	return BaseColor;
}