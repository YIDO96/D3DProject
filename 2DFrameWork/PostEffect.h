#pragma once
class PostEffect
{
	struct PSP
	{
		int filter = 0;
		Vector3 color;
		Vector2 screen; //좌표
		float radius = 100;
		int count = 1;

		int select = 0;
		float width;
		float height;
		float padding;
	};
	ID3D11Buffer* PEBuffer;
private:
    RenderTarget*   target; //그려줄 텍스처
    UI*             ui;     //매핑할 오브젝트

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

