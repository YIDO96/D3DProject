#include "framework.h"

shared_ptr<Mesh> Mesh::CreateMesh()
{
    shared_ptr<Mesh> temp = make_shared<Mesh>();
    temp->vertexType = VertexType::BILLBOARD;
    temp->primitiveTopology = D3D10_PRIMITIVE_TOPOLOGY_POINTLIST;
    temp->byteWidth = sizeof(VertexBillboard);


    //정점 갯수
    temp->vertexCount = 1;
    //인덱스 갯수
    temp->indexCount = 1;

    VertexBillboard* Vertex = new VertexBillboard[temp->vertexCount];
    temp->indices = new UINT[temp->indexCount];

    Vertex[0].position = Vector3(0, 0, 0);
    Vertex[0].mov = Vector2(0, 0);
    Vertex[0].size = Vector2(0, 0);
   
    temp->indices[0] = 0;



    temp->vertices = Vertex;

    //CreateVertexBuffer
    {
        D3D11_BUFFER_DESC desc;
        desc = { 0 };
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.ByteWidth = temp->byteWidth * temp->vertexCount;
        desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA data = { 0 };
        data.pSysMem = temp->vertices;

        HRESULT hr = D3D->GetDevice()->CreateBuffer(&desc, &data, &temp->vertexBuffer);
        assert(SUCCEEDED(hr));
    }

    //Create Index Buffer
    {
        D3D11_BUFFER_DESC desc;
        ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
        desc.ByteWidth = sizeof(UINT) * temp->indexCount;
        desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

        D3D11_SUBRESOURCE_DATA data = { 0 };
        data.pSysMem = temp->indices;

        HRESULT hr = D3D->GetDevice()->CreateBuffer(&desc, &data, &temp->indexBuffer);
        assert(SUCCEEDED(hr));
    }

    return temp;
}

Mesh::Mesh()
{
    indices = nullptr;
    vertices = nullptr;
    instance = nullptr;
    vertexBuffer = nullptr;
    indexBuffer = nullptr;
    instanceBuffer = nullptr;
    vertexType = VertexType::P;
}

Mesh::Mesh(void* vertices, UINT vertexCount, UINT* indices, UINT indexCount, VertexType type)
{
    vertexType = type;
    primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    switch (type)
    {
    case VertexType::P:
        byteWidth = sizeof(VertexP);
        break;
    case VertexType::PC:
        byteWidth = sizeof(VertexPC);
        break;
    case VertexType::PCN:
        byteWidth = sizeof(VertexPCN);
        break;
    case VertexType::PTN:
        byteWidth = sizeof(VertexPTN);
        break;
    case VertexType::MODEL:
        byteWidth = sizeof(VertexModel);
        break;

    case VertexType::TERRAIN:
        byteWidth = sizeof(VertexTerrain);
        break;
    case VertexType::PT:
        byteWidth = sizeof(VertexPT);
        break;
    case VertexType::BILLBOARD:
        byteWidth = sizeof(VertexBillboard);
        break;
    case VertexType::PSV:
        byteWidth = sizeof(VertexPSV);
        break;
    case VertexType::PSP:
        byteWidth = sizeof(VertexPSP);
        break;
    }
    ////////////////////////////////////////////////////

    ///////////////////////////////////////////////////
    this->vertices = vertices;
    this->vertexCount = vertexCount;
    this->indices = indices;
    this->indexCount = indexCount;

    //CreateVertexBuffer
    {
        D3D11_BUFFER_DESC desc;
        desc = { 0 };
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.ByteWidth = byteWidth * vertexCount;
        desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA data = { 0 };
        data.pSysMem = vertices;

        HRESULT hr = D3D->GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
        assert(SUCCEEDED(hr));
    }

    //Create Index Buffer
    {
        D3D11_BUFFER_DESC desc;
        ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
        desc.ByteWidth = sizeof(UINT) * indexCount;
        desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

        D3D11_SUBRESOURCE_DATA data = { 0 };
        data.pSysMem = indices;

        HRESULT hr = D3D->GetDevice()->CreateBuffer(&desc, &data, &indexBuffer);
        assert(SUCCEEDED(hr));
    }
}

void Mesh::CreateInstanceBuffer(Matrix* instance, UINT instanceCount)
{
    this->instance = instance;
    this->instanceCount = instanceCount;
    SafeRelease(instanceBuffer);

    //CreateInstanceBuffer
    {
        D3D11_BUFFER_DESC desc;
        desc = { 0 };
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.ByteWidth = sizeof(Matrix) * instanceCount;
        desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA data = { 0 };
        data.pSysMem = instance;
        HRESULT hr = D3D->GetDevice()->CreateBuffer(&desc, &data, &instanceBuffer);
        assert(SUCCEEDED(hr));
    }

}

