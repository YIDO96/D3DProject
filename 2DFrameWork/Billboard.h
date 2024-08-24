#pragma once
class Billboard : public Actor
{
private:


public:
	static Billboard* Create(string name = "Billboard");
	virtual void	Release();
	virtual void	Update() override;
	virtual void	Render(shared_ptr<Shader> pShader = nullptr) override;
	virtual void	RenderDetail();
};

