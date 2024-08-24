#include "Framework.h"

void Direct3D11::Create()
{
	//EnumerateAdapters
	{
		IDXGIFactory1* factory;
		FAILED(CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&factory));

		UINT index = 0;
		while (true)
		{
			IDXGIAdapter1* adapter;
			HRESULT hr = factory->EnumAdapters1(index, &adapter);

			if (hr == DXGI_ERROR_NOT_FOUND)
				break;

			Check(hr);

			D3DEnumAdapterInfo* adapterInfo = new D3DEnumAdapterInfo();
			ZeroMemory(adapterInfo, sizeof(D3DEnumAdapterInfo));
			adapterInfo->adapterOrdinal = index;
			adapter->GetDesc1(&adapterInfo->adapterDesc);
			adapterInfo->adapter = adapter;


			IDXGIOutput* output = NULL;

			hr = adapterInfo->adapter->EnumOutputs(0, &output);
			if (DXGI_ERROR_NOT_FOUND == hr)
				break;

			D3DEnumOutputInfo* outputInfo = new D3DEnumOutputInfo();
			ZeroMemory(outputInfo, sizeof(D3DEnumOutputInfo));

			hr = output->GetDesc(&outputInfo->outputDesc);
			Check(hr);

			outputInfo->output = output;

			UINT num_modes = 0;
			DXGI_MODE_DESC* displayModes = nullptr;
			DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;

			hr = output->GetDisplayModeList(format, DXGI_ENUM_MODES_INTERLACED, &num_modes, nullptr); // 모드 개수 가져오기
			Check(hr);

			displayModes = new DXGI_MODE_DESC[num_modes];
			hr = output->GetDisplayModeList(format, DXGI_ENUM_MODES_INTERLACED, &num_modes, displayModes); // 디스플레이 모드 가져오기
			Check(hr);


			for (UINT i = 0; i < num_modes; i++)
			{
				bool isCheck = true;
				isCheck &= displayModes[i].Width == App.width;
				isCheck &= displayModes[i].Height == App.height;

				if (isCheck == true)
				{
					outputInfo->numerator = displayModes[i].RefreshRate.Numerator;
					outputInfo->denominator = displayModes[i].RefreshRate.Denominator;
				}
			}

			adapterInfo->outputInfo = outputInfo;

			SafeDeleteArray(displayModes);

			adapterInfos.push_back(adapterInfo);

			index++;
		}
		SafeRelease(factory);
	}
	//CreateSwapChainAndDevice
	{
		DXGI_SWAP_CHAIN_DESC swapChainDesc = { 0 };
		swapChainDesc.BufferCount = 1;
		swapChainDesc.BufferDesc.Width = 0;
		swapChainDesc.BufferDesc.Height = 0;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		if (App.vSync == true)
		{
			swapChainDesc.BufferDesc.RefreshRate.Numerator = adapterInfos[0]->outputInfo->numerator;
			swapChainDesc.BufferDesc.RefreshRate.Denominator = adapterInfos[0]->outputInfo->denominator;
		}
		else
		{
			swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
			swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		}

		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.OutputWindow = App.handle;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.Windowed = !App.fullScreen;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Flags = 0;

		UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
		//#if defined(_DEBUG)
		//		creationFlags = D3DCOMPILE_PREFER_FLOW_CONTROL | D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
		//#endif
		D3D_FEATURE_LEVEL featureLevels[] =
		{
			D3D_FEATURE_LEVEL_11_1,
		};

		UINT maxVideoMemory = 0;
		int selectedAdapterIndex = -1;
		for (UINT i = 0; i < adapterInfos.size(); ++i)
		{
			if (adapterInfos[i]->adapterDesc.DedicatedVideoMemory > maxVideoMemory)
			{
				selectedAdapterIndex = i;
				maxVideoMemory = (UINT)adapterInfos[i]->adapterDesc.DedicatedVideoMemory;
			}
		}

		numerator = adapterInfos[0]->outputInfo->numerator;
		denominator = adapterInfos[0]->outputInfo->denominator;

		HRESULT hr = D3D11CreateDeviceAndSwapChain
		(
			adapterInfos[selectedAdapterIndex]->adapter
			, D3D_DRIVER_TYPE_UNKNOWN
			, NULL
			, creationFlags
			, featureLevels
			, 1
			, D3D11_SDK_VERSION
			, &swapChainDesc
			, &swapChain
			, &device
			, NULL
			, &deviceContext
		);
		assert(SUCCEEDED(hr));
	}

	CreateBackBuffer(App.width, App.height);
	//ResizeScreen(App.width, App.height);
	isCreated = true;

	HRESULT hr2 = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	Check(hr2);
}

