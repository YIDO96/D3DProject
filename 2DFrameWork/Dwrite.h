#pragma once

struct FontBrushDesc;
struct FontTextDesc;

class Dwrite :public Singleton<Dwrite>
{

private:
    ID2D1Device* device;
    ID2D1DeviceContext* deviceContext = nullptr;
    ID2D1Factory1* factory;
    IDWriteFactory* writeFactory = nullptr;
    ID2D1Bitmap1* targetBitmap;

    vector<FontBrushDesc> fontBrush;
    vector<FontTextDesc>  fontText;
public:
    Dwrite();
    ~Dwrite();
    ID2D1DeviceContext* GetDC() { return deviceContext; }

    //창사이즈 조절
    void CreateBackBuffer(float width, float height);
    void DeleteBackBuffer();
public:
    //UI안에서 그릴때
    void RenderText
    (
        wstring text
        , RECT rect
        , float size = 20.0f
        , wstring font = L"돋움체"
        , Color color = Color(1, 1, 1, 1)
        , DWRITE_FONT_WEIGHT weight = DWRITE_FONT_WEIGHT_NORMAL
        , DWRITE_FONT_STYLE style = DWRITE_FONT_STYLE_NORMAL
        , DWRITE_FONT_STRETCH stretch = DWRITE_FONT_STRETCH_NORMAL
    );

};


struct FontBrushDesc
{
    Color color;
    ID2D1SolidColorBrush* brush = NULL;
    //연산자 재정의
    bool operator==(const FontBrushDesc& val)
    {
        return color.x == val.color.x and
            color.y == val.color.y and
            color.z == val.color.z and
            color.w == val.color.w;
    }
};

struct FontTextDesc
{
    wstring font;
    float fontSize;

    DWRITE_FONT_WEIGHT weight;
    DWRITE_FONT_STYLE style;
    DWRITE_FONT_STRETCH stretch;

    IDWriteTextFormat* format = nullptr;

    bool operator==(const FontTextDesc& val)
    {
        bool b = true;
        b &= font == val.font;
        b &= fontSize == val.fontSize;
        b &= weight == val.weight;
        b &= style == val.style;
        b &= stretch == val.stretch;
        return b;
    }
};