#include "Common.hlsl"

struct VertexInput
{
    float4 Position : POSITION0;
};
struct PixelInput
{
    float4 Position : SV_POSITION;
};

PixelInput VS(VertexInput input)
{
   
    PixelInput output;
    //  o           =  i X W
    output.Position = mul(input.Position, World);
	output.Position = mul(output.Position, View);
	output.Position = mul(output.Position, Proj);
    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{

    return float4(1, 1, 1, 1);
}