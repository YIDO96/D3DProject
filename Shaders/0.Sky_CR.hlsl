#include "Common.hlsl"

struct VertexInput
{
    float4 Position : POSITION0;
};

struct GeometryInput
{
	float4 Position : SV_POSITION;
	float3 Uv : UV0;
};


struct PixelInput
{
	uint TargetIndex : SV_RenderTargetArrayIndex;
    float4 Position : SV_POSITION;
    float3 Uv : UV0;
};

GeometryInput VS(VertexInput input)
{
   
	GeometryInput output;
    output.Uv = normalize(input.Position.xyz);
    output.Position = input.Position;
    //  o           =  i X W
    output.Position.xyz = mul(output.Position.xyz, (float3x3) World);
	//output.Position.xyz = mul(output.Position.xyz, (float3x3) View);
 //   output.Position = mul(output.Position, Proj);
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
			output.Position.xyz = mul(output.Position.xyz, (float3x3) (CubeViews[i]));
			output.Position = mul(output.Position, CubeProjection);
			output.Uv = input[vertex].Uv;
			stream.Append(output);
		}
		stream.RestartStrip();
	}
    
    
    
}



float4 PS(PixelInput input) : SV_TARGET
{
    float4 BaseColor = float4(1, 1, 1, 1);
    
   
     //물체에서 나오는 방향(구)
	float3 Nor = normalize(input.Uv.xyz);
    
    
    //큐브맵에서 가져온 색
	BaseColor = TextureBG.Sample(SamplerBG, input.Uv);
    

    
    return BaseColor;
}