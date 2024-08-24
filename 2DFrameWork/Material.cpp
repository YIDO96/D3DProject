#include "Framework.h"

ID3D11Buffer* Material::materialBuffer = nullptr;

Material::Material()
{
    file = "ChangeName.mtl";
    ambient = Color(0.3f, 0.3f, 0.3f, 0.0f);
    diffuse = Color(1.0f, 1.0f, 1.0f, 0.0f);
    specular = Color(1.0f, 1.0f, 1.0f, 0.0f);
    emissive = Color(0.2f, 0.2f, 0.2f, 0.0f);
    shininess = 30.0f;
    opacity = 1.0f;
    roughness = 0.0f;
    metallic = 0.0f;
    shadow = 0.0f;
    normalMap = nullptr;
    diffuseMap = nullptr;
    emissiveMap = nullptr;
    specularMap = nullptr;
}


Material::~Material()
{
    SafeReset(normalMap);
    SafeReset(diffuseMap);
    SafeReset(specularMap);
    SafeReset(emissiveMap);
}

void Material::CreateStaticMember()
{
    Material::materialBuffer;

    D3D11_BUFFER_DESC desc = { 0 };
    desc.ByteWidth = sizeof(MaterialBuffer);
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;//상수버퍼
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    desc.MiscFlags = 0;
    desc.StructureByteStride = 0;
    HRESULT hr = D3D->GetDevice()->CreateBuffer(&desc, NULL, &materialBuffer);
    assert(SUCCEEDED(hr));
    D3D->GetDC()->PSSetConstantBuffers(1, 1, &materialBuffer);
}

void Material::DeleteStaticMember()
{
    SafeRelease(materialBuffer);
}

