
cbuffer VS_W : register(b0)
{
	matrix World;
}
cbuffer VS_V : register(b1)
{
	matrix View;
}
cbuffer VS_P : register(b2)
{
	matrix Proj;
}
//���̷��� (�ִϸ��̼Ǳ���)
#define MAX_BONE 256
cbuffer VS_Bones : register(b3)
{
	matrix Bones[MAX_BONE];
}

cbuffer VS_ClipPlane : register(b4)
{
	float4 ClipPlane;
}



Texture2D TextureN : register(t0);
SamplerState SamplerN : register(s0);

Texture2D TextureD : register(t1);
SamplerState SamplerD : register(s1);

Texture2D TextureS : register(t2);
SamplerState SamplerS : register(s2);

Texture2D TextureE : register(t3);
SamplerState SamplerE : register(s3);

TextureCube TextureBG : register(t4);
SamplerState SamplerBG : register(s4);

TextureCube TextureShadow : register(t5);
SamplerState SamplerShadow : register(s5);

//����ī�޶�
cbuffer PS_InvView : register(b0)
{
    matrix InvView;
}

cbuffer PS_Proj : register(b6)
{
    matrix PSProj;
}

cbuffer PS_Material : register(b1)
{
	float4 Ka;
	float4 Kd;
	float4 Ks;
	float4 Ke;
	float Shininess;
	float Opacity;
	float roughness;
	float metallic;
	float shadow;
	float3 matPadding;
}
cbuffer PS_DirLight : register(b2)
{
	float4 DirLight;
	float4 DirColor;
}


#define MAX_LIGHT 16
struct Light
{
    //4
	int isActive;
	int Type;
	float Inner;
	float Outer;
	//
	int Size;
	float3 Direction;
    //4
	float3 Position;
	float Radius;
    //4
	float4 Color;
};
cbuffer PS_Lights : register(b3)
{
	Light lights[MAX_LIGHT];
}
//�׸���ĸ��ī�޶�
cbuffer PS_ShadowViewPos : register(b4)
{
	float4 ShadowViewPos;
}
cbuffer PS_ShadowView : register(b5)
{
	matrix ShadowViews[6];
	matrix ShadowProjection;
}



cbuffer GS_P : register(b0)
{
	matrix GSProj;
}
cbuffer GS_CubeMap : register(b1)
{
	matrix CubeViews[6];
	matrix CubeProjection;
};


matrix SkinWorld (float4 indices, float4 weights)
{
	matrix transform = 0; //�����
	transform += mul(weights.x, Bones[(uint) indices.x]);
	transform += mul(weights.y, Bones[(uint) indices.y]);
	transform += mul(weights.z, Bones[(uint) indices.z]);
	transform += mul(weights.w, Bones[(uint) indices.w]);
	
	
	return transform;
}


float4 DiffuseMapping(float2 Uv)
{
    [flatten]
	if (Kd.a)
		return TextureD.Sample(SamplerD, Uv);
    
	return float4(1, 1, 1, 1);
}
float3 SpecularMapping(float2 Uv)
{
    [flatten]
	if (Ks.a)
		return TextureS.Sample(SamplerS, Uv).rgb;
    
	return float3(1, 1, 1);
}
float3 NormalMapping(float3 N, float3 T, float3 B, float2 Uv)
{
	T = normalize(T);
	B = normalize(B);
	N = normalize(N);
    
    [flatten]
	if (Ka.a)
	{
        
        // 130,130,255 (�⺻��) -> 0,0,1
        // 0.5,0.5,1.0
		float3 normal = TextureN.Sample(SamplerN, Uv).rgb;
        //float3 normal = float3(0.5, 0.5, 1.0);
        
        //������ �������� ȸ������� ����
		float3x3 TBN = float3x3(T, B, N);
        
        // rgb (0~1,0~1,0~1) *2
        // rgb (0~2,0~2,0~2) -1
        // rgb (-1~1,-1~1,-1~1)
		N = normal * 2.0f - 1.0f;
        //������ ������ ȸ����Ű��  normal * Matrix
		N = mul(N, TBN);
	}
    
    return normalize(N);
}


float3 EmissiveMapping(float3 BaseColor, float2 Uv, float3 Normal, float3 wPosition)
{
    //Emissive
	float3 Emissive = 0;
	float3 EmissiveMap = BaseColor;
    float3 ViewDir = normalize(wPosition - InvView._41_42_43);
    //�ݻ���� �������� Emissive ���� ����Ѵ�.
    [flatten]
    //rgb�߿� ���� �ϳ��� 0 �� �ƴϸ�
	if (any(Ke.rgb))
	{
        //���
		float emissive = 1.0f - saturate(dot(Normal, -ViewDir));
		emissive = smoothstep(0.0f, 1.0f, emissive);
		Emissive = emissive * Ke.rgb;
	}
    //EmissiveMap�� �����Ҷ� �ؽ��� ������ �����ش�.
    [flatten]
	if (Ke.a)
	{
		EmissiveMap = TextureE.Sample(SamplerE, Uv).rgb;
	}
	Emissive *= EmissiveMap;
	return saturate(Emissive);
}