Mesh::~Mesh()
{
}

void Mesh::Set()
{
    UINT offset = 0;
    D3D->GetDC()->IASetVertexBuffers(0,
        1,
        &vertexBuffer,
        &byteWidth,
        &offset);
    D3D->GetDC()->IASetPrimitiveTopology
    (primitiveTopology);
    D3D->GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
}

void Mesh::LoadFile(string file)
{
    this->file = file;
    BinaryReader in;
    wstring path = L"../Contents/Mesh/" + Utility::ToWString(file);
    in.Open(path);
    //읽기전
    switch (vertexType)
    {
    case VertexType::P:
        if (vertices)delete[](VertexP*)vertices;
        break;
    case VertexType::PC:
        if (vertices)delete[](VertexPC*)vertices;
        break;
    case VertexType::PCN:
        if (vertices)delete[](VertexPCN*)vertices;
        break;
    case VertexType::PTN:
        if (vertices)delete[](VertexPTN*)vertices;
        break;
    case VertexType::MODEL:
        if (vertices)delete[](VertexModel*)vertices;
        break;
    case VertexType::TERRAIN:
        if (vertices)delete[](VertexTerrain*)vertices;
        break;
    case VertexType::PT:
        if (vertices)delete[](VertexPT*)vertices;
        break;
    case VertexType::BILLBOARD:
        if (vertices)delete[](VertexBillboard*)vertices;
        break;
    case VertexType::PSV:
        if (vertices)delete[](VertexPSV*)vertices;
        break;
    }
    vertexType = (VertexType)in.UInt();
    primitiveTopology = (D3D_PRIMITIVE_TOPOLOGY)in.UInt();
    byteWidth = in.UInt();
    vertexCount = in.UInt();
    indexCount = in.UInt();

    SafeDeleteArray(indices);
    indices = new UINT[indexCount];


    //읽고난후
    switch (vertexType)
    {
    case VertexType::P:
    {
        vertices = new VertexP[vertexCount];
        VertexP* vertex = (VertexP*)vertices;
        for (UINT i = 0; i < vertexCount; i++)
        {
            vertex[i].position = in.vector3();
        }
        break;
    }

    case VertexType::PC:
    {
        vertices = new VertexPC[vertexCount];
        VertexPC* vertex = (VertexPC*)vertices;
        for (UINT i = 0; i < vertexCount; i++)
        {
            vertex[i].position = in.vector3();
            vertex[i].color = in.color3f();
        }
        break;
    }
    case VertexType::PCN:
    {
        vertices = new VertexPCN[vertexCount];
        VertexPCN* vertex = (VertexPCN*)vertices;
        for (UINT i = 0; i < vertexCount; i++)
        {
            vertex[i].position = in.vector3();
            vertex[i].color = in.color3f();
            vertex[i].normal = in.vector3();
        }
        break;
    }
    case VertexType::PTN:
    {
        vertices = new VertexPTN[vertexCount];
        VertexPTN* vertex = (VertexPTN*)vertices;
        for (UINT i = 0; i < vertexCount; i++)
        {
            vertex[i].position = in.vector3();
            vertex[i].uv = in.vector2();
            vertex[i].normal = in.vector3();
        }
        break;
    }
    case VertexType::MODEL:
    {
        vertices = new VertexModel[vertexCount];
        VertexModel* vertex = (VertexModel*)vertices;
        for (UINT i = 0; i < vertexCount; i++)
        {
            vertex[i].position = in.vector3();
            vertex[i].uv = in.vector2();
            vertex[i].normal = in.vector3();
            vertex[i].tangent = in.vector3();
            vertex[i].indices = in.vector4();
            vertex[i].weights = in.vector4();
        }
        break;
    }
    case VertexType::TERRAIN:
    {
        vertices = new VertexTerrain[vertexCount];
        VertexTerrain* vertex = (VertexTerrain*)vertices;
        for (UINT i = 0; i < vertexCount; i++)
        {
            vertex[i].position = in.vector3();
            vertex[i].uv = in.vector2();
            vertex[i].normal = in.vector3();
            vertex[i].weights = in.vector4();
        }
        break;
    }
    case VertexType::PT:
    {
        vertices = new VertexPT[vertexCount];
        VertexPT* vertex = (VertexPT*)vertices;
        for (UINT i = 0; i < vertexCount; i++)
        {
            vertex[i].position = in.vector3();
            vertex[i].uv = in.vector2();
        }
        break;
    }
    case VertexType::BILLBOARD:
    {
        vertices = new VertexBillboard[vertexCount];
        VertexBillboard* vertex = (VertexBillboard*)vertices;
        for (UINT i = 0; i < vertexCount; i++)
        {
            vertex[i].position = in.vector3();
            vertex[i].size = in.vector2();
        }
        break;
    }
    case VertexType::PSV:
    {
        vertices = new VertexPSV[vertexCount];
        VertexPSV* vertex = (VertexPSV*)vertices;
        for (UINT i = 0; i < vertexCount; i++)
        {
            vertex[i].position = in.vector3();
            vertex[i].size = in.vector2();
            vertex[i].velocity = in.vector3();
        }
        break;
    }
    }
    for (UINT i = 0; i < indexCount; i++)
    {
        indices[i] = in.UInt();
    }
    in.Close();

    SafeRelease(vertexBuffer);
    SafeRelease(indexBuffer);
    //CreateVertexBuffer
    {
        D3D11_BUFFER_DESC desc;
        desc = { 0 };
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.ByteWidth = byteWidth * vertexCount;
        desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA data = { 0 };
        data.pSysMem = vertices;

        HRESULT hr = D3D->GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
        assert(SUCCEEDED(hr));
    }

    //Create Index Buffer
    {
        D3D11_BUFFER_DESC desc;
        ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
        desc.ByteWidth = sizeof(UINT) * indexCount;
        desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

        D3D11_SUBRESOURCE_DATA data = { 0 };
        data.pSysMem = indices;

        HRESULT hr = D3D->GetDevice()->CreateBuffer(&desc, &data, &indexBuffer);
        assert(SUCCEEDED(hr));
    }

}

