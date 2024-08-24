#include "framework.h"

BlendState::BlendState()
{
	D3D11_BLEND_DESC desc;


	desc.IndependentBlendEnable = false;


	desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

	desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE; //1
	desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;// 0
	desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD; // 1 + 0

	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	desc.AlphaToCoverageEnable = false;
	desc.RenderTarget[0].BlendEnable = true;
	D3D->GetDevice()->CreateBlendState(&desc, &Enable);


	desc.AlphaToCoverageEnable = true;
	desc.RenderTarget[0].BlendEnable = false;
	D3D->GetDevice()->CreateBlendState(&desc, &Unable);


}

BlendState::~BlendState()
{
	SafeRelease(Enable);
	SafeRelease(Unable);

}

void BlendState::Set(bool BlendEnable)
{
	float blendFactor[] = { 0, 0, 0, 0 };
	if (BlendEnable)
		D3D->GetDC()->OMSetBlendState(Enable, blendFactor, 0xffffffff);
	else
		D3D->GetDC()->OMSetBlendState(Unable, blendFactor, 0xffffffff);
}
