#include "Common.hlsl"

struct VertexInput
{
    float4 Position : POSITION0;
};
struct PixelInput
{
    float4 Position : SV_POSITION;
	float3 Uv : UV0;
};

PixelInput VS(VertexInput input)
{
   
    PixelInput output;
    output.Position = input.Position;
	output.Uv = normalize(input.Position);
    //  o           =  i X W
    output.Position.xyz = mul(output.Position.xyz, (float3x3) World);
	output.Position.xyz = mul(output.Position.xyz, (float3x3) View);
    output.Position = mul(output.Position, Proj);
    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
	float4 BaseColor = float4(0.38f, 0.63f, 0.94f, 1);
	float4 BaseColor2 = float4(0.08f, 0.15f, 0.24f, 1);
    
	float3 Nor = normalize(input.Uv);
       //광원이 바라보는 방향
	float3 Light = normalize(DirLight.xyz);
      //광원 위치
	float d = saturate(dot(-Light, Nor));
    d = pow(d, 80.0f);
    
	if (Light.y < 0.0f)
	{
         // -1 ~ 0
        
		BaseColor = BaseColor * -Light.y + BaseColor2 * (1.0f  + Light.y);

	}
    else
	{
		BaseColor = BaseColor2;

	}
		BaseColor.rgb += float3(d, d, d);
    
   
    
		return BaseColor;
	}