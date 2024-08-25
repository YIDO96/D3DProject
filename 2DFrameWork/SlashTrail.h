#pragma once
class SlashTrail
{
    shared_ptr<Shader>      shader;
 

    //mesh 멤버
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
    float       interval;       //생성 간격
    int         maxTrail;       //사각형 갯수
    bool        isPlaying;

    SlashTrail();
    ~SlashTrail();
    void Play();
    void Stop();
    void Update(); //잔상메시 갱신
    void Render(); //잔상메시 렌더
    void RenderDetail();
};

