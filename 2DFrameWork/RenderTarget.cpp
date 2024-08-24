#include "framework.h"

void RenderTarget::CreateBackBuffer(float width, float height, bool stencil)
{
    //Create rgb
    HRESULT hr;
    {
        D3D11_TEXTURE2D_DESC desc = { 0 };
        desc.Width = width;
        desc.Height = height;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
        hr = D3D->GetDevice()->CreateTexture2D(&desc, nullptr, &rgb);
        Check(hr);
    }
    //Create rgb Target
    {
        D3D11_RENDER_TARGET_VIEW_DESC desc = {};
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

        hr = D3D->GetDevice()->CreateRenderTargetView(rgb, &desc, &rgbTarget);
        Check(hr);
    }
    //Create rgb Resource
    {
        D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        desc.Texture2D.MipLevels = 1;

        hr = D3D->GetDevice()->CreateShaderResourceView(rgb, &desc, &rgbResource);
        Check(hr);
    }
    //Create depth Texture
    {
        D3D11_TEXTURE2D_DESC desc;
        ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
        desc.Width = this->width;
        desc.Height = this->height;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = stencil ? DXGI_FORMAT_R24G8_TYPELESS : DXGI_FORMAT_R32_TYPELESS;
        desc.SampleDesc.Count = 1;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
        hr = D3D->GetDevice()->CreateTexture2D(&desc, nullptr, &depth);
        Check(hr);
    }

    {//Create DSV
        D3D11_DEPTH_STENCIL_VIEW_DESC desc;
        ZeroMemory(&desc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
        desc.Format = stencil ? DXGI_FORMAT_D24_UNORM_S8_UINT : DXGI_FORMAT_D32_FLOAT;
        desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        desc.Texture2D.MipSlice = 0;

        hr = D3D->GetDevice()->CreateDepthStencilView(depth, &desc, &depthTarget);
        Check(hr);
    }
    //Create SRV2
    {
        D3D11_SHADER_RESOURCE_VIEW_DESC desc;
        ZeroMemory(&desc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
        desc.Format = stencil ? DXGI_FORMAT_R24_UNORM_X8_TYPELESS : DXGI_FORMAT_R32_FLOAT;
        desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        desc.Texture2D.MipLevels = 1;
        hr = D3D->GetDevice()->CreateShaderResourceView(depth, &desc, &depthResource);
        Check(hr);
    }
    {
        D3D11_SAMPLER_DESC SamplerDesc;

        SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
        SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        SamplerDesc.MipLODBias = 0.0f;
        SamplerDesc.MaxAnisotropy = 1;
        SamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        SamplerDesc.MinLOD = -FLT_MAX;
        SamplerDesc.MaxLOD = FLT_MAX;
        hr = D3D->GetDevice()->CreateSamplerState(&SamplerDesc, &sampler);
    }
}

void RenderTarget::DeleteBackBuffer()
{
    SafeRelease(rgbTarget);
    SafeRelease(depthTarget);
    SafeRelease(rgbResource);
    SafeRelease(depthResource);
    SafeRelease(rgb);
    SafeRelease(depth);
}

RenderTarget::RenderTarget(UINT width, UINT height, bool stencil)
    : width(width), height(height)
{
    CreateBackBuffer(width, height, stencil);
}

RenderTarget::~RenderTarget()
{
    DeleteBackBuffer();
}

void RenderTarget::SetTarget(Color clear)
{
    D3D->SetRenderTarget(rgbTarget, depthTarget);
    D3D->Clear(clear, rgbTarget, depthTarget);
}

void RenderTarget::SetRGBTexture(int slot)
{
    D3D->GetDC()->PSSetShaderResources(slot, 1, &rgbResource);
    D3D->GetDC()->PSSetSamplers(slot, 1, &sampler);
}

void RenderTarget::ResizeScreen(float width, float height)
{
    if (width < 1 || height < 1)
        return;

    this->width = width;
    this->height = height;

    DeleteBackBuffer();
    CreateBackBuffer(width, height);
}