void Mesh::SaveFile(string file)
{
    this->file = file;

    BinaryWriter out;
    wstring path = L"../Contents/Mesh/" + Utility::ToWString(file);
    out.Open(path);

    out.UInt((UINT)vertexType);
    out.UInt((UINT)primitiveTopology);
    out.UInt(byteWidth);
    out.UInt(vertexCount);
    out.UInt(indexCount);

    switch (vertexType)
    {
    case VertexType::P:
    {
        VertexP* vertex = (VertexP*)vertices;
        for (UINT i = 0; i < vertexCount; i++)
        {
            out.vector3(vertex[i].position);
        }
        break;
    }

    case VertexType::PC:
    {
        VertexPC* vertex = (VertexPC*)vertices;
        for (UINT i = 0; i < vertexCount; i++)
        {
            out.vector3(vertex[i].position);
            out.color3f(vertex[i].color);
        }
        break;
    }
    case VertexType::PCN:
    {
        VertexPCN* vertex = (VertexPCN*)vertices;
        for (UINT i = 0; i < vertexCount; i++)
        {
            out.vector3(vertex[i].position);
            out.color3f(vertex[i].color);
            out.vector3(vertex[i].normal);
        }
        break;
    }
    case VertexType::PTN:
    {
        VertexPTN* vertex = (VertexPTN*)vertices;
        for (UINT i = 0; i < vertexCount; i++)
        {
            out.vector3(vertex[i].position);
            out.vector2(vertex[i].uv);
            out.vector3(vertex[i].normal);
        }
        break;
    }
    case VertexType::MODEL:
    {
        VertexModel* vertex = (VertexModel*)vertices;
        for (UINT i = 0; i < vertexCount; i++)
        {
            out.vector3(vertex[i].position);
            out.vector2(vertex[i].uv);
            out.vector3(vertex[i].normal);
            out.vector3(vertex[i].tangent);
            out.vector4(vertex[i].indices);
            out.vector4(vertex[i].weights);
        }
        break;
    }
    case VertexType::TERRAIN:
    {
        VertexTerrain* vertex = (VertexTerrain*)vertices;
        for (UINT i = 0; i < vertexCount; i++)
        {
            out.vector3(vertex[i].position);
            out.vector2(vertex[i].uv);
            out.vector3(vertex[i].normal);
            out.vector4(vertex[i].weights);
        }
        break;
    }
    case VertexType::PT:
    {
        VertexPT* vertex = (VertexPT*)vertices;
        for (UINT i = 0; i < vertexCount; i++)
        {
            out.vector3(vertex[i].position);
            out.vector2(vertex[i].uv);
        }
        break;
    }
    case VertexType::BILLBOARD:
    {
        VertexBillboard* vertex = (VertexBillboard*)vertices;
        for (UINT i = 0; i < vertexCount; i++)
        {
            out.vector3(vertex[i].position);
            out.vector2(vertex[i].size);
        }
        break;
    }
    case VertexType::PSV:
    {
        VertexPSV* vertex = (VertexPSV*)vertices;
        for (UINT i = 0; i < vertexCount; i++)
        {
            out.vector3(vertex[i].position);
            out.vector2(vertex[i].size);
            out.vector3(vertex[i].velocity);
        }
        break;
    }
    }
    for (UINT i = 0; i < indexCount; i++)
    {
        out.UInt(indices[i]);
    }
    out.Close();
}

