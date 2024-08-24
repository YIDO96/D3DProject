#include "Framework.h"

Shader::Shader() :
    vertexLayout(nullptr), vertexShader(nullptr)
    , pixelShader(nullptr), geometryShader(nullptr)
{

}

Shader::~Shader()
{
    SafeRelease(vertexLayout);
    SafeRelease(vertexShader);
    SafeRelease(pixelShader);
    SafeRelease(geometryShader);
}

void Shader::LoadFile(string file)
{
    this->file = file;
    //컴파일결과를 담아놓는 인터페이스
    ID3D10Blob* VsBlob;
    ID3D10Blob* PsBlob;

    wstring path = L"../Shaders/" + Utility::ToWString(file);

    DWORD flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG;


    D3DCompileFromFile(path.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "VS", "vs_5_0", flags, 0, &VsBlob, nullptr);

    D3D->GetDevice()->CreateVertexShader(VsBlob->GetBufferPointer(), VsBlob->GetBufferSize(),
        nullptr, &vertexShader);

    {
        D3DReflect(VsBlob->GetBufferPointer(), VsBlob->GetBufferSize(),
            IID_ID3D11ShaderReflection, (void**)&reflection);

        D3D11_SHADER_DESC shaderDesc;
        reflection->GetDesc(&shaderDesc);

        vector<D3D11_INPUT_ELEMENT_DESC> layouts;

        for (UINT i = 0; i < shaderDesc.InputParameters; i++)
        {
            D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
            reflection->GetInputParameterDesc(i, &paramDesc);

            D3D11_INPUT_ELEMENT_DESC elementDesc;
            elementDesc.SemanticName = paramDesc.SemanticName;
            elementDesc.SemanticIndex = paramDesc.SemanticIndex;
            elementDesc.InputSlot = 0;
            elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
            elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
            elementDesc.InstanceDataStepRate = 0;

            if (paramDesc.Mask < 2)
            {
                if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
                    elementDesc.Format = DXGI_FORMAT_R32_UINT;
                else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
                    elementDesc.Format = DXGI_FORMAT_R32_SINT;
                else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
                    elementDesc.Format = DXGI_FORMAT_R32_FLOAT;
            }
            else if (paramDesc.Mask < 4)
            {
                if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
                    elementDesc.Format = DXGI_FORMAT_R32G32_UINT;
                else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
                    elementDesc.Format = DXGI_FORMAT_R32G32_SINT;
                else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
                    elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
            }
            else if (paramDesc.Mask < 8)
            {
                if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
                    elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
                else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
                    elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
                else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
                    elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
            }
            else if (paramDesc.Mask < 16)
            {
                if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
                    elementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
                else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
                    elementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
                else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
                    elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
            }

            string temp = paramDesc.SemanticName;
            if (temp == "POSITION")
                elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;

            int n = temp.find_first_of("_");

            temp = temp.substr(0, n);

            if (temp == "INSTANCE")
            {
                elementDesc.InputSlot = 1;
                elementDesc.InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
                elementDesc.InstanceDataStepRate = 1;
            }

            layouts.push_back(elementDesc);
        }

        D3D->GetDevice()->CreateInputLayout(layouts.data(), layouts.size(),
            VsBlob->GetBufferPointer(), VsBlob->GetBufferSize(),
            &vertexLayout);
    }


    D3DCompileFromFile(path.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "PS", "ps_5_0", flags, 0, &PsBlob, nullptr);

    D3D->GetDevice()->CreatePixelShader(PsBlob->GetBufferPointer(), PsBlob->GetBufferSize(),
        nullptr, &pixelShader);

    SafeRelease(VsBlob);
    SafeRelease(PsBlob);
}


void Shader::LoadGeometry()
{
    SafeRelease(geometryShader);

    ID3D10Blob* GsBlob;

    wstring path = L"../Shaders/" + Utility::ToWString(file);

    DWORD flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG;


    D3DCompileFromFile(path.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "GS", "gs_5_0", flags, 0, &GsBlob, nullptr);

    D3D->GetDevice()->CreateGeometryShader(GsBlob->GetBufferPointer(), GsBlob->GetBufferSize(),
        nullptr, &geometryShader);

    SafeRelease(GsBlob);
}

void Shader::Set()
{
    D3D->GetDC()->VSSetShader(vertexShader, 0, 0);
    D3D->GetDC()->PSSetShader(pixelShader, 0, 0);
    D3D->GetDC()->GSSetShader(geometryShader, 0, 0);
    D3D->GetDC()->IASetInputLayout(vertexLayout);
}
