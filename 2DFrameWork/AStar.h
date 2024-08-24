#pragma once


class Tile
{
public:
    int         F, G, H;//Ÿ�� ���
    Tile*       P;      //���� ���Ž�Ų Ÿ��
    bool        isFind; //�˻������� �ִ°�?


    int         idxX, idxZ;    //Ÿ�� �ε���
    bool        pass = true;
    Vector3     pos;

    void ClearCost();         //��� �ʱ�ȭ
    void ClacH(Tile& Dest,float scale); //H����ض�
};

struct compare
{
    //������ �����ε�
    bool operator()(Tile* a, Tile* b)
    {
        return a->F > b->F;
    }
};

class AStar
{
private:
    Terrain*    target;     //���� ��
    int         size;       //���� ����
    float       scale;
    vector<vector<Tile>>    Tiles;  //��ã�⿡ �ʿ��� ��� 2��������
public:
    AStar();
    void    CreateNode(Terrain* map, int size);
    void    ResizeNode(int size);
    bool    GetNearNode(Vector3 coord, int& idxX ,int& idxZ);


    bool    PathFinding(Vector3 start, Vector3 end, OUT vector<Vector3>& way);

};

