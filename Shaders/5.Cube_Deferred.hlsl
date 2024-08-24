#include "Common.hlsl"

struct VertexInput
{
    float4 Position : POSITION0;
	float2 Uv : UV0;
	float3 Normal : NORMAL0;
	float4 Weights : WEIGHTS0;
};
struct PixelInput
{
    float4 Position : SV_POSITION;
	float2 Uv : UV0;
	float4 wPosition : POSITION0;
	float3 Normal : NORMAL0;
	float4 Weights : WEIGHTS0;
};
struct DeferredOutput
{
	float4 diffuse : SV_TARGET0;
	float4 specular : SV_TARGET1;
	float4 normal : SV_TARGET2;
    float4 emissive : SV_TARGET3;
    float4 ambient : SV_TARGET4;
};
PixelInput VS(VertexInput input)
{
   
    PixelInput output;
	output.Uv = input.Uv;
    //  o           =  i X W
    output.Position = mul(input.Position, World);
	output.wPosition = output.Position;
	output.Position = mul(output.Position, View);
	output.Position = mul(output.Position, Proj);
	
	
	output.Normal = mul(input.Normal, (float3x3) World);
	output.Weights = input.Weights;
    return output;
}



DeferredOutput PS(PixelInput input)
{
	DeferredOutput output;
	
	
	float4 BaseColor = 0;
	BaseColor.a = 1.0f;
	  [flatten]
	if (Ka.a)
		BaseColor.rgb += TextureN.Sample(SamplerN, input.Uv).rgb * input.Weights.x;
	  [flatten]
	if (Kd.a)
		BaseColor.rgb += TextureD.Sample(SamplerD, input.Uv).rgb * input.Weights.y;
      [flatten]
	if (Ks.a)
		BaseColor.rgb += TextureS.Sample(SamplerS, input.Uv).rgb * input.Weights.z;
      [flatten]
	if (Ke.a)
		BaseColor.rgb += TextureE.Sample(SamplerE, input.Uv).rgb * input.Weights.w;
	
	output.diffuse = BaseColor;
	
	
	
    output.specular = float4(1, 1, 1, Shininess);
    output.normal = float4(normalize(input.Normal)*0.5f + 0.5f,1);
    output.emissive = float4(EmissiveMapping(output.diffuse.xyz, input.Uv, input.Normal, input.wPosition.xyz), 1);
    output.ambient = float4(Ka.rgb * output.diffuse.rgb, 1);
	
	output.diffuse.rgb *= Kd.rgb;
	output.specular.rgb *= Ks.rgb;
	
	return output;
}