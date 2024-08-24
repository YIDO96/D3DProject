#pragma once
class BlendState :public Singleton<BlendState>
{
	ID3D11BlendState* Enable; //알파 블렌딩
	ID3D11BlendState* Unable; //알파 테스트
public:
	BlendState();
	~BlendState();
	void Set(bool BlendEnable);
};
