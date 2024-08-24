#include "framework.h"

Deferred::Deferred(UINT width, UINT height)
{
    diffuseTarget = new RenderTarget(width, height,true);
    specularTarget = new RenderTarget(width, height);
    normalTarget = new RenderTarget(width, height);
    emissiveTarget = new RenderTarget(width, height);
    ambientTarget = new RenderTarget(width, height);

    rtvs[0] = diffuseTarget;
    rtvs[1] = specularTarget;
    rtvs[2] = normalTarget;
    rtvs[3] = emissiveTarget;
    rtvs[4] = ambientTarget;

    srvs[0] = diffuseTarget->rgbResource;
    srvs[1] = specularTarget->rgbResource;
    srvs[2] = normalTarget->rgbResource;
    srvs[3] = emissiveTarget->rgbResource;
    srvs[4] = ambientTarget->rgbResource;
    srvs[5] = diffuseTarget->depthResource;

    shader = RESOURCE->shaders.Load("6.DeferredRender.hlsl");

    UINT vertices[4] = { 0, 1, 2, 3 };

    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = sizeof(UINT) * 4;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA subData = {};
    subData.pSysMem = vertices;

    D3D->GetDevice()->CreateBuffer(&bufferDesc, &subData, &buffer);

}

Deferred::~Deferred()
{
    delete diffuseTarget;
    delete specularTarget;
    delete normalTarget;
    delete emissiveTarget;
    delete ambientTarget;
}

void Deferred::SetTarget(Color clear)
{
    vector<ID3D11RenderTargetView*> vecRtvs;

    for (int i = 0; i < 5; i++)
    {
        vecRtvs.push_back(rtvs[i]->rgbTarget);
        D3D->GetDC()->ClearRenderTargetView(vecRtvs.back(), (float*)&clear);
    }
    D3D->GetDC()->ClearDepthStencilView(diffuseTarget->depthTarget, D3D11_CLEAR_DEPTH, 1.0f, 0);

    D3D->GetDC()->OMSetRenderTargets(5, vecRtvs.data(), diffuseTarget->depthTarget);
}

void Deferred::SetRGBTexture(int slot)
{
    for (int i = 0; i < 6; i++)
    {
        D3D->GetDC()->PSSetShaderResources(slot + i, 1, &srvs[i]);
    }
}

void Deferred::ResizeScreen(float width, float height)
{
    diffuseTarget->ResizeScreen(width, height);
    specularTarget->ResizeScreen(width, height);
    normalTarget->ResizeScreen(width, height);
    emissiveTarget->ResizeScreen(width, height);
    ambientTarget->ResizeScreen(width, height);
    //posTarget->ResizeScreen(width, height);
}

void Deferred::RenderDetail()
{
    ImVec2 size(300, 300);
    ImGui::Image((void*)diffuseTarget->rgbResource, size);
    ImGui::Image((void*)specularTarget->rgbResource, size);
    ImGui::Image((void*)normalTarget->rgbResource, size);
    ImGui::Image((void*)emissiveTarget->rgbResource, size);
    ImGui::Image((void*)ambientTarget->rgbResource, size);
    ImGui::Image((void*)diffuseTarget->depthResource, size);
}

void Deferred::Render()
{
    SetRGBTexture(10);
    shader->Set();
    UINT stride = sizeof(UINT);
    UINT offset = 0;
    D3D->GetDC()->IASetVertexBuffers(0, 1, &buffer, &stride, &offset);
    D3D->GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    D3D->GetDC()->Draw(4, 0);
}
