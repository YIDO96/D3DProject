#pragma once
class Transform
{

protected:
    static bool         worldPos;
    static ID3D11Buffer* WBuffer;
public:
    static void CreateStaticMember();
    static void DeleteStaticMember();
protected:
    Vector3				position;
public:
    Matrix				S, R, T, RT, W;
    Vector3				scale;
    Vector3				rotation;
    class GameObject*   parent;

protected:
    void	SaveTransform(Xml::XMLElement* This, Xml::XMLDocument* doc);
    void	LoadTransform(Xml::XMLElement* This);
public:
    Transform();
    virtual ~Transform() {};
    virtual void	Update();
    void            UpdateAnim(Matrix&& bone);
    virtual void	RenderDetail();
    virtual void    Set();

public:
    Vector3 GetRight() { return Vector3(RT._11, RT._12, RT._13); }
    Vector3 GetUp() { return Vector3(RT._21, RT._22, RT._23); }
    Vector3 GetForward() { return Vector3(RT._31, RT._32, RT._33); }

    //월드 공간 좌표
    // Read
    Vector3     GetWorldPos();
    // = Write
    void        SetWorldPos(Vector3 WPos);
    void        SetWorldPosX(float WPosX);
    void        SetWorldPosY(float WPosY);
    void        SetWorldPosZ(float WPosZ);
    // +=
    void        MoveWorldPos(const Vector3& WScaleVec);

    //월드 스케일 로컬 좌표
    Vector3     GetLocalPos();
    void        SetLocalPos(Vector3 WPos);
    void        SetLocalPosX(float WPosX);
    void        SetLocalPosY(float WPosY);
    void        SetLocalPosZ(float WPosZ);
    void        MoveLocalPos(const Vector3& WScaleVec);
    Matrix      GetLocal();
};

