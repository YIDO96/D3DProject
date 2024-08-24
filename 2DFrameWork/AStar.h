#pragma once


class Tile
{
public:
    int         F, G, H;//타일 비용
    Tile*       P;      //나를 갱신시킨 타일
    bool        isFind; //검사한적이 있는가?


    int         idxX, idxZ;    //타일 인덱스
    bool        pass = true;
    Vector3     pos;

    void ClearCost();         //비용 초기화
    void ClacH(Tile& Dest,float scale); //H계산해라
};

struct compare
{
    //연산자 오버로딩
    bool operator()(Tile* a, Tile* b)
    {
        return a->F > b->F;
    }
};

class AStar
{
private:
    Terrain*    target;     //나눌 맵
    int         size;       //나눌 갯수
    float       scale;
    vector<vector<Tile>>    Tiles;  //길찾기에 필요한 노드 2차원갯수
public:
    AStar();
    void    CreateNode(Terrain* map, int size);
    void    ResizeNode(int size);
    bool    GetNearNode(Vector3 coord, int& idxX ,int& idxZ);


    bool    PathFinding(Vector3 start, Vector3 end, OUT vector<Vector3>& way);

};

