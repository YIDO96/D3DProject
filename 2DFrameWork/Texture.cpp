#include "framework.h"


Texture::Texture()
{
    Sampler = nullptr;
    srv = nullptr;
    //기본 샘플러 값
    //SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    SamplerDesc.MipLODBias = 0.0f;
    SamplerDesc.MaxAnisotropy = 1;
    SamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    SamplerDesc.MinLOD = -FLT_MAX;
    SamplerDesc.MaxLOD = FLT_MAX;
    CreateSampler();
}

Texture::~Texture()
{
    SafeRelease(srv);
    SafeRelease(Sampler);
}

void Texture::LoadFile(string file)
{
    this->file = file;
    size_t index = file.find_last_of('.');
    //확장자 문자열 자르기
    string format = file.substr(index + 1, file.length());
    wstring path = L"../Contents/Texture/" + Utility::ToWString(file);

    ScratchImage image;
    HRESULT hr;
    if (format == "tga")
        hr = LoadFromTGAFile(path.c_str(), nullptr, image);
    else if (format == "dds")
        hr = LoadFromDDSFile(path.c_str(), DDS_FLAGS_NONE, nullptr, image);
    else
        hr = LoadFromWICFile(path.c_str(), WIC_FLAGS_NONE, nullptr, image);
    Check(hr);

    CreateShaderResourceView(D3D->GetDevice(), image.GetImages(), image.GetImageCount(),
        image.GetMetadata(), &srv);
}


void Texture::CreateSampler()
{
    SafeRelease(Sampler);
    HRESULT hr;

    hr = D3D->GetDevice()->CreateSamplerState(&SamplerDesc, &Sampler);
    Check(hr);
}

void Texture::Set(int slot)
{
    D3D->GetDC()->PSSetShaderResources(slot,// 연결될레지스터 번호 0
        1,//리소스갯수
        &srv);
    D3D->GetDC()->PSSetSamplers(slot, 1, &Sampler);
}

ScratchImage* Texture::GetPixelData(string file)
{
    size_t index = file.find_last_of('.');
    //확장자 문자열 자르기
    string format = file.substr(index + 1, file.length());
    wstring path = L"../Contents/Texture/" + Utility::ToWString(file);

    ScratchImage* image = new ScratchImage();
    HRESULT hr;
    if (format == "tga")
        hr = LoadFromTGAFile(path.c_str(), nullptr, *image);
    else if (format == "dds")
        hr = LoadFromDDSFile(path.c_str(), DDS_FLAGS_NONE, nullptr, *image);
    else
        hr = LoadFromWICFile(path.c_str(), WIC_FLAGS_NONE, nullptr, *image);
    Check(hr);

    return image;
}