float3 DirLighting(float3 BaseColor, float3 SpecularMap, float3 Normal, float3 wPostion)
{
	float3 DirectionLight = normalize(DirLight.xyz);
	float Diffuse = saturate(dot(-DirectionLight, Normal));
    
	float3 RecflectLight = reflect(DirectionLight, Normal);
    float3 ViewDir = normalize(InvView._41_42_43 - wPostion);
	float Specular = saturate(dot(ViewDir, RecflectLight));
	Specular = pow(Specular, Shininess);
    
    //         ��� * ���͸��� *  DiffuseMap
	float3 D = Diffuse * Kd.rgb * BaseColor;
	float3 S = Specular * Ks.rgb * SpecularMap;
	return saturate((D + S) * DirColor.rgb);
}

float3 DeferredDirLighting(float3 BaseColor, float4 SpecularMap, float3 Normal, float3 wPostion)
{
	float3 DirectionLight = normalize(DirLight.xyz);
	float Diffuse = saturate(dot(-DirectionLight, Normal));
    
	float3 RecflectLight = reflect(DirectionLight, Normal);
	float3 ViewDir = normalize(InvView._41_42_43 - wPostion);
	float Specular = saturate(dot(ViewDir, RecflectLight));
    Specular = pow(Specular, SpecularMap.a);
    
    //         ��� * ���͸��� *  DiffuseMap
	float3 D = Diffuse * BaseColor;
	float3 S = Specular * SpecularMap;
	return saturate((D + S) * DirColor.rgb);
}


float3 PointLighting(float3 BaseColor, float3 SpecularMap, float3 Normal, float3 wPosition, int idx)
{
    //return float3(1, 1, 1);
	float3 DirectionLight = lights[idx].Position - wPosition;
	float distanceToLight = length(DirectionLight);
	DirectionLight /= distanceToLight;
    
	float distanceToLightNormal = 1.0f - saturate(distanceToLight / lights[idx].Radius);
	float attention = distanceToLightNormal * distanceToLightNormal;
    
    //���ǰ��
	float Diffuse = saturate(dot(DirectionLight, Normal)) * attention;
  
    
	float3 RecflectLight = reflect(DirectionLight, Normal);
    float3 ViewDir = normalize(InvView._41_42_43 - wPosition);
	float Specular = saturate(dot(ViewDir, RecflectLight)) * attention;
	Specular = pow(Specular, Shininess);
    
	float3 D = Diffuse * Kd.rgb * BaseColor;
	float3 S = Specular * Ks.rgb * SpecularMap;
    
	return saturate((D + S) * lights[idx].Color.rgb);
}
float3 DeferredPointLighting(float3 BaseColor, float4 SpecularMap, float3 Normal, float3 wPosition, int idx)
{
    //return float3(1, 1, 1);
	float3 DirectionLight = lights[idx].Position - wPosition;
	float distanceToLight = length(DirectionLight);
	DirectionLight /= distanceToLight;
    
	float distanceToLightNormal = 1.0f - saturate(distanceToLight / lights[idx].Radius);
	float attention = distanceToLightNormal * distanceToLightNormal;
    
    //���ǰ��
	float Diffuse = saturate(dot(DirectionLight, Normal)) * attention;
  
    
	float3 RecflectLight = reflect(DirectionLight, Normal);
    float3 ViewDir = normalize(InvView._41_42_43 - wPosition);
	float Specular = saturate(dot(ViewDir, RecflectLight)) * attention;
    Specular = pow(Specular, SpecularMap.a);
    
	float3 D = Diffuse * BaseColor;
	float3 S = Specular * SpecularMap;
    
	return saturate((D + S) * lights[idx].Color.rgb);
}
float3 SpotLighting(float3 BaseColor, float3 SpecularMap, float3 Normal, float3 wPosition, int idx)
{
    //�ȼ����� ����Ʈ���ϴ� ����
	float3 DirectionLight = lights[idx].Position - wPosition;
	float distanceToLight = length(DirectionLight);
	DirectionLight /= distanceToLight;
    
	float distanceToLightNormal = 1.0f - saturate(distanceToLight / lights[idx].Radius);
	float attention = distanceToLightNormal * distanceToLightNormal;
    
	float cosAngle = saturate(dot(normalize(-lights[idx].Direction),
    DirectionLight));
    
    
    // 0 ~ 90 -> 1 ~ 0 
	float outer = cos(radians(lights[idx].Outer));
    // 0 ~ 90   -> 1 ~ ����
	float inner = 1.0f / cos(radians(lights[idx].Inner));
    
	//                    1~0        -1
	cosAngle = saturate((cosAngle - outer) * inner);
	attention *= cosAngle;
    
    //���ǰ��
	float Diffuse = saturate(dot(DirectionLight, Normal)) * attention;
  
    
	float3 RecflectLight = reflect(DirectionLight, Normal);
    float3 ViewDir = normalize(InvView._41_42_43 - wPosition);
	float Specular = saturate(dot(ViewDir, RecflectLight)) * attention;
	Specular = pow(Specular, Shininess);
    
	float3 D = Diffuse * Kd.rgb * BaseColor;
	float3 S = Specular * Ks.rgb * SpecularMap;
    
	return saturate((D + S) * lights[idx].Color.rgb);
}

