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
struct PixelInput
{
    float4 Position : SV_POSITION;
	float2 Uv : UV0;
	float4 wPosition : POSITION0;
	float3 Normal : NORMAL0;
	float3 Tangent : TANGENT;
	float3 Binormal : BINORMAL;
	
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
	
	output.Position = mul(output.Position, View);
	output.Position = mul(output.Position, Proj);
	
	
	output.Normal = mul(input.Normal, (float3x3) world);
	output.Tangent = mul(input.Tangent, (float3x3) world);
	output.Binormal = cross(output.Normal.xyz, output.Tangent.xyz);
    return output;
}

DeferredOutput PS(PixelInput input)
{
	DeferredOutput output;
	
	output.diffuse = DiffuseMapping(input.Uv);
	
	output.specular = float4(SpecularMapping(input.Uv), Shininess);
	output.diffuse.rgb *= Kd.rgb;
	output.specular.rgb *= Ks.rgb;
	
    output.normal = float4(NormalMapping(input.Normal, input.Tangent, input.Binormal, input.Uv) * 0.5f + 0.5f, 1);
    output.emissive = float4(EmissiveMapping(output.diffuse.xyz, input.Uv, input.Normal, input.wPosition.xyz), 1);
	output.ambient = float4(Ka.rgb * output.diffuse.rgb, 1);
	
	return output;
}