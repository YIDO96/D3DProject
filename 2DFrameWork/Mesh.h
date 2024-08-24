#pragma once
class Mesh
{
    friend class GameObject;
    friend class Rain;
    friend class Pop;
private:
    ID3D11Buffer*           vertexBuffer;
    ID3D11Buffer*           indexBuffer;
    ID3D11Buffer*           instanceBuffer = nullptr;

public:
    void*                   vertices;
    UINT*                   indices;
    Matrix*                 instance;

    UINT                    vertexCount;
    UINT                    indexCount;
    UINT                    instanceCount = 0;


    D3D_PRIMITIVE_TOPOLOGY  primitiveTopology;
    VertexType              vertexType;
    UINT                    byteWidth;
    string                  file;
    string                  instanceFile;
public:
    static shared_ptr<Mesh> CreateMesh();

    Mesh();
    Mesh(void* vertices, UINT vertexCount, UINT* indices,
        UINT indexCount, VertexType type);
    void CreateInstanceBuffer(Matrix* instance, UINT instanceCount);
    ~Mesh();
    void Set();
    void LoadFile(string file);
    void SaveFile(string file);
    void                RenderDetail();
    const Vector3& GetVertexPosition(UINT idx);
    void UpdateBuffer();
    void DrawCall();
    void LoadInstanceFile(string file);
    void SaveInstanceFile(string file);
    void InstanceEdit();
    /*void Reset();
    
    */
};
