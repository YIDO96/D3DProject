#pragma once
class SceneManager :public Singleton<SceneManager>
{
private:
    bool                    isChanging = false;
    Scene *                 nextScene = nullptr;
    Scene *                 currentScene = nullptr;
    map<string, Scene*>    scenes;

    float changingTime = 0.0f;

public:
    ~SceneManager();

    //¾À Ãß°¡
    bool    AddScene(string key, Scene * value);
    //¾À »èÁ¦
    bool    DeleteScene(string key);
    //¾À º¯°æ
    Scene * ChangeScene(string key, float changingTime = 0.0f);
    //¾À °¡Á®¿À±â
    Scene * GetScene(string key);
    //ÇöÀç ¾À °¡Á®¿À±â
    Scene * GetCurrentScene();

    void    Release();
    void    Update();
    void    LateUpdate();
    void    Render();
    void    PreRender();
    void    ResizeScreen();
};