float3 DeferredSpotLighting(float3 BaseColor, float4 SpecularMap, float3 Normal, float3 wPosition, int idx)
{
    //�ȼ����� ����Ʈ���ϴ� ����
	float3 DirectionLight = lights[idx].Position - wPosition;
	float distanceToLight = length(DirectionLight);
	DirectionLight /= distanceToLight;
    
	float distanceToLightNormal = 1.0f - saturate(distanceToLight / lights[idx].Radius);
	float attention = distanceToLightNormal * distanceToLightNormal;
    
	float cosAngle = saturate(dot(normalize(-lights[idx].Direction),
    DirectionLight));
    
    
    // 0 ~ 90 -> 1 ~ 0 
	float outer = cos(radians(lights[idx].Outer));
    // 0 ~ 90   -> 1 ~ ����
	float inner = 1.0f / cos(radians(lights[idx].Inner));
    
	//                    1~0        -1
	cosAngle = saturate((cosAngle - outer) * inner);
	attention *= cosAngle;
    
    //���ǰ��
	float Diffuse = saturate(dot(DirectionLight, Normal)) * attention;
  
    
	float3 RecflectLight = reflect(DirectionLight, Normal);
    float3 ViewDir = normalize(InvView._41_42_43 - wPosition);
	float Specular = saturate(dot(ViewDir, RecflectLight)) * attention;
    Specular = pow(Specular, SpecularMap.a);
    
	float3 D = Diffuse  * BaseColor;
	float3 S = Specular  * SpecularMap;
    
	return saturate((D + S) * lights[idx].Color.rgb);
}

float3 EnvironmentMapping(float3 Normal, float3 wPosition)
{
	 [flatten]
	if (roughness != 0.0f)
	{
        float3 ViewDir = normalize(wPosition - InvView._41_42_43);
		float3 reflection = reflect(ViewDir, Normal);
		return TextureBG.Sample(SamplerBG, reflection.xyz) * roughness;
		
	}
	return float3(0, 0, 0);
}

float4 AddShadow(float4 BaseColor, float4 wPosition)
{
	float factor = 0.0f;
	if (shadow != 0.0f)
	{
		float3 Dir = wPosition.xyz - ShadowViewPos.xyz;
		float TextureZ = TextureShadow.Sample(SamplerShadow, Dir.xyz).r;
		for (int i = 0; i < 6; i++)
		{
			wPosition.w = 1.0f;
			float4 ShadowNdc = mul(wPosition, ShadowViews[i]);
			ShadowNdc = mul(ShadowNdc, ShadowProjection);
			//ndc�� ��ȯ
			ShadowNdc.xyz /= ShadowNdc.w;
			ShadowNdc.z /= ShadowNdc.w;
			//�������� �ȿ� ������
			[flatten]
			if (-1.0f < ShadowNdc.x && ShadowNdc.x < 1.0f &&
			-1.0f < ShadowNdc.y && ShadowNdc.y < 1.0f &&
			0.0f < ShadowNdc.z && ShadowNdc.z < 1.0f)
			{
				// 0 �׸��� ���� 1 �׸��� ����
				
				float CurrentDepth = ShadowNdc.z - ShadowViewPos.w;
				factor = TextureZ >= CurrentDepth;
				break;
			}
		}
	}
	if (factor)
	{
		BaseColor.rgb = saturate(BaseColor.rgb * (1.0f - shadow));
	}
	return BaseColor;
}
float4 Lighting(float4 BaseColor, float2 Uv, float3 Normal, float3 wPosition)
{
	float3 SpecularMap = SpecularMapping(Uv);
	
	float4 Result = float4(DirLighting(BaseColor.rgb, SpecularMap,
    Normal, wPosition),
    BaseColor.a);
	
	if (Result.a > Opacity)
		Result.a = Opacity;
	
	//���� ����ŭ
	//�ݺ�

	for (int i = 0; i < lights[0].Size; i++)
	{
        [flatten]
		if (!lights[i].isActive)
			continue;
        
        [flatten]
		if (lights[i].Type == 0)
		{
			Result.rgb += PointLighting(BaseColor.rgb, SpecularMap,
			Normal, wPosition, i);
		}
		else if (lights[i].Type == 1)
		{
			Result.rgb += SpotLighting(BaseColor.rgb, SpecularMap,
            Normal, wPosition, i);
		}
	}
	
	//�߱�
	//Ambient
	Result.rgb += Ka.rgb * BaseColor.rgb;
    //Emissive
	Result.rgb += EmissiveMapping(BaseColor.rgb, Uv, Normal, wPosition);
	//Environment
	Result.rgb += EnvironmentMapping(Normal, wPosition);
	
	return saturate(Result);


}

