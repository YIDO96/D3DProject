#pragma once
class PostEffect
{
	struct PSP
	{
		int filter = 0;
		Vector3 color;
		Vector2 screen; //��ǥ
		float radius = 100;
		int count = 1;

		int select = 0;
		float width;
		float height;
		float padding;
	};
	ID3D11Buffer* PEBuffer;
private:
    RenderTarget*   target; //�׷��� �ؽ�ó
    UI*             ui;     //������ ������Ʈ

public:
	PSP psp;

    PostEffect();
    ~PostEffect();
    void SetTarget(Color color = Color(0,0,0,1));
    void Update();
    void Render();
	void RenderDetail();
	void RenderHierarchy();
};

