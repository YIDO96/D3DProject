#pragma once
class Deferred
{

private:
    RenderTarget* diffuseTarget;
    RenderTarget* specularTarget;
    RenderTarget* normalTarget;
    RenderTarget* ambientTarget;
    RenderTarget* emissiveTarget;
    //RenderTarget* posTarget;

    RenderTarget* rtvs[5];
    ID3D11ShaderResourceView* srvs[6];

    ID3D11Buffer*       buffer;
    shared_ptr<Shader>  shader;
public:
    Deferred(UINT width = App.GetWidth(),
        UINT height = App.GetHeight());
    ~Deferred();
    void SetTarget(Color clear = Color(0, 0, 0, 0));
    void SetRGBTexture(int slot);
    //창사이즈 조절
    void ResizeScreen(float width, float height);
    void RenderDetail();
    void Render();
};