void Material::Set()
{
    //상수버퍼
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    D3D->GetDC()->Map(materialBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    memcpy_s(mappedResource.pData, sizeof(MaterialBuffer), (MaterialBuffer*)this, sizeof(MaterialBuffer));
    D3D->GetDC()->Unmap(materialBuffer, 0);


    if (normalMap)normalMap->Set(0);
    if (diffuseMap)diffuseMap->Set(1);
    if (specularMap)specularMap->Set(2);
    if (emissiveMap)emissiveMap->Set(3);
}

void Material::RenderDetail()
{
    ImGui::ColorEdit3("ambient", (float*)&ambient, ImGuiColorEditFlags_PickerHueWheel);
    ImGui::ColorEdit3("diffuse", (float*)&diffuse, ImGuiColorEditFlags_PickerHueWheel);
    ImGui::ColorEdit3("specular", (float*)&specular, ImGuiColorEditFlags_PickerHueWheel);
    ImGui::ColorEdit3("emissive", (float*)&emissive, ImGuiColorEditFlags_PickerHueWheel);
    ImGui::DragFloat("shiness", &shininess, 0.05f);
    ImGui::DragFloat("opacity", &opacity, 0.05f);
    ImGui::DragFloat("roughness", &roughness, 0.05f, 0.0f, 1.0f);
    ImGui::DragFloat("metallic", &metallic, 0.05f, 0.0f, 1.0f);
    ImGui::DragFloat("shadow", &shadow, 0.05f, 0.0f, 1.0f);


    if (normalMap)
        normalMap->RenderDetail();

    if (GUI->FileImGui("Load NormalMap", "Load NormalMap",
        ".dds,.jpg,.tga,.png,.bmp", "../Contents/Texture"))
    {
        string path = ImGuiFileDialog::Instance()->GetCurrentPath();
        Utility::Replace(&path, "\\", "/");
        if (path.find("/Texture/") != -1)
        {
            size_t tok = path.find("/Texture/") + 9;
            path = path.substr(tok, path.length())
                + "/" + ImGuiFileDialog::Instance()->GetCurrentFileName();
        }
        else
        {
            path = ImGuiFileDialog::Instance()->GetCurrentFileName();
        }
        SafeReset(normalMap);
        normalMap = RESOURCE->textures.Load(path);
        ambient.w = 1.0f;
    }

    if (diffuseMap)
        diffuseMap->RenderDetail();

    if (GUI->FileImGui("Load DiffuseMap", "Load DiffuseMap",
        ".dds,.jpg,.tga,.png,.bmp", "../Contents/Texture"))
    {
        string path = ImGuiFileDialog::Instance()->GetCurrentPath();
        Utility::Replace(&path, "\\", "/");
        if (path.find("/Texture/") != -1)
        {
            size_t tok = path.find("/Texture/") + 9;
            path = path.substr(tok, path.length())
                + "/" + ImGuiFileDialog::Instance()->GetCurrentFileName();
        }
        else
        {
            path = ImGuiFileDialog::Instance()->GetCurrentFileName();
        }
        SafeReset(diffuseMap);
        diffuseMap = RESOURCE->textures.Load(path);
        diffuse.w = 1.0f;
    }



    if (specularMap)
        specularMap->RenderDetail();

    if (GUI->FileImGui("Load SpecularMap", "Load SpecularMap",
        ".dds,.jpg,.tga,.png,.bmp", "../Contents/Texture"))
    {
        string path = ImGuiFileDialog::Instance()->GetCurrentPath();
        Utility::Replace(&path, "\\", "/");
        if (path.find("/Texture/") != -1)
        {
            size_t tok = path.find("/Texture/") + 9;
            path = path.substr(tok, path.length())
                + "/" + ImGuiFileDialog::Instance()->GetCurrentFileName();
        }
        else
        {
            path = ImGuiFileDialog::Instance()->GetCurrentFileName();
        }
        SafeReset(specularMap);
        specularMap = RESOURCE->textures.Load(path);
        specular.w = 1.0f;
    }

    if (emissiveMap)
        emissiveMap->RenderDetail();

    if (GUI->FileImGui("Load emissiveMap", "Load emissiveMap",
        ".dds,.jpg,.tga,.png,.bmp", "../Contents/Texture"))
    {
        string path = ImGuiFileDialog::Instance()->GetCurrentPath();
        Utility::Replace(&path, "\\", "/");
        if (path.find("/Texture/") != -1)
        {
            size_t tok = path.find("/Texture/") + 9;
            path = path.substr(tok, path.length())
                + "/" + ImGuiFileDialog::Instance()->GetCurrentFileName();
        }
        else
        {
            path = ImGuiFileDialog::Instance()->GetCurrentFileName();
        }
        SafeReset(emissiveMap);
        emissiveMap = RESOURCE->textures.Load(path);
        emissive.w = 1.0f;
    }
}

void Material::LoadFile(string file)
{
    this->file = file;
    BinaryReader in;
    wstring path = L"../Contents/Material/" + Utility::ToWString(file);
    in.Open(path);

    ambient = in.color4f();
    diffuse = in.color4f();
    specular = in.color4f();
    emissive = in.color4f();

    if (in.Int())
    {
        normalMap = RESOURCE->textures.Load(in.String());
        normalMap->SamplerDesc.Filter = (D3D11_FILTER)in.Int();
        normalMap->SamplerDesc.AddressU = (D3D11_TEXTURE_ADDRESS_MODE)in.Int();
        normalMap->SamplerDesc.AddressV = (D3D11_TEXTURE_ADDRESS_MODE)in.Int();
        normalMap->SamplerDesc.AddressW = (D3D11_TEXTURE_ADDRESS_MODE)in.Int();
        normalMap->CreateSampler();
    }
    if (in.Int())
    {
        diffuseMap = RESOURCE->textures.Load(in.String());
        diffuseMap->SamplerDesc.Filter = (D3D11_FILTER)in.Int();
        diffuseMap->SamplerDesc.AddressU = (D3D11_TEXTURE_ADDRESS_MODE)in.Int();
        diffuseMap->SamplerDesc.AddressV = (D3D11_TEXTURE_ADDRESS_MODE)in.Int();
        diffuseMap->SamplerDesc.AddressW = (D3D11_TEXTURE_ADDRESS_MODE)in.Int();
        diffuseMap->CreateSampler();
    }
    if (in.Int())
    {
        specularMap = RESOURCE->textures.Load(in.String());
        specularMap->SamplerDesc.Filter = (D3D11_FILTER)in.Int();
        specularMap->SamplerDesc.AddressU = (D3D11_TEXTURE_ADDRESS_MODE)in.Int();
        specularMap->SamplerDesc.AddressV = (D3D11_TEXTURE_ADDRESS_MODE)in.Int();
        specularMap->SamplerDesc.AddressW = (D3D11_TEXTURE_ADDRESS_MODE)in.Int();
        specularMap->CreateSampler();
    }
    if (in.Int())
    {
        emissiveMap = RESOURCE->textures.Load(in.String());
        emissiveMap->SamplerDesc.Filter = (D3D11_FILTER)in.Int();
        emissiveMap->SamplerDesc.AddressU = (D3D11_TEXTURE_ADDRESS_MODE)in.Int();
        emissiveMap->SamplerDesc.AddressV = (D3D11_TEXTURE_ADDRESS_MODE)in.Int();
        emissiveMap->SamplerDesc.AddressW = (D3D11_TEXTURE_ADDRESS_MODE)in.Int();
        emissiveMap->CreateSampler();
    }
    shininess = in.Float();
    opacity = in.Float();
    roughness = in.Float();
    metallic = in.Float();
    shadow = in.Float();

    in.Close();

}

void Material::SaveFile(string file)
{
    this->file = file;
    BinaryWriter out;
    wstring path = L"../Contents/Material/" + Utility::ToWString(file);
    out.Open(path);

    out.color4f(ambient);
    out.color4f(diffuse);
    out.color4f(specular);
    out.color4f(emissive);

    if (normalMap)
    {
        out.Int(1);
        out.String(normalMap->file);
        out.Int((D3D11_FILTER)normalMap->SamplerDesc.Filter);
        out.Int((D3D11_TEXTURE_ADDRESS_MODE)normalMap->SamplerDesc.AddressU);
        out.Int((D3D11_TEXTURE_ADDRESS_MODE)normalMap->SamplerDesc.AddressV);
        out.Int((D3D11_TEXTURE_ADDRESS_MODE)normalMap->SamplerDesc.AddressW);
    }
    else
        out.Int(0);

    if (diffuseMap)
    {
        out.Int(1);
        out.String(diffuseMap->file);
        out.Int((D3D11_FILTER)diffuseMap->SamplerDesc.Filter);
        out.Int((D3D11_TEXTURE_ADDRESS_MODE)diffuseMap->SamplerDesc.AddressU);
        out.Int((D3D11_TEXTURE_ADDRESS_MODE)diffuseMap->SamplerDesc.AddressV);
        out.Int((D3D11_TEXTURE_ADDRESS_MODE)diffuseMap->SamplerDesc.AddressW);
    }
    else
        out.Int(0);

    if (specularMap)
    {
        out.Int(1);
        out.String(specularMap->file);
        out.Int((D3D11_FILTER)specularMap->SamplerDesc.Filter);
        out.Int((D3D11_TEXTURE_ADDRESS_MODE)specularMap->SamplerDesc.AddressU);
        out.Int((D3D11_TEXTURE_ADDRESS_MODE)specularMap->SamplerDesc.AddressV);
        out.Int((D3D11_TEXTURE_ADDRESS_MODE)specularMap->SamplerDesc.AddressW);
    }
    else
        out.Int(0);

    if (emissiveMap)
    {
        out.Int(1);
        out.String(emissiveMap->file);
        out.Int((D3D11_FILTER)emissiveMap->SamplerDesc.Filter);
        out.Int((D3D11_TEXTURE_ADDRESS_MODE)emissiveMap->SamplerDesc.AddressU);
        out.Int((D3D11_TEXTURE_ADDRESS_MODE)emissiveMap->SamplerDesc.AddressV);
        out.Int((D3D11_TEXTURE_ADDRESS_MODE)emissiveMap->SamplerDesc.AddressW);
    }
    else
        out.Int(0);



    out.Float(shininess);
    out.Float(opacity);
    out.Float(roughness);
    out.Float(metallic);
    out.Float(shadow);

    out.Close();
}
