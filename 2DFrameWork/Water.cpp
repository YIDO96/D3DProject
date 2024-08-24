#include "framework.h"

Water* Water::Create(string name)
{
    Water* temp = new Water();
    temp->name = name;
    //temp->LoadFile("water.xml");
    temp->type = ObType::Water;
    return temp;
}

Water::Water()
{
    reflection = new CubeRenderTarget(512);
    refraction = new CubeRenderTarget(512);
    {
        D3D11_BUFFER_DESC desc = { 0 };
        desc.ByteWidth = sizeof(WaterBuffer);
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;//상수버퍼
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        desc.MiscFlags = 0;
        desc.StructureByteStride = 0;
        HRESULT hr = D3D->GetDevice()->CreateBuffer(&desc, NULL, &waterBuffer);
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
        HRESULT hr = D3D->GetDevice()->CreateBuffer(&desc, NULL, &clipPlaneBuffer);
        assert(SUCCEEDED(hr));
    }
}


void Water::Update()
{
    Actor::Update();
}

void Water::Render(shared_ptr<Shader> pShader)
{
    {
        waterBufferDesc.time = TIMER->GetWorldTime();
        //상수버퍼
        D3D11_MAPPED_SUBRESOURCE mappedResource;
        D3D->GetDC()->Map(waterBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        memcpy_s(mappedResource.pData, sizeof(WaterBuffer), &waterBufferDesc, sizeof(WaterBuffer));
        D3D->GetDC()->Unmap(waterBuffer, 0);
        D3D->GetDC()->PSSetConstantBuffers(10, 1, &waterBuffer);
    }
    reflection->SetRGBTexture(10);
    refraction->SetRGBTexture(11);
    //BLEND->Set(true);
    Actor::Render(pShader);
    //BLEND->Set(false);
}

void Water::RenderDetail()
{
    Actor::RenderDetail();
    if (ImGui::BeginTabBar("MyTabBar3"))
    {
        if (ImGui::BeginTabItem("Water"))
        {
            if (ImGui::SliderFloat("UvScale", &uvScale, 1, 100))
            {
                UpdateUv();
            }
            if (ImGui::SliderFloat2("velocity", (float*)&waterBufferDesc.velocity, -10, 10))
            {

            }


            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}

void Water::SetReflectionTarget()
{
    Vector3 Dir = GetWorldPos() - Camera::main->GetWorldPos();
    float Distance = Dir.Length();
    Dir.Normalize();
    Vector3 reflect = Vector3::Reflect(Dir, GetUp());

  

    reflection->position = GetWorldPos() - reflect * Distance;
    reflection->SetTarget();


    Plane pl(GetWorldPos(), GetUp());
    {
        //상수버퍼
        D3D11_MAPPED_SUBRESOURCE mappedResource;
        D3D->GetDC()->Map(clipPlaneBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        memcpy_s(mappedResource.pData, sizeof(Vector4), &pl, sizeof(Vector4));
        D3D->GetDC()->Unmap(clipPlaneBuffer, 0);
        D3D->GetDC()->VSSetConstantBuffers(4, 1, &clipPlaneBuffer);
    }

}

void Water::SetRefractionTarget()
{
    Plane pl(GetWorldPos(), -GetUp());
    refraction->position = Camera::main->GetWorldPos();
    refraction->SetTarget();

    {
        //상수버퍼
        D3D11_MAPPED_SUBRESOURCE mappedResource;
        D3D->GetDC()->Map(clipPlaneBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        memcpy_s(mappedResource.pData, sizeof(Vector4), &pl, sizeof(Vector4));
        D3D->GetDC()->Unmap(clipPlaneBuffer, 0);
        D3D->GetDC()->VSSetConstantBuffers(4, 1, &clipPlaneBuffer);
    }
}

void Water::UpdateUv()
{
    VertexModel* vertices = (VertexModel*)mesh->vertices;

    vertices[0].uv = Vector2(0, uvScale);
    vertices[1].uv = Vector2(0, 0);
    vertices[2].uv = Vector2(uvScale, uvScale);
    vertices[3].uv = Vector2(uvScale, 0);
    mesh->UpdateBuffer();
}
