#include "Common.hlsl"

cbuffer VS_Data : register(b10)
{
	float duration;
	float time;
	float gravity;
	float padding1;
}

struct VertexInput
{
    float4 Position : POSITION0;
    float2 Size : SIZE0;
    float3 Velocity : VELOCITY0;
};
struct VertexOutput
{
    float4 Position : POSITION0;
    float2 Size : SIZE0;
};

struct PixelInput
{
    float4 Position : SV_POSITION;
    float2 Uv : UV0;
};



VertexOutput VS(VertexInput input)
{
    VertexOutput output;
    
    input.Velocity.y -= gravity * time;
    
    //중심점값
    output.Position = mul(input.Position, World);
    
     //벨로시티값으로 퍼지기
    output.Position.xyz += (input.Velocity * time);
    output.Size = input.Size;
    
    
	output.Position.w = 1.0f;
	output.Position = mul(output.Position, View);
    
    return output;
}

static const float2 TEXCOORD[4] =
{
    float2(0.0f, 1.0f),
    float2(0.0f, 0.0f),
    float2(1.0f, 1.0f),
    float2(1.0f, 0.0f)
};
[maxvertexcount(4)]
void GS(point VertexOutput input[1], inout TriangleStream<PixelInput> output)
{
    //한개의 점을 네개로 나누기
    
    // 월드변환후 뷰 프로젝션변환
    
  float3 up = float3(0, 1, 0);
	float3 forward = float3(0, 0, 1);
	float3 right = float3(1, 0, 0);
    
    float2 halfSize = input[0].Size * 0.5f;
    
    float4 vertices[4];
    //input[0].Position.xyz (기준좌표,중점)
    
    //왼쪽 아래
   // vertices[0] = float4(input[0].Position.xyz - halfSize.x * right - halfSize.y * up, 1.0f);
    vertices[0] = float4(input[0].Position.xyz - right * halfSize.x - up * halfSize.y, 1.0f);
    // 왼 위
    vertices[1] = float4(input[0].Position.xyz - right * halfSize.x + up * halfSize.y, 1.0f);
    // 오 아래
    vertices[2] = float4(input[0].Position.xyz + right * halfSize.x - up * halfSize.y, 1.0f);
    // 오 위
    vertices[3] = float4(input[0].Position.xyz + right * halfSize.x + up * halfSize.y, 1.0f);
    
    PixelInput pixelInput;
    
    [unroll(4)]
    for (int i = 0; i < 4; i++)
    {
        //월드에서 다시 ndc까지 변환
        pixelInput.Position = mul(vertices[i], GSProj);
        pixelInput.Uv = TEXCOORD[i];
        
        output.Append(pixelInput);
    }
    
}

float4 PS(PixelInput input) : SV_TARGET
{
    
    float4 BaseColor = DiffuseMapping(input.Uv);
       
    if (BaseColor.a == 0)
        discard;
    return BaseColor;
}