#pragma once

enum class VertexType
{
    P,
    PC,
    PCN,
    PTN,
    MODEL,
    TERRAIN,
    PT,         //ui
    BILLBOARD,
    PSV,
    PSP,
};

struct VertexP
{
    VertexP()
    {
    };
    VertexP(Vector3 pos)
    {
        position = pos;
    };
    Vector3 position;
};

struct VertexPC
{
    VertexPC()
    {
    };
    VertexPC(Vector3 pos, Color color)
    {
        this->position = pos;
        this->color = color;
    };
    Vector3 position;
    Color color;
};

struct VertexPCN
{
    VertexPCN()
    {
    };
    VertexPCN(Vector3 pos, Color color, Vector3 normal)
    {
        this->position = pos;
        this->color = color;
        this->normal = normal;
    };
    Vector3 position;
    Color color;
    Vector3 normal;
};
struct VertexPTN
{
    VertexPTN()
    {
    };
    VertexPTN(Vector3 pos, Vector2 uv, Vector3 normal)
    {
        this->position = pos;
        this->uv = uv;
        this->normal = normal;
    };
    Vector3 position;
    Vector2 uv;
    Vector3 normal;
};

struct VertexModel
{
    VertexModel()
    {
    };
    VertexModel(Vector3 pos, Vector2 uv, Vector3 normal,
        Vector3 tangent, Vector4 indices, Vector4 weights)
    {
        this->position = pos;
        this->uv = uv;
        this->normal = normal;
        this->tangent = tangent;
        this->indices = indices;
        this->weights = weights;
    };
    Vector3 position;
    Vector2 uv;
    Vector3 normal;
    //노멀매핑 필요
    Vector3 tangent;
    //스키닝 필요
    Vector4 indices;   // 0 , 1 , 3 , 5
    Vector4 weights;   //0.7 ,0.1,0.1,0.1
};

struct VertexTerrain
{
    VertexTerrain()
    {
        weights.x = 1.0f;
        weights.y = 0.0f;
        weights.z = 0.0f;
        weights.w = 0.0f;
    };
    VertexTerrain(Vector3 pos, Vector2 uv, Vector3 normal, Vector4 weight)
    {
        this->position = pos;
        this->uv = uv;
        this->normal = normal;
        this->weights = weight;
    };
    Vector3 position;
    Vector2 uv;
    Vector3 normal;
    Vector4 weights; //
};

struct VertexPT
{
    VertexPT()
    {
    };
    VertexPT(Vector3 pos, Vector2 uv)
    {
        this->position = pos;
        this->uv = uv;
    };
    Vector3 position;
    Vector2 uv;
};

struct VertexBillboard
{
    VertexBillboard()
    {
    };
    VertexBillboard(Vector3 pos, Vector2 mov, Vector2 size)
    {
        this->position = pos;
        this->mov = mov;
        this->size = size;
    };
    Vector3 position;
    Vector2 mov;
    Vector2 size;
};

struct VertexPSV
{
    VertexPSV()
    {
    };
    VertexPSV(Vector3 pos, Vector2 size, Vector3 velocity)
    {
        this->position = pos;
        this->size = size;
        this->velocity = velocity;
    };
    Vector3 position;
    Vector2 size;
    Vector3 velocity;
};

struct VertexPSP
{
    VertexPSP()
    {
    };
    VertexPSP(Vector3 pos, Vector2 size, Vector2 pivot)
    {
        this->position = pos;
        this->size = size;
        this->pivot = pivot;
    };
    Vector3 position;
    Vector2 size;
    Vector2 pivot;
};