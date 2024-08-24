#include "Common.hlsl"
struct VertexInput
{
    float4 Position : POSITION0;
    float4 Color : COLOR0;
};
struct PixelInput
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR0;
};

PixelInput VS(VertexInput input)
{
   
    PixelInput output;
    //  o           =  i X W
    output.Position = mul(input.Position, World);
	output.Position = mul(output.Position, View);
	output.Position = mul(output.Position, Proj);
    output.Color = input.Color;
    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    //          »ö
    return float4(1, 1, 0, 1);
}