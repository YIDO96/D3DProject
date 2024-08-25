#pragma once
class Sky : public Actor
{
public:
	shared_ptr<Texture> texCube = nullptr;
	static Sky* Create(string name = "Sky");
	virtual void	Render(shared_ptr<Shader> pShader = nullptr) override;
	void	        RenderDetail();
};
