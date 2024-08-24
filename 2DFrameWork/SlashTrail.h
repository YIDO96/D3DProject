#pragma once
class SlashTrail
{
    shared_ptr<Shader>      shader;
 

    //mesh ¸â¹ö
    vector<VertexPT>        vertices;
    ID3D11Buffer*           vertexBuffer;
    D3D_PRIMITIVE_TOPOLOGY  primitiveTopology;
    VertexType              vertexType;
    UINT                    byteWidth;

    Vector3                 lastTop;
    Vector3                 lastBottom;
    float                   time;
public:
    shared_ptr<Material>    material;
    GameObject* Top = nullptr;
    GameObject* Bottom = nullptr;
    float       interval;       //»ý¼º °£°Ý
    int         maxTrail;       //»ç°¢Çü °¹¼ö
    bool        isPlaying;

    SlashTrail();
    ~SlashTrail();
    void Play();
    void Stop();
    void Update(); //ÀÜ»ó¸Þ½Ã °»½Å
    void Render(); //ÀÜ»ó¸Þ½Ã ·»´õ
    void RenderDetail();
};

