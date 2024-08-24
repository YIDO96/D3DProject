
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
//스켈레톤 (애니메이션까지)
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

//현재카메라
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
//그림자캡쳐카메라
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
	matrix transform = 0; //영행렬
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
        
        // 130,130,255 (기본값) -> 0,0,1
        // 0.5,0.5,1.0
		float3 normal = TextureN.Sample(SamplerN, Uv).rgb;
        //float3 normal = float3(0.5, 0.5, 1.0);
        
        //세개의 법선으로 회전행렬을 만듦
		float3x3 TBN = float3x3(T, B, N);
        
        // rgb (0~1,0~1,0~1) *2
        // rgb (0~2,0~2,0~2) -1
        // rgb (-1~1,-1~1,-1~1)
		N = normal * 2.0f - 1.0f;
        //매핑한 법선을 회전시키기  normal * Matrix
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
    //반사색이 있을때만 Emissive 값을 계산한다.
    [flatten]
    //rgb중에 값이 하나라도 0 이 아니면
	if (any(Ke.rgb))
	{
        //계수
		float emissive = 1.0f - saturate(dot(Normal, -ViewDir));
		emissive = smoothstep(0.0f, 1.0f, emissive);
		Emissive = emissive * Ke.rgb;
	}
    //EmissiveMap이 존재할때 텍스쳐 색상을 입혀준다.
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
    
    //         계수 * 머터리얼 *  DiffuseMap
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
    
    //         계수 * 머터리얼 *  DiffuseMap
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
    
    //빛의계수
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
    
    //빛의계수
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
    //픽셀에서 라이트향하는 방향
	float3 DirectionLight = lights[idx].Position - wPosition;
	float distanceToLight = length(DirectionLight);
	DirectionLight /= distanceToLight;
    
	float distanceToLightNormal = 1.0f - saturate(distanceToLight / lights[idx].Radius);
	float attention = distanceToLightNormal * distanceToLightNormal;
    
	float cosAngle = saturate(dot(normalize(-lights[idx].Direction),
    DirectionLight));
    
    
    // 0 ~ 90 -> 1 ~ 0 
	float outer = cos(radians(lights[idx].Outer));
    // 0 ~ 90   -> 1 ~ 무한
	float inner = 1.0f / cos(radians(lights[idx].Inner));
    
	//                    1~0        -1
	cosAngle = saturate((cosAngle - outer) * inner);
	attention *= cosAngle;
    
    //빛의계수
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
    //픽셀에서 라이트향하는 방향
	float3 DirectionLight = lights[idx].Position - wPosition;
	float distanceToLight = length(DirectionLight);
	DirectionLight /= distanceToLight;
    
	float distanceToLightNormal = 1.0f - saturate(distanceToLight / lights[idx].Radius);
	float attention = distanceToLightNormal * distanceToLightNormal;
    
	float cosAngle = saturate(dot(normalize(-lights[idx].Direction),
    DirectionLight));
    
    
    // 0 ~ 90 -> 1 ~ 0 
	float outer = cos(radians(lights[idx].Outer));
    // 0 ~ 90   -> 1 ~ 무한
	float inner = 1.0f / cos(radians(lights[idx].Inner));
    
	//                    1~0        -1
	cosAngle = saturate((cosAngle - outer) * inner);
	attention *= cosAngle;
    
    //빛의계수
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
			//ndc로 변환
			ShadowNdc.xyz /= ShadowNdc.w;
			ShadowNdc.z /= ShadowNdc.w;
			//투영공간 안에 있을때
			[flatten]
			if (-1.0f < ShadowNdc.x && ShadowNdc.x < 1.0f &&
			-1.0f < ShadowNdc.y && ShadowNdc.y < 1.0f &&
			0.0f < ShadowNdc.z && ShadowNdc.z < 1.0f)
			{
				// 0 그림자 없음 1 그림자 있음
				
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
			Result.rgb += PointLighting(BaseColor.rgb, SpecularMap,
			Normal, wPosition, i);
		}
		else if (lights[i].Type == 1)
		{
			Result.rgb += SpotLighting(BaseColor.rgb, SpecularMap,
            Normal, wPosition, i);
		}
	}
	
	//발광
	//Ambient
	Result.rgb += Ka.rgb * BaseColor.rgb;
    //Emissive
	Result.rgb += EmissiveMapping(BaseColor.rgb, Uv, Normal, wPosition);
	//Environment
	Result.rgb += EnvironmentMapping(Normal, wPosition);
	
	return saturate(Result);


}

