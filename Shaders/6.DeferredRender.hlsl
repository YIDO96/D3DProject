#include "Common.hlsl"

struct PixelInput
{
    float4 Position : SV_POSITION;
	float2 Uv : UV0;
};

static const float2 arrBasePos[4] =
{
    float2(-1.0f, +1.0f),
	float2(+1.0f, +1.0f),
	float2(-1.0f, -1.0f),
	float2(+1.0f, -1.0f)
};

static const float2 arrUv[4] =
{
    float2(0.0f, 0.0f),
	float2(+1.0f, 0.0f),
	float2(0.0f, 1.0f),
	float2(+1.0f, 1.0f)
};
PixelInput VS(uint vertexID : SV_VertexID)
{
   
    PixelInput output;
    output.Position = float4(arrBasePos[vertexID].xy, 0.0f, 1.0f);
    output.Uv = output.Position.xy;
	
    return output;
}

Texture2D diffuseTexture : register(t10);
Texture2D specularTexture : register(t11);
Texture2D normalTexture : register(t12);
Texture2D emissiveTexture : register(t13);
Texture2D ambientTexture : register(t14);
Texture2D depthTexture : register(t15);
//Texture2D NDCTexture : register(t13);


SamplerState SamplerDefault
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

float ConvertDepthToLinear(float depth)
{
    float linearDepth = PSProj._43 / (depth - PSProj._33);

    return linearDepth;
}
float3 CalcWorldPos(float2 csPos, float linearDepth)
{
    float4 position;
	
    float2 temp;
    temp.x = 1 / PSProj._11;
    temp.y = 1 / PSProj._22;
    position.xy = csPos.xy * temp * linearDepth;
    position.z = linearDepth;
    position.w = 1.0f;
	
    return mul(position, InvView).xyz;
}
float4 PS(PixelInput input) : SV_TARGET
{
    int3 location3 = int3(input.Position.xy, 0);
	
    float4 BaseColor = diffuseTexture.Load(location3);
    float3 Normal = normalize(normalTexture.Load(location3).xyz * 2.0f - 1.0f);
    float4 Specular = specularTexture.Load(location3);
    float depth = depthTexture.Load(location3).x;
    float linearDepth = ConvertDepthToLinear(depth);
    float3 wPosition = CalcWorldPos(input.Uv, linearDepth);
 
	
    float3 emissive = emissiveTexture.Load(location3).xyz;
    float3 ambient = ambientTexture.Load(location3).xyz;
	
	
	float4 Result = float4(DeferredDirLighting(BaseColor.rgb, Specular,
    Normal, wPosition.xyz),
    BaseColor.a);
	
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
			Result.rgb += DeferredPointLighting(BaseColor.rgb, Specular,
			Normal, wPosition.xyz, i);
        }
		else if (lights[i].Type == 1)
		{
			Result.rgb += DeferredSpotLighting(BaseColor.rgb, Specular,
            Normal, wPosition.xyz, i);
        }
	}
	
	Result.rgb += emissive;
	Result.rgb += ambient;
	
	
	return Result;
}