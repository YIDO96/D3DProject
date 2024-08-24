#include "Common.hlsl"

struct VertexInput
{
    float4 Position : POSITION0;
	float2 Uv : UV0;
	float3 Normal : NORMAL0;
	float4 Weights : WEIGHTS0;
};
struct GeometryInput
{
    float4 Position : SV_POSITION;
	float2 Uv : UV0;
	float4 wPosition : POSITION0;
	float3 Normal : NORMAL0;
	float4 Weights : WEIGHTS0;
	float Clip : SV_ClipDistance0;
};

struct PixelInput
{
	uint TargetIndex : SV_RenderTargetArrayIndex;
	float4 Position : SV_POSITION;
	float2 Uv : UV0;
	float4 wPosition : POSITION0;
	float3 Normal : NORMAL0;
	float4 Weights : WEIGHTS0;
	float Clip : SV_ClipDistance0;
};

GeometryInput VS(VertexInput input)
{
   
	GeometryInput output;
	output.Uv = input.Uv;
    //  o           =  i X W
    output.Position = mul(input.Position, World);
	output.Clip = dot(output.Position, ClipPlane);
	output.wPosition = output.Position;
	//output.Position = mul(output.Position, View);
	//output.Position = mul(output.Position, Proj);
	output.Normal = mul(input.Normal, (float3x3) World);
	output.Weights = input.Weights;
    return output;
}


[maxvertexcount(18)]
void GS(triangle GeometryInput input[3], inout TriangleStream<PixelInput> stream)
{
	int vertex = 0;
	PixelInput output;
    
    [unroll(6)]
	for (int i = 0; i < 6; i++)
	{
		output.TargetIndex = i;
        [unroll(3)]
		for (vertex = 0; vertex < 3; vertex++)
		{
			output.Position = input[vertex].Position;
			output.Position = mul(output.Position,  CubeViews[i]);
			output.Position = mul(output.Position, CubeProjection);
			output.Uv = input[vertex].Uv;
			
			output.wPosition = input[vertex].wPosition;
			output.Normal = input[vertex].Normal;
			output.Weights = input[vertex].Weights;
			output.Clip = input[vertex].Clip;
			stream.Append(output);
		}
		stream.RestartStrip();
	}
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
	
	
	return BaseColor;
}