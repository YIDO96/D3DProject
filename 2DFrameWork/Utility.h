#pragma once

namespace Utility
{
    void Replace(string* str, string comp, string rep);
    void Replace(wstring* str, wstring comp, wstring rep);

    wstring ToWString(string value);
    string ToString(wstring value);

    Ray MouseToRay(Camera* Cam = Camera::main);
    Vector3 MouseToNDC(Camera* Cam = Camera::main);

    float NormalizeAngle(float angle);
    Vector3 NormalizeAngles(Vector3 angles);
    Vector3 QuaternionToYawPtichRoll(Quaternion q1);

    bool RayIntersectTri(IN Ray WRay, IN GameObject* Target, OUT Vector3& HitPoint);


    bool RayIntersectMap(IN Ray WRay, IN GameObject* Terrain, OUT Vector3& HitPoint);
    bool RayIntersectLocalMap(IN Ray WRay, IN GameObject* Terrain, OUT Vector3& HitPoint);


    //템플릿 함수는 선언,정의 분리 불가
    template<typename T>
    T Saturate(T src, T min = 0, T max = 1)
    {
        if (src < min)
        {
            return min;
        }
        else if (src > max)
        {
            return max;
        }
        return src;
    }

    template<typename T>
    void Swap(T & a, T & b)
    {
        T temp = a;
        a = b;
        b = temp;
    }


    template<typename T>
    T Lerp(T a, T b , float w)
    {
        return a + (b - a) * w;
    }
    template<typename T>
    T Lerp(T a, T b, T c ,float w)
    {
        return Lerp(Lerp(a,b,w),Lerp(b,c,w),w) ;
    }
};

