#include "framework.h"
ID3D11Buffer* Skeleton::bonesBuffer = nullptr;
//Skeleton::Skeleton(Skeleton& src)
//{
//    file = src.file;
//    memcpy_s(bonesOffset, sizeof(Matrix) * MAX_BONE, src.bonesOffset, sizeof(Matrix) * MAX_BONE);
//}
void Skeleton::CreateStaticMember()
{
    D3D11_BUFFER_DESC desc = { 0 };
    desc.ByteWidth = sizeof(Matrix) * MAX_BONE;
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;//상수버퍼
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    desc.MiscFlags = 0;
    desc.StructureByteStride = 0;
    HRESULT hr = D3D->GetDevice()->CreateBuffer(&desc, NULL, &bonesBuffer);
    D3D->GetDC()->VSSetConstantBuffers(3, 1, &bonesBuffer);
    assert(SUCCEEDED(hr));
}

void Skeleton::DeleteStaticMember()
{
    SafeRelease(bonesBuffer);
}

void Skeleton::Set()
{
    {
        D3D11_MAPPED_SUBRESOURCE mappedResource;
        D3D->GetDC()->Map(bonesBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        memcpy_s(mappedResource.pData, sizeof(Matrix) * MAX_BONE, &bones, sizeof(Matrix) * MAX_BONE);
        D3D->GetDC()->Unmap(bonesBuffer, 0);
    }
}

void Skeleton::LoadFile(string file)
{
    this->file = file;
    BinaryReader in;
    wstring path = L"../Contents/Skeleton/" + Utility::ToWString(file);
    in.Open(path);
    for (int i = 0; i < MAX_BONE; i++)
    {
        bonesOffset[i] = in.matrix();
    }
    in.Close();
}

void Skeleton::SaveFile(string file)
{
    this->file = file;
    BinaryWriter out;
    wstring path = L"../Contents/Skeleton/" + Utility::ToWString(file);
    out.Open(path);
    for (int i = 0; i < MAX_BONE; i++)
    {
        out.matrix(bonesOffset[i]);
    }
    out.Close();
}