void Mesh::RenderDetail()
{
    InstanceEdit();

    //for (int i = 0; i < vertexCount; i++)
    //{
    //    //if (vertexType == VertexType::PT)
    //    //{

    //    //    if (i % 4 == 0)
    //    //    {
    //    //        string str = "Face" + to_string((i / 4));
    //    //        ImGui::Text(str.c_str());
    //    //    }

    //    //    VertexPT* vertex = (VertexPT*)vertices;

    //    //    //vertex[i].uv.x += DELTA;
    //    //    //D3D->GetDC()->UpdateSubresource
    //    //    //(vertexBuffer, 0, NULL, vertices, 0, 0);
    //    //    //string str = "pos" + to_string(i);
    //    //    /*if (ImGui::SliderFloat3(str.c_str(), (float*)
    //    //        &vertex[i].position, -10, 10))
    //    //    {
    //    //        D3D->GetDC()->UpdateSubresource
    //    //        (vertexBuffer, 0, NULL, vertices, 0, 0);
    //    //    }*/
    //    //    string str = "uv" + to_string(i);
    //    //    if (ImGui::InputFloat2(str.c_str(), (float*)
    //    //        &vertex[i].uv))
    //    //    {
    //    //        D3D->GetDC()->UpdateSubresource
    //    //        (vertexBuffer, 0, NULL, vertices, 0, 0);
    //    //    }
    //    //}
    //}


}

const Vector3& Mesh::GetVertexPosition(UINT idx)
{
    if (vertexType == VertexType::PC)
    {
        VertexPC* Vertices = (VertexPC*)vertices;
        return Vertices[indices[idx]].position;
    }
    else if (vertexType == VertexType::PCN)
    {
        VertexPCN* Vertices = (VertexPCN*)vertices;
        return Vertices[indices[idx]].position;
    }
    else if (vertexType == VertexType::PTN)
    {
        VertexPTN* Vertices = (VertexPTN*)vertices;
        return Vertices[indices[idx]].position;
    }
    else if (vertexType == VertexType::MODEL)
    {
        VertexModel* Vertices = (VertexModel*)vertices;
        return Vertices[indices[idx]].position;
    }
    else if (vertexType == VertexType::TERRAIN)
    {
        VertexTerrain* Vertices = (VertexTerrain*)vertices;
        return Vertices[indices[idx]].position;
    }
    else if (vertexType == VertexType::PT)
    {
        VertexPT* Vertices = (VertexPT*)vertices;
        return Vertices[indices[idx]].position;
    }
    else if (vertexType == VertexType::BILLBOARD)
    {
        VertexBillboard* Vertices = (VertexBillboard*)vertices;
        return Vertices[indices[idx]].position;
    }
    else if (vertexType == VertexType::PSV)
    {
        VertexPSV* Vertices = (VertexPSV*)vertices;
        return Vertices[indices[idx]].position;
    }
    else if (vertexType == VertexType::PSP)
    {
        VertexPSP* Vertices = (VertexPSP*)vertices;
        return Vertices[indices[idx]].position;
    }

    VertexP* Vertices = (VertexP*)vertices;
    return Vertices[indices[idx]].position;
}

void Mesh::UpdateBuffer()
{
    D3D->GetDC()->UpdateSubresource(vertexBuffer, 0, NULL, vertices, 0, 0);
}

void Mesh::DrawCall()
{
    if (not instanceBuffer)
    {
        D3D->GetDC()->DrawIndexed(indexCount, 0, 0);
    }
    else
    {
        UINT temp = sizeof(Matrix);
        UINT offset = 0;
        D3D->GetDC()->IASetVertexBuffers(1, 1, &instanceBuffer, &temp, &offset);
        D3D->GetDC()->DrawIndexedInstanced(indexCount, instanceCount, 0, 0, 0);
    }
}