Direct3D11::Direct3D11()
	:numerator(0), denominator(1), isCreated(0), backBuffer(nullptr), device(nullptr)
	, deviceContext(nullptr), renderTargetView(nullptr), swapChain(nullptr)
{

}

Direct3D11::~Direct3D11()
{


	DeleteBackBuffer();

	if (swapChain != nullptr)
		swapChain->SetFullscreenState(false, NULL);

	SafeRelease(deviceContext);
	SafeRelease(device);
	SafeRelease(swapChain);
	SafeRelease(blendState);
}

void Direct3D11::SetRenderTarget(ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsv)
{
	if (rtv == nullptr)
		rtv = renderTargetView;

	if (dsv == nullptr)
		dsv = depthStencilView;

	deviceContext->OMSetRenderTargets(1, &rtv, dsv);

}

void Direct3D11::Clear(Color color, ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsv)
{
	if (rtv == nullptr)
		rtv = renderTargetView;

	if (dsv == nullptr)
		dsv = depthStencilView;

	deviceContext->ClearRenderTargetView(rtv, color);
	deviceContext->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 1);

}

void Direct3D11::Present()
{
	//수직동기화를 결정
	swapChain->Present(App.vSync == true ? 1 : 0, 0);
}

void Direct3D11::ResizeScreen(float width, float height)
{
	if (width < 1 || height < 1)
		return;

	App.width = width;
	App.height = height;
	DeleteBackBuffer();
	DWRITE->DeleteBackBuffer();
	{
		HRESULT hr = swapChain->ResizeBuffers(0, (UINT)width, (UINT)height, DXGI_FORMAT_UNKNOWN, 0);
		assert(SUCCEEDED(hr));
	}
	CreateBackBuffer(width, height);
	DWRITE->CreateBackBuffer(width, height);
}

void Direct3D11::CreateBackBuffer(float width, float height)
{
	HRESULT hr;

	//Create RTV - System BackBuffer
	{
		ID3D11Texture2D* backbufferPointer;
		hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backbufferPointer);
		Check(hr)

			hr = device->CreateRenderTargetView(backbufferPointer, NULL, &renderTargetView);
		Check(hr)

			SafeRelease(backbufferPointer);
	}

	//Create Texture - DSV용
	{
		D3D11_TEXTURE2D_DESC desc = { 0 };
		desc.Width = (UINT)width;
		desc.Height = (UINT)height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		hr = device->CreateTexture2D(&desc, NULL, &backBuffer);
		assert(SUCCEEDED(hr));
	}
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
		desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipSlice = 0;

		hr = device->CreateDepthStencilView(backBuffer, &desc, &depthStencilView);
		assert(SUCCEEDED(hr));

		SetRenderTarget(renderTargetView, depthStencilView);
	}
}

void Direct3D11::DeleteBackBuffer()
{
	SafeRelease(depthStencilView);
	SafeRelease(renderTargetView);
	SafeRelease(backBuffer);
}

D3DEnumAdapterInfo::D3DEnumAdapterInfo()
	: adapter(nullptr), outputInfo(nullptr)
{
}

D3DEnumAdapterInfo::~D3DEnumAdapterInfo()
{
	SafeRelease(adapter);
	SafeDelete(outputInfo);
}


D3DEnumOutputInfo::D3DEnumOutputInfo()
	: output(nullptr), numerator(0), denominator(1)
{
}

D3DEnumOutputInfo::~D3DEnumOutputInfo()
{
	SafeRelease(output);
}
