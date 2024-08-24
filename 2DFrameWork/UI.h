#pragma once
class UI : public Actor
{
	bool			 Press = false;
public:
	Vector2			 pivot;
	static UI*		Create(string name = "UI");
	bool			MouseOver(class Camera* cam = Camera::main);
	void			RenderDetail();

	function<void()> mouseOver = nullptr; //마우스가 위에존재할떄
	function<void()> mouseDown = nullptr; // 위에서 눌렀을때 한번
	function<void()> mousePress = nullptr; // 누르고 있을때
	function<void()> mouseUp = nullptr; //누르고 떼었을때

	virtual void	Update() override;
	void			Render(shared_ptr<Shader> pShader = nullptr);
};

