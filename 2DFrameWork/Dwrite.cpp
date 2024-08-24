
#include "Framework.h"

Dwrite::Dwrite()
{
    HRESULT hr;

    hr = DWriteCreateFactory
    (
        DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)&writeFactory
    );
    assert(SUCCEEDED(hr));


    D2D1_FACTORY_OPTIONS option;
    option.debugLevel = D2D1_DEBUG_LEVEL_NONE;
    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, &factory);
    assert(SUCCEEDED(hr));

    IDXGIDevice* dxgiDevice;
    hr = D3D->GetDevice()->QueryInterface(&dxgiDevice);
    assert(SUCCEEDED(hr));

    hr = factory->CreateDevice(dxgiDevice, &device);
    assert(SUCCEEDED(hr));

    hr = device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS, &deviceContext);
    assert(SUCCEEDED(hr));


    IDXGISurface* dxgiSurface;
    hr = D3D->GetSwapChain()->GetBuffer(0, __uuidof(IDXGISurface), (void**)&dxgiSurface);
    assert(SUCCEEDED(hr));


    D2D1_BITMAP_PROPERTIES1 bp;
    bp.pixelFormat.format = DXGI_FORMAT_R8G8B8A8_UNORM;
    bp.pixelFormat.alphaMode = D2D1_ALPHA_MODE_IGNORE;
    bp.dpiX = 96;
    bp.dpiY = 96;
    bp.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
    bp.colorContext = NULL;

    hr = deviceContext->CreateBitmapFromDxgiSurface(dxgiSurface, &bp, &targetBitmap);
    assert(SUCCEEDED(hr));
    SafeRelease(dxgiDevice);
    SafeRelease(dxgiSurface);
    deviceContext->SetTarget(targetBitmap);

    FontBrushDesc a;
    FontBrushDesc b;

    //a == b;
}

Dwrite::~Dwrite()
{
    //벡터 사이즈 만큼 삭제
    for (FontBrushDesc desc : fontBrush)
        SafeRelease(desc.brush);

    for (FontTextDesc desc : fontText)
        SafeRelease(desc.format);

    SafeRelease(targetBitmap);

    SafeRelease(factory);
    SafeRelease(writeFactory);

    SafeRelease(deviceContext);
    SafeRelease(device);
}

void Dwrite::CreateBackBuffer(float width, float height)
{
    HRESULT hr;

    hr = device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS, &deviceContext);
    assert(SUCCEEDED(hr));

    IDXGISurface* dxgiSurface;
    hr = D3D->GetSwapChain()->GetBuffer(0, __uuidof(IDXGISurface), (void**)&dxgiSurface);
    assert(SUCCEEDED(hr));


    D2D1_BITMAP_PROPERTIES1 bp;
    bp.pixelFormat.format = DXGI_FORMAT_R8G8B8A8_UNORM;
    bp.pixelFormat.alphaMode = D2D1_ALPHA_MODE_IGNORE;
    bp.dpiX = 96;
    bp.dpiY = 96;
    bp.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
    bp.colorContext = NULL;

    hr = deviceContext->CreateBitmapFromDxgiSurface(dxgiSurface, &bp, &targetBitmap);
    assert(SUCCEEDED(hr));
    SafeRelease(dxgiSurface);

    deviceContext->SetTarget(targetBitmap);
}

void Dwrite::DeleteBackBuffer()
{
    SafeRelease(targetBitmap);
    SafeRelease(deviceContext);
}

void Dwrite::RenderText(wstring text, RECT rect, float size, wstring font, Color color, DWRITE_FONT_WEIGHT weight, DWRITE_FONT_STYLE style, DWRITE_FONT_STRETCH stretch)
{
    FontBrushDesc brushDesc;
    brushDesc.color = color;

    FontBrushDesc* findBrush = NULL;
    //반복문  fontBrush 벡터의 사이즈만큼 반복
    //반복자 desc
    
    for (FontBrushDesc& desc : fontBrush)
    {
        //연산자 재정의 ==  호출
        if (desc == brushDesc)
        {
            findBrush = &desc;
            break;
        }
    }
    //벡터안에 해당 브러시가 없다면
    if (findBrush == NULL)
    {
        D2D1::ColorF colorF = D2D1::ColorF(color.x, color.y, color.z);
        deviceContext->CreateSolidColorBrush(colorF, &brushDesc.brush);

        //해당 브러시를 벡터에 추가(기존에 존재하지 않기때문에)
        fontBrush.push_back(brushDesc);
        findBrush = &brushDesc;
    }

    //매개변수 폰트값 정의
    FontTextDesc textDesc;
    textDesc.font = font;
    textDesc.fontSize = size;
    textDesc.stretch = stretch;
    textDesc.style = style;
    textDesc.weight = weight;

    FontTextDesc* findText = NULL;
    //벡터의 사이즈만큼 반복

    //for (FontTextDesc& desc : fontText)
    for (UINT i = 0; i < fontText.size(); i++)
    {
        //만약 기존 텍스트가 존재 할때
        //if (desc == textDesc)
        if (fontText[i] == textDesc)
        {
            //findText = &desc;
            findText = &fontText[i];
            break;
        }
    }
    //없다면 추가
    if (findText == NULL)
    {
        writeFactory->CreateTextFormat
        (
            textDesc.font.c_str(), NULL, textDesc.weight, textDesc.style, textDesc.stretch, textDesc.fontSize,
            L"ko", &textDesc.format
        );

        fontText.push_back(textDesc);
        findText = &textDesc;
    }

    D2D1_RECT_F rectF;
    rectF.left = (float)rect.left;
    rectF.right = (float)rect.right;
    rectF.top = (float)rect.top;
    rectF.bottom = (float)rect.bottom;


    deviceContext->DrawTextW
    (
        text.c_str(), text.size(), findText->format, rectF, findBrush->brush
    );
}
