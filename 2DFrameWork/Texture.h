#pragma once
class Texture 
{
	ID3D11SamplerState*				Sampler;	//샘플러
public:
	ID3D11ShaderResourceView*		srv;		//텍스쳐자원
	string							file;
	D3D11_SAMPLER_DESC			    SamplerDesc;
	Texture();
	~Texture();
	void LoadFile(string file);
	void RenderDetail();
	void CreateSampler();
	void Set(int slot);
	static ScratchImage* GetPixelData(string file);
};

