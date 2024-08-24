#pragma once
class UI : public Actor
{
	bool			 Press = false;
public:
	Vector2			 pivot;
	static UI*		Create(string name = "UI");
	bool			MouseOver(class Camera* cam = Camera::main);
	void			RenderDetail();

	function<void()> mouseOver = nullptr; //���콺�� ���������ҋ�
	function<void()> mouseDown = nullptr; // ������ �������� �ѹ�
	function<void()> mousePress = nullptr; // ������ ������
	function<void()> mouseUp = nullptr; //������ ��������

	virtual void	Update() override;
	void			Render(shared_ptr<Shader> pShader = nullptr);
};

