#pragma once
class Terrain : public Actor
{
    struct InputDesc
    {
        UINT index;
        Vector3 v0, v1, v2;
    };
    struct OutputDesc
    {
        int picked;
        float u, v, distance;
    };
    struct RayDesc
    {
        Vector3 position;
        float size;
        Vector3 direction;
        float padding;
    };
	static ID3D11ComputeShader* computeShader;

public:
	static Terrain* Create(string name = "Terrain");
    static void		CreateStaticMember();
    static void		DeleteStaticMember();
protected:
	Terrain();
	~Terrain();
private:
    //compute Input
    InputDesc* inputArray;
    ID3D11Resource* input;
    ID3D11ShaderResourceView* srv = nullptr;//읽기전용
    //compute Output
    OutputDesc* outputArray;
    ID3D11Resource* output;
    ID3D11UnorderedAccessView* uav;//읽기쓰기 둘다가능
    //copy용
    ID3D11Resource* result;
    //ray
    RayDesc                     ray;
    ID3D11Buffer* rayBuffer;
public:
	int				size;
	float			uvScale;
	int				garo;
    //CS            컴퓨트 쉐이더로 피킹할때만 만들기
    void            CreateStructuredBuffer();
    void            DeleteStructuredBuffer();

	void			CreateMesh(int   garo);
	void			LoadHeightRaw(string file);
	void			LoadHeightImage(string file);
	void			UpdateStructuredBuffer();
	void			UpdateNormal();
	void	        RenderDetail();

    bool            ComPutePicking(Ray WRay, OUT Vector3& HitPoint);
};

