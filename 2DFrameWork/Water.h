#pragma once
class Water : public Actor
{
    friend class GameObject;
private:
    struct WaterBuffer
    {
        Vector2 velocity;
        float time;
        float padding;
    }waterBufferDesc;
    float uvScale = 20.0f;
    ID3D11Buffer* waterBuffer;
    ID3D11Buffer* clipPlaneBuffer;

    //반사될 상 그려줄 렌더타겟
    CubeRenderTarget* reflection;
    CubeRenderTarget* refraction;


public:
    static Water* Create(string name = "Water");
    Water();

    void Update();
    virtual void Render(shared_ptr<Shader> pShader = nullptr);
    void RenderDetail();

    void SetReflectionTarget();
    void SetRefractionTarget();
    void UpdateUv();

};

