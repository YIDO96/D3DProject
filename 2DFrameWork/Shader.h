#pragma once

//파일로부터 hlsl을 컴파일해오는 클래스

class Shader
{
    ID3D11InputLayout*      vertexLayout;
    ID3D11VertexShader*     vertexShader;
    ID3D11PixelShader*      pixelShader;

    ID3D11ShaderReflection* reflection;

    ID3D11GeometryShader*   geometryShader;
public:
    VertexType              vertextype;
    string                  file;
    Shader();
    ~Shader();
    void LoadFile(string file);
    void Set();
    void LoadGeometry();
};
