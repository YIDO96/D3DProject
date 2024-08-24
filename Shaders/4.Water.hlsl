#include "Common.hlsl"

cbuffer PS_Water : register(b10)
{
	float2 velocity;
	float time; //흐르는 시간 속도
	float padding1;
}
TextureCube WaterReflection : register(t10);
SamplerState WaterReflectionSampler : register(s10);

TextureCube WaterRefraction : register(t11);
SamplerState WaterRefractionSampler : register(s11);

struct VertexInput
{
	float4 Position : POSITION0;
	float2 Uv : UV0;
	float3 Normal : NORMAL0;
	float3 Tangent : TANGENT0;
	//정점변환때만 쓰이는 멤버
	float4 Indices : INDICES0;
	float4 Weights : WEIGHTS0;
};
struct PixelInput
{
	float4 Position : SV_POSITION;
	float3 wPosition : POSITION0;
	float2 Uv : UV0;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
	float3 Binormal : BINORMAL;
};

PixelInput VS(VertexInput input)
{
   
	PixelInput output;
    //output.Color = input.Color;
	output.Uv = input.Uv;
	
	Matrix world;
    [flatten]
	if (input.Weights.x)
		world = SkinWorld(input.Indices, input.Weights);
	else
		world = World;
		
	output.Position = mul(input.Position, world);
	output.wPosition = output.Position.xyz;
	output.Position = mul(output.Position, View);
	output.Position = mul(output.Position, Proj);
	//input.Normal.w = 0.0f;
	output.Normal = mul(input.Normal, (float3x3) world);
	//input.Tangent.w = 0.0f;
	output.Tangent = mul(input.Tangent, (float3x3) world);
	output.Binormal = cross(output.Normal.xyz, output.Tangent.xyz);
    
	return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
	float2 Uv = input.Uv + velocity * time;
	
	
	float4 BaseColor = DiffuseMapping(Uv);
	
	
	float3 MappingNormal = NormalMapping(input.Normal, input.Tangent, input.Binormal, Uv);
	float3 Normal = normalize(input.Normal);
	
	float3 SpecularMap = SpecularMapping(Uv);
	float4 Result = float4(DirLighting(BaseColor.rgb, SpecularMap,
    MappingNormal, input.wPosition),
    BaseColor.a);
	
	if (Result.a > Opacity)
		Result.a = Opacity;
	
	//광원 수만큼
	//반복

	for (int i = 0; i < lights[0].Size; i++)
	{
        [flatten]
		if (!lights[i].isActive)
			continue;
        
        [flatten]
		if (lights[i].Type == 0)
		{
			Result.rgb += PointLighting(BaseColor.rgb, SpecularMap,
			MappingNormal, input.wPosition, i);
		}
		else if (lights[i].Type == 1)
		{
			Result.rgb += SpotLighting(BaseColor.rgb, SpecularMap,
            MappingNormal, input.wPosition, i);
		}
	}
	
	//발광
	//Ambient
	Result.rgb += Ka.rgb * BaseColor.rgb;
    //Emissive
	Result.rgb += EmissiveMapping(BaseColor.rgb, Uv, MappingNormal, input.wPosition);
	//Environment
	
	BaseColor = Result;
	
	//물이 투명할때 (굴절)
	if (Opacity < 1.0f)
	{
		float3 ViewDir =input.wPosition.xyz - ViewPos.xyz;
		ViewDir.xyz += MappingNormal.x * 0.1f;
		float3 refractionRGB = WaterRefraction.Sample(WaterRefractionSampler, ViewDir.xyz).xyz;
		refractionRGB *= (1 - Opacity);
		
		BaseColor.rgb *= Opacity;
		BaseColor.rgb += refractionRGB;

	}
	//거칠기에따라 물반사율
	if (roughness > 0.0f)
	{
		float3 ViewDir = normalize(input.wPosition.xyz - ViewPos.xyz);
		float3 reflection = reflect(ViewDir, Normal);
		reflection.xyz += MappingNormal.x * 0.1f;
		
		float3 reflectionRGB = WaterReflection.Sample(WaterReflectionSampler, reflection.xyz).xyz;
		reflectionRGB *= roughness;
		
		BaseColor.rgb += reflectionRGB;
	}
	
	
		BaseColor.a = 1.0f;
		return BaseColor;

}