#pragma once
class RenderTarget
{
    friend class Deferred;
protected:

    int width, height;

    ID3D11Texture2D* rgb;    //rgb
    ID3D11Texture2D* depth;  //depth

    //파이프라인 결합할때 타겟으로 쓸 인터페이스
    ID3D11RenderTargetView* rgbTarget;
    ID3D11DepthStencilView* depthTarget;

    //파이프라인 결합할때 텍스쳐로 쓸 인터페이스
    ID3D11ShaderResourceView* rgbResource;
    ID3D11ShaderResourceView* depthResource;

    ID3D11SamplerState* sampler;

    //창사이즈 조절
    void CreateBackBuffer(float width, float height, bool stencil = false);
    void DeleteBackBuffer();

public:
    RenderTarget(UINT width = App.GetWidth(),
        UINT height = App.GetHeight(), bool stencil = false);
    ~RenderTarget();


    void SetTarget(Color clear = Color(0, 0, 0, 1));
    void SetRGBTexture(int slot);

  
    //창사이즈 조절
    void ResizeScreen(float width, float height);
};

