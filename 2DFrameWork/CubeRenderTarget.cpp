#include "framework.h"
ID3D11Buffer* CubeRenderTarget::camBuffer = nullptr;
ID3D11Buffer* CubeRenderTarget::shadowPosBuffer = nullptr;
void CubeRenderTarget::CreateBackBuffer(float width, float height)
{
	DXGI_FORMAT rtvFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	//Create rgb
	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
		desc.Width = width;
		desc.Height = height;
		desc.ArraySize = 6;
		desc.Format = rtvFormat;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
		desc.MipLevels = 1;
		desc.SampleDesc.Count = 1;

		Check(D3D->GetDevice()->CreateTexture2D(&desc, NULL, &rgb));
	}

	//Create rgb Target
	{
		D3D11_RENDER_TARGET_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
		desc.Format = rtvFormat;
		desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.ArraySize = 6;

		Check(D3D->GetDevice()->CreateRenderTargetView(rgb, &desc, &rgbTarget));
	}
	//Create rgb SRV
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		desc.Format = rtvFormat;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		desc.TextureCube.MipLevels = 1;

		Check(D3D->GetDevice()->CreateShaderResourceView(rgb, &desc, &rgbResource));
	}
	DXGI_FORMAT dsvFormat = DXGI_FORMAT_D32_FLOAT;
	//Create depth Texture

	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
		desc.Width = width;
		desc.Height = height;
		desc.ArraySize = 6;
		desc.Format = dsvFormat;
		desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
		desc.MipLevels = 1;
		desc.SampleDesc.Count = 1;

		Check(D3D->GetDevice()->CreateTexture2D(&desc, NULL, &depth));
	}


	//CreateDSV
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
		desc.Format = dsvFormat;
		desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.ArraySize = 6;

		Check(D3D->GetDevice()->CreateDepthStencilView(depth, &desc, &depthTarget));
	}

	this->width = (float)width;
}

void CubeRenderTarget::DeleteBackBuffer()
{
	SafeRelease(rgbTarget);
	SafeRelease(depthTarget);
	SafeRelease(rgbResource);
	//SafeRelease(depthResource);
	SafeRelease(rgb);
	SafeRelease(depth);
}

CubeRenderTarget::CubeRenderTarget(UINT width)
{
	
	CreateBackBuffer(width, width);

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
		D3D->GetDevice()->CreateSamplerState(&SamplerDesc, &sampler);
	}
	nearZ = 0.001f;
	farZ = 10000.0f;
	shadowBias = -0.005f;
	viewport.width = (float)width;
	viewport.height = (float)width;
}

CubeRenderTarget::~CubeRenderTarget()
{
	DeleteBackBuffer();
}

void CubeRenderTarget::CreateStaticMember()
{
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.ByteWidth = sizeof(GSCamBuffer);
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;//상수버퍼
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		HRESULT hr = D3D->GetDevice()->CreateBuffer(&desc, NULL, &camBuffer);
		assert(SUCCEEDED(hr));
	}

	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.ByteWidth = sizeof(Vector4);
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;//상수버퍼
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		HRESULT hr = D3D->GetDevice()->CreateBuffer(&desc, NULL, &shadowPosBuffer);
		assert(SUCCEEDED(hr));
	}

}

void CubeRenderTarget::DeleteStaticMember()
{
	SafeRelease(camBuffer);
}

void CubeRenderTarget::SetTarget(Color clear)
{
	{
		float x = position.x;
		float y = position.y;
		float z = position.z;

		struct LookAt
		{
			Vector3 LookAt;
			Vector3 Up;
		} lookAt[6];

		lookAt[0] = { Vector3(x + 1.0f, y, z), Vector3(0, 1.0f, 0) };
		lookAt[1] = { Vector3(x - 1.0f, y, z), Vector3(0, 1.0f, 0) };
		lookAt[2] = { Vector3(x, y + 1.0f, z), Vector3(0, 0, -1.0f) };
		lookAt[3] = { Vector3(x, y - 1.0f, z), Vector3(0, 0, +1.0f) };
		lookAt[4] = { Vector3(x, y, z + 1.0f), Vector3(0, 1.0f, 0) };
		lookAt[5] = { Vector3(x, y, z - 1.0f), Vector3(0, 1.0f, 0) };

		for (UINT i = 0; i < 6; i++)
		{
			desc.Views[i] =
				Matrix::CreateLookAt(position, lookAt[i].LookAt, lookAt[i].Up);
			desc.Views[i] = desc.Views[i].Transpose();
		}
	}
	desc.Projection =
		Matrix::CreatePerspectiveFieldOfView(PI*0.5f, 1.0f, nearZ, farZ);

	desc.Projection = desc.Projection.Transpose();

	D3D->GetDC()->RSSetViewports(1, viewport.Get11());

	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		D3D->GetDC()->Map(camBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		memcpy_s(mappedResource.pData, sizeof(GSCamBuffer), &desc, sizeof(GSCamBuffer));
		D3D->GetDC()->Unmap(camBuffer, 0);
		D3D->GetDC()->GSSetConstantBuffers(1, 1, &camBuffer);
	}
	D3D->SetRenderTarget(rgbTarget, depthTarget);
	D3D->Clear(clear, rgbTarget, depthTarget);
}

void CubeRenderTarget::SetRGBTexture(int slot)
{
	{
		Vector4 shadowPos;
		shadowPos.x = position.x;
		shadowPos.y = position.y;
		shadowPos.z = position.z;
		shadowPos.w = shadowBias;


		D3D11_MAPPED_SUBRESOURCE mappedResource;
		D3D->GetDC()->Map(shadowPosBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		memcpy_s(mappedResource.pData, sizeof(Vector4), &shadowPos, sizeof(Vector4));
		D3D->GetDC()->Unmap(shadowPosBuffer, 0);
		D3D->GetDC()->PSSetConstantBuffers(4, 1, &shadowPosBuffer);
		D3D->GetDC()->PSSetConstantBuffers(5, 1, &camBuffer);
	}

	D3D->GetDC()->PSSetShaderResources(slot, 1, &rgbResource);
	D3D->GetDC()->PSSetSamplers(slot, 1, &sampler);
}

void CubeRenderTarget::ResizeScreen(float width)
{
	if (width < 1)
		return;

	this->width = width;

	viewport.width = (float)width;
	viewport.height = (float)width;

	DeleteBackBuffer();
	CreateBackBuffer(width, width);
}

void CubeRenderTarget::RenderDetail()
{
	if (ImGui::SliderInt("Size", &width, 1, 1000))
	{
		ResizeScreen(width);
	}
	ImGui::SliderFloat("ShadowBias", &shadowBias, -0.1f, 0.1f);
	ImGui::DragFloat("nearZ", &nearZ, 0.05f, 0.00001f);
	ImGui::DragFloat("farZ", &farZ, 0.05f, 0.00001f, 1000000.0f);
}
