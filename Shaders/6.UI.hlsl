#include "Common.hlsl"

struct VertexInput
{
    float4 Position : POSITION0;
	float2 Uv : UV0;
};
struct PixelInput
{
    float4 Position : SV_POSITION;
	float2 Uv : UV0;
};

PixelInput VS(VertexInput input)
{
   
    PixelInput output;
	output.Uv = input.Uv;
    //  o           =  i X W
    output.Position = mul(input.Position, World);
    //output.Position = mul(output.Position, ViewProj);
    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
	float4 result = TextureD.Sample(SamplerD, input.Uv);
    
	if (result.a == 0)
	{
		discard;
	}
	return result;
}