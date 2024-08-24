#include "Common.hlsl"

struct VertexInput
{
    float4 Position : POSITION0;
	float2 Mov : MOVE0;
	float2 Size : SIZE0;
};
struct PixelInput
{
    float4 Position : SV_POSITION;
	float2 Uv : UV0;
};

static const float2 TEXCOORD[4] =
{
	float2(0.0f, 1.0f),
    float2(0.0f, 0.0f),
    float2(1.0f, 1.0f),
    float2(1.0f, 0.0f)
};



VertexInput VS(VertexInput input)
{
   
	VertexInput output;
	output.Size = input.Size;
	output.Mov = input.Mov;
    //  o           =  i X W
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    return output;
}

[maxvertexcount(4)]
void GS(point VertexInput input[1], inout TriangleStream<PixelInput> output)
{
	float4 vertices[4];
	
	//// �� �Ʒ�
	vertices[0] = float4(input[0].Position.xy + input[0].Mov, input[0].Position.z, 1.0f);
	// //���� ��
	vertices[1] = float4(vertices[0].x, vertices[0].y + input[0].Size.y, input[0].Position.z, 1.0f);
	// //���� �Ʒ�
	vertices[2] = float4(vertices[0].x + input[0].Size.x, vertices[0].y, input[0].Position.z, 1.0f);
	// //���� ��
	vertices[3] = float4(vertices[0].x + input[0].Size.x, vertices[0].y + input[0].Size.y, input[0].Position.z, 1.0f);
	
		//// �� �Ʒ�
	//vertices[0] = float4(1,1,1,1);
	// //���� ��
	//vertices[1] = float4(1, 1, 1, 1);
	//// //���� �Ʒ�
	//vertices[2] = float4(1, 1, 1, 1);
	//// //���� ��
	//vertices[3] = float4(1, 1, 1, 1);
	
	
	PixelInput pixelInput;
    
    [unroll(4)]
	for (int i = 0; i < 4; i++)
	{
        //���忡�� �ٽ� ndc���� ��ȯ
		pixelInput.Position = mul(vertices[i], GSProj);
		pixelInput.Uv = TEXCOORD[i];
		if (pixelInput.Uv.x == 1.0f)
		{
			pixelInput.Uv.x 
			 = input[0].Size.x;

		}
        
			output.Append(pixelInput);
		}
	
	
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