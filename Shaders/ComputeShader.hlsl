
//읽기용
struct InputDesc
{
	uint index;
	float3 v0;
	float3 v1;
	float3 v2;
};
StructuredBuffer<InputDesc> input; //srv

//읽기 쓰기용
struct OutputDesc
{
	int picked;
	float u;
	float v;
	float distance;
};
RWStructuredBuffer<OutputDesc> output; //uav

cbuffer Ray : register(b0)
{
	float3 position;
	float outputSize;

	float3 direction;
}

//레이 삼각형 교차 함수
void IntersectTri(uint index)
{
	float3 A = input[index].v0;
	float3 B = input[index].v1;
	float3 C = input[index].v2;
    
    
	float3 e1 = B - A;
	float3 e2 = C - A;

	float3 P, T, Q;
	P = cross(direction, e2);

	float d = 1.0f / dot(e1, P);

	T = position - A;
	output[index].u = dot(T, P) * d;

	Q = cross(T, e1);
	output[index].v = dot(direction, Q) * d;
	output[index].distance = dot(e2, Q) * d;

	bool b = (output[index].u >= 0.0f) &&
        (output[index].v >= 0.0f) &&
        (output[index].u + output[index].v <= 1.0f) &&
        (output[index].distance >= 0.0f);

	output[index].picked = b ? (1 + input[index].index) : 0;
}


[numthreads(32, 32, 1)]
void CS(uint3 groupID : SV_GroupID,
    uint groupIndex : SV_GroupIndex)
{
    //내가 한스레드당 첫번쨰 삼각형부터 계산을했을때
    //내가 지금 접근한 삼각형의 인덱스
	uint index
    //                          0 ~ 1023
        = groupID.x * 32 * 32 + groupIndex;

	if (outputSize > index)
		IntersectTri(index);

}