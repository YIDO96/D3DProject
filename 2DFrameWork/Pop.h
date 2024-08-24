#pragma once

struct POP_DESC
{
    float duration;		//	총 재생시간
    float time;			//  현재 재생중인시간
    float gravity;		//  중력값
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
    Vector2             particleScale = Vector2(0, 0); //이미지 크기값
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

