#include "Common.hlsl"

cbuffer PS_Brush : register(b10)
{
	float3 BrushPoint;
	float BrushRange;
	
	float BrushShape;
	float BrushType;
	float2 BrushPadding;
}



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

float4 PS(PixelInput input) : SV_TARGET
{
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
	
	//BaseColor = float4(input.Weights.w, input.Weights.w, input.Weights.w, 1);
	//
	
	float3 SpecularMap = float3(1, 1, 1);
	
	BaseColor = float4(DirLighting(BaseColor.rgb, SpecularMap,
    normalize(input.Normal), input.wPosition.xyz),
    BaseColor.a);
	
	for (int i = 0; i < lights[0].Size; i++)
	{
        [flatten]
		if (!lights[i].isActive)
			continue;
        
        [flatten]
		if (lights[i].Type == 0)
		{
			BaseColor.rgb += PointLighting(BaseColor.rgb, SpecularMap,
			input.Normal, input.wPosition.xyz, i);
		}
		else if (lights[i].Type == 1)
		{
			BaseColor.rgb += SpotLighting(BaseColor.rgb, SpecularMap,
            input.Normal, input.wPosition.xyz, i);
		}
	}
	
	
	BaseColor.rgb += Ka.rgb * BaseColor.rgb;
	BaseColor.rgb += EmissiveMapping(BaseColor.rgb, input.Uv, input.Normal, input.wPosition.xyz);
	
	
	BaseColor.rgb = saturate(BaseColor.rgb);
	
	BaseColor = AddShadow(BaseColor, input.wPosition);
	

	
	
	float3 v1 = float3(BrushPoint.x, 0.0f, BrushPoint.z);
	float3 v2 = float3(input.wPosition.x, 0.0f, input.wPosition.z);
	float Dis = distance(v2, v1);
	float w;
	
	[branch]
	if (BrushShape == 0.0f)
	{
		w = saturate(Dis / BrushRange);
		w = 1.0f - w;
	}
	else
	{
		float DisX = abs(v1.x - v2.x);
		float DisZ = abs(v1.z - v2.z);
		if (DisX < BrushRange
            && DisZ < BrushRange)
		{
			w = saturate(((DisX > DisZ) ? DisX : DisZ) / BrushRange);
			w = 1.0f - w;
		}
		else
		{
			w = 0.0f;
		}
	}
	if (BrushType == 0)
	{
		BaseColor.rgb += float3(w, 0, 0);
	}
	else if (BrushType == 1)
	{
		BaseColor.rgb += float3(0, w, 0);
	}
	else if(BrushType == 2)
	{
		BaseColor.rgb += float3(0, 0, w);
	}
	else if(BrushType == 3)
	{
		BaseColor.rgb += float3(w, w, 0);
	}
		return BaseColor;
}