#pragma once

struct POP_DESC
{
    float duration;		//	�� ����ð�
    float time;			//  ���� ������νð�
    float gravity;		//  �߷°�
    float padding;
    POP_DESC()
    {
        gravity = 60.0f;
    }
};

class Pop :  public Particle
{
    static ID3D11Buffer* PopBuffer;
public:
    POP_DESC			desc;
    Vector2             particleScale = Vector2(0, 0); //�̹��� ũ�Ⱚ
    int                 particleCount = 100;
    float               velocityScalar = 10.0f;

    static void         CreateStaticMember();
    static void         DeleteStaticMember();
    static Pop* Create(string name = "Pop");
    virtual void        Render();
    virtual void        Update();
    void                RenderDetail();
    void                Reset();
    virtual void        Play();
    virtual void        Stop();


};

