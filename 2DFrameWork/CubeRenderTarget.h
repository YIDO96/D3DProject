#pragma once
class CubeRenderTarget
{
private:
    static ID3D11Buffer* camBuffer;
    static ID3D11Buffer* shadowPosBuffer;
    struct GSCamBuffer
    {
        Matrix Views[6];
        Matrix Projection;
    } desc;
   

protected:
    int width;

    ID3D11Texture2D* rgb;
    ID3D11Texture2D* depth;

    ID3D11RenderTargetView* rgbTarget;
    ID3D11DepthStencilView* depthTarget;

    ID3D11ShaderResourceView* rgbResource;

    ID3D11SamplerState* sampler;

    //창사이즈 조절
    void CreateBackBuffer(float width, float height);
    void DeleteBackBuffer();

public:
    Viewport    viewport;
    Vector3     position;       //360도 카메라 중심점 
    float		nearZ , farZ;   //시야공간 끝을 어디까지 정할지
    float       shadowBias;     //그림자 오차범위

    CubeRenderTarget(UINT width = 256);
    ~CubeRenderTarget();

    static void CreateStaticMember();
    static void DeleteStaticMember();

    virtual void SetTarget(Color clear = Color(0, 0, 0, 1));
    virtual void SetRGBTexture(int slot);
    virtual void ResizeScreen(float width);
    void RenderDetail();
};