void Mesh::LoadInstanceFile(string file)
{
    instanceFile = file;
    BinaryReader in;
    wstring path = L"../Contents/Instance/" + Utility::ToWString(file);
    in.Open(path);
    instanceCount = in.Int();
    SafeDeleteArray(instance);
    instance = new Matrix[instanceCount];

    for (int i = 0; i < instanceCount; i++)
    {
        instance[i] = in.matrix();
    }
    in.Close();

    CreateInstanceBuffer(instance, instanceCount);
}

void Mesh::SaveInstanceFile(string file)
{
    instanceFile = file;
    BinaryWriter out;
    wstring path = L"../Contents/Instance/" + Utility::ToWString(file);
    out.Open(path);
    out.Int(instanceCount);
    for (int i = 0; i < instanceCount; i++)
    {
        out.matrix(instance[i]);
    }
    out.Close();
}

void Mesh::InstanceEdit()
{
    //내 인스턴스 갯수
    string instanceCount = to_string(this->instanceCount);
    ImGui::Text(instanceCount.c_str());

    for (int i = 0; i < this->instanceCount; i++)
    {
        string instanceCount = "Instance" + to_string(i);
        if (ImGui::TreeNode(instanceCount.c_str()))
        {
            Matrix Local = instance[i].Transpose();
            Vector3 s,r, t;
            Quaternion q;
            Local.Decompose(s, q, t);
            r = Utility::QuaternionToYawPtichRoll(q);

            ImGui::DragFloat3("scale", (float*)&s, 0.05f);
            ImGui::InputFloat3("rot", (float*)&r);
            ImGui::DragFloat3("pos", (float*)&t, 0.05f);
               // SetWorldPos(wol);
            Matrix S = Matrix::CreateScale(s.x, s.y, s.z);
            // Ry*Rx*Rz
            Matrix R = Matrix::CreateFromYawPitchRoll(r.y, r.x, r.z);
            Matrix T = Matrix::CreateTranslation(t.x, t.y, t.z);
            Local = S * R * T;

            Local = Local.Transpose();
            instance[i] = Local;

            D3D->GetDC()->UpdateSubresource
            (instanceBuffer, 0, NULL, instance, 0, 0);

            ImGui::TreePop();
        }


        //좌표
        //ImGui::Text("X: %f Y: %f Z: %f", instance[i]._14, instance[i]._24, instance[i]._34);
       

        /*if (ImGui::Button(instanceCount.c_str()))
        {
            instance[i] = GUI->World;
            D3D->GetDC()->UpdateSubresource
            (instanceBuffer, 0, NULL, instance, 0, 0);
        }*/
    }
    if (ImGui::Button("+"))
    {
        Matrix* Instance = new Matrix[this->instanceCount + 1];
        //복사
        memcpy(Instance, instance, sizeof(Matrix) * this->instanceCount);

        CreateInstanceBuffer(Instance, this->instanceCount + 1);

        //this->instanceCount += 1;
    }
    if (ImGui::Button("-"))
    {
        if (this->instanceCount > 1)
        {
            Matrix* Instance = new Matrix[this->instanceCount - 1];
            //복사
            memcpy(Instance, instance, sizeof(Matrix) * (this->instanceCount - 1));

            CreateInstanceBuffer(Instance, this->instanceCount - 1);

            //this->instanceCount--;
        }
        else if (this->instanceCount == 1)
        {
            this->instanceCount = 0;
            SafeDeleteArray(instance);
            SafeRelease(instanceBuffer);

        }


    }


    if (GUI->FileImGui("Save Instance", "Save Instance",
        ".ins", "../Contents/Instance"))
    {
        string path = ImGuiFileDialog::Instance()->GetCurrentPath();
        Utility::Replace(&path, "\\", "/");
        if (path.find("/Instance/") != -1)
        {
            size_t tok = path.find("/Instance/") + 10;
            path = path.substr(tok, path.length())
                + "/" + ImGuiFileDialog::Instance()->GetCurrentFileName();
        }
        else
        {
            path = ImGuiFileDialog::Instance()->GetCurrentFileName();
        }
        SaveInstanceFile(path);
    }

    ImGui::SameLine();

    if (GUI->FileImGui("Load Instance", "Load Instance",
        ".ins", "../Contents/Instance"))
    {
        string path = ImGuiFileDialog::Instance()->GetCurrentPath();
        Utility::Replace(&path, "\\", "/");
        if (path.find("/Instance/") != -1)
        {
            size_t tok = path.find("/Instance/") + 10;
            path = path.substr(tok, path.length())
                + "/" + ImGuiFileDialog::Instance()->GetCurrentFileName();
        }
        else
        {
            path = ImGuiFileDialog::Instance()->GetCurrentFileName();
        }
        LoadInstanceFile(path);
    }


}
