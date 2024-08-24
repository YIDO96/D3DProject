#pragma once
class DepthState : public Singleton<DepthState>
{
	ID3D11DepthStencilState* Enable;
	ID3D11DepthStencilState* Unable;
public:
	DepthState();
	~DepthState();
	void Set(bool DepthEnable);
};

