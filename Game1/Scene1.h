#pragma once

class Scene1 : public Scene
{
private:
    Camera* cam1;
    Grid* grid;

    Terrain* terrain;
    Actor*      vam;
    Deferred*   deferred;   //4개의 렌더타겟  
    UI*      post;       //4개의 렌더타겟을 받아 그려줄
    Actor* test;

public:
    Scene1();
    ~Scene1();

    // Scene을(를) 통해 상속됨
    virtual void Init() override;
    virtual void Release() override;
    virtual void Update() override;
    virtual void LateUpdate() override;
    virtual void PreRender() override;
    virtual void Render() override;
    virtual void ResizeScreen() override;
};

