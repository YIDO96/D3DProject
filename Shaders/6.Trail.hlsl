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
	output.Position = mul(input.Position, View);
	output.Position = mul(output.Position, Proj);
    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    float4 BaseColor = float4(1, 1, 1, 1);
    if(Kd.a)
    {
        BaseColor.a = TextureD.Sample(SamplerD, input.Uv).r;
    }

    BaseColor.rgb += Kd.rgb * 2.0f - 1.0f; // 0.5가 디폴트
    BaseColor = saturate(BaseColor);
    
    return BaseColor;
}