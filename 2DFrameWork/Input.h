#pragma once


class Input : public Singleton<Input>
{
    friend class Window;
private:
    unsigned char   keyState[256];
    unsigned char   keyOldState[256];
    unsigned char   keyMap[256];
    Vector3         wheelStatus;
    Vector3         wheelOldStatus;
    Vector3         oldPostion;
    Vector3         currentPostion;

    //익명 enum
    enum
    {
        KEY_INPUT_STATUS_DOWN,
        KEY_INPUT_STATUS_PRESS,
        KEY_INPUT_STATUS_NONE,
        KEY_INPUT_STATUS_UP,
    };
public:
    Vector3 position;       //마우스 위치
    Vector3 movePosition;   //마우스 이동벡터
    Vector3 wheelMoveValue;
public:
    Input();
    ~Input();

    bool KeyDown(int KeyCode); //눌렀을때 최초1회
    bool KeyPress(int KeyCode);//누르고있을때
    bool KeyUp(int KeyCode); //눌렀다가 떼었을때 최초1회

    LRESULT InputProc(UINT message, WPARAM wParam, LPARAM lParam);
    void Update();
};

