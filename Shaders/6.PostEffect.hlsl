#include "Common.hlsl"

cbuffer PS_PostEffect : register(b11)
{
	int _Filter;
	float3 _Color;
    
	float2 _Screen;
	float _Radius;
	int count;
    
	int select;
	float width;
	float height;
	float _Padding;
}
SamplerState SamplerPost
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Clamp;
	AddressV = Clamp;
};
static const float2 arr[8] =
{
	float2(-1, -1), float2(0, -1), float2(1, -1),
    float2(-1, 0), float2(1, 0),
    float2(-1, 1), float2(0, 1), float2(1, 1)
};

static const float weights[13] =
{
	0.0561f, 0.1353f, 0.2780f, 0.4868f, 0.7261f, 0.9231f,
    1.0f,
    0.9231f, 0.7261f, 0.4868f, 0.2780f, 0.1353f, 0.0561f
};


float4 CrossBlur(float2 uv)
{
	float4 color = 0;
    
	for (int i = 0; i < count; i++)
	{
		float divX = (1 + i) / width;
		float divY = (1 + i) / height;
        
		color += TextureD.Sample(SamplerPost, float2(uv.x + divX, uv.y));
		color += TextureD.Sample(SamplerPost, float2(uv.x - divX, uv.y));
		color += TextureD.Sample(SamplerPost, float2(uv.x, uv.y + divY));
		color += TextureD.Sample(SamplerPost, float2(uv.x, uv.y - divY));
	}

	color /= count * 4;
    
	return color;
}

float4 OctaBlur(float2 uv)
{
	float4 color = 0;
    
	for (int i = 0; i < count; i++)
	{
		float divX = (1 + i) / width;
		float divY = (1 + i) / height;
        
		for (int j = 0; j < 8; j++)
		{
			float x = arr[j].x * divX + uv.x;
			float y = arr[j].y * divY + uv.y;
            
			color += TextureD.Sample(SamplerPost, float2(x, y));
		}
	}

	color /= count * 8;
    
	return color;
}

float4 GaussianBlur(float2 uv)
{
	float divX = 1.0f / width;
	float divY = 1.0f / height;
    
	float sum = 0;
	float4 color = 0;
    
	for (int i = -6; i <= 6; i++)
	{
		float2 temp = uv + float2(divX * i * count, 0);
		color += weights[6 + i] * TextureD.Sample(SamplerPost, temp);
        
		temp = uv + float2(0, divY * i * count);
		color += weights[6 + i] * TextureD.Sample(SamplerPost, temp);
        
		sum += weights[6 + i] * 2;
	}

	color /= sum;
    
	return color;
}

float4 Mosaic(float2 uv)
{
	int x = (int) (uv.x * count);
	int y = (int) (uv.y * count);
    
	float2 temp;
	temp.x = (float) x / count;
	temp.y = (float) y / count;
    
	return TextureD.Sample(SamplerPost, temp);
}

float4 RadialBlur(float2 uv)
{
	float2 radiusUV = uv - float2(0.5f, 0.5f);
	float r = length(radiusUV);
	radiusUV /= r;
    
	r = saturate(2 * r / width);
    
	float2 delta = -radiusUV * r * r * height / count;
    
	float4 color = 0;
    
	for (int i = 0; i < count; i++)
	{
		color += TextureD.Sample(SamplerPost, uv);
		uv += delta;
	}
    
	color /= count;

	return color;
}

float4 Outline(float2 uv)
{
	float sum = 0;
    
	for (int i = 1; i <= count; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			float2 temp = float2(1.0f / width * i, 1.0f / height * i);
			temp *= arr[j];
            
			float4 albedo = TextureD.Sample(SamplerPost, uv + temp);
            
			sum += albedo.x;
		}
	}

	sum /= count * 8;
    
	if (sum > 0.0f && sum < 1.0f)
		return TextureD.Sample(SamplerPost, uv) * 0.9f;
    
	return TextureD.Sample(SamplerPost, uv);
}

float4 Water(float2 uv)
{
	uv.y = uv.y + (sin(uv.y * count) * 0.03);
	uv.x = uv.x + (sin(uv.x * count) * 0.03);
    
	float4 Color = TextureD.Sample(SamplerPost, uv);
    
	return Color;
    
}

struct VertexInput
{
	float4 Position : POSITION0;
	float2 Uv : UV0;
};
struct PixelInput
{
	float4 Position : SV_POSITION;
	float2 Uv : UV0;
};

PixelInput VS(VertexInput input)
{
   
	PixelInput output;
	output.Uv = input.Uv;
    //  o           =  i X W
	output.Position = mul(input.Position, World);
	return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
	
	float2 uv = input.Uv;
    
	float4 BaseColor;
    
	
	//블러 
	//매핑될 색깔의 좌표를 수정
	
	if (select == 0)
		BaseColor = TextureD.Sample(SamplerPost, input.Uv);
	else if (select == 1)
		BaseColor = CrossBlur(input.Uv);
	else if (select == 2)
		BaseColor = OctaBlur(input.Uv);
	else if (select == 3)
		BaseColor = GaussianBlur(input.Uv);
	else if (select == 4)
		BaseColor = Mosaic(input.Uv);
	else if (select == 5)
		BaseColor = RadialBlur(input.Uv);
	else if (select == 6)
		BaseColor = Outline(input.Uv);
	else if (select == 7)
		BaseColor = Water(input.Uv);
    
    
    //샘플링한 rgb를 바꾸는 효과
    
	//모노
	if (_Filter == 1)
	{
		BaseColor.rgb = (BaseColor.r + BaseColor.g + BaseColor.b) / 3.0f;
	}
	//세피아
	else if (_Filter == 2)
	{
		float3 temp = (BaseColor.r + BaseColor.g + BaseColor.b) / 3.0f;
		BaseColor.r = dot(temp.rgb, float3(0.393f, 0.769f, 0.189f));
		BaseColor.g = dot(temp.rgb, float3(0.349f, 0.686f, 0.168f));
		BaseColor.b = dot(temp.rgb, float3(0.272f, 0.534f, 0.131f));
	}
	//색계단
	else if (_Filter == 3)
	{
		BaseColor.r *= count;
		BaseColor.g *= count;
		BaseColor.b *= count;
        
		BaseColor.r = floor(BaseColor.r);
		BaseColor.g = floor(BaseColor.g);
		BaseColor.b = floor(BaseColor.b);
        
		BaseColor.r /= count;
		BaseColor.g /= count;
		BaseColor.b /= count;
	}
	//색반전
	else if (_Filter == 4)
	{
        
		BaseColor.rgb = (1.0f - BaseColor.rgb);
	}
	//조명효과
	BaseColor.rgb += (_Color * 2 - 1);
    
	float dis = length(input.Position.xy - _Screen);
	if (dis > _Radius)
	{
		discard;
	}
    
    
	return BaseColor;
}