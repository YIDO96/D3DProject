#pragma once
#define MAX_BONE 256
class Skeleton
{
    static ID3D11Buffer* bonesBuffer;
public:
    Skeleton() {};
    //                   버퍼에 갱신할 값
    Matrix				 bones[MAX_BONE];
    //                   파일로 쓰여진값
    Matrix				 bonesOffset[MAX_BONE];// * W
    string               file;
    static void CreateStaticMember();
    static void DeleteStaticMember();
    virtual void Set();
    void LoadFile(string file);
    void SaveFile(string file);
};

