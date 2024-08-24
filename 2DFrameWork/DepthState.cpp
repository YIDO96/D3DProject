#include "framework.h"

DepthState::DepthState()
{
	D3D11_DEPTH_STENCIL_DESC desc;
	desc.DepthEnable = true;
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	desc.DepthFunc = D3D11_COMPARISON_LESS;

	desc.StencilEnable = true;
	desc.StencilReadMask = 0xff;
	desc.StencilWriteMask = 0xff;

	desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	D3D->GetDevice()->CreateDepthStencilState(&desc, &Enable);

	desc.DepthEnable = false;

	D3D->GetDevice()->CreateDepthStencilState(&desc, &Unable);
}

DepthState::~DepthState()
{
	SafeRelease(Enable);
	SafeRelease(Unable);
}

void DepthState::Set(bool DepthEnable)
{
	if (DepthEnable)
		D3D->GetDC()->OMSetDepthStencilState(Enable, 1);
	else
		D3D->GetDC()->OMSetDepthStencilState(Unable, 1);
}
