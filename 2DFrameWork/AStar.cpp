#include "framework.h"

AStar::AStar()
{
    target = nullptr;
    size = 0;
}


void AStar::CreateNode(Terrain* map, int size)
{
    map->Update();
    target = map;
    this->size = size;
    scale = (float)map->garo / (float)size;

    Tiles.resize(size);
    for (int z = 0; z < size; z++)
    {
        Tiles[z].resize(size);
    }

    //���� ��ġ���� ��� �߰�
    Ray top;
    top.direction = Vector3(0, -1, 0);

    float half = map->garo * 0.5f;
    top.position = Vector3(-half, 10000, half);

    for (int z = 0; z < size; z++)
    {
        for (int x = 0; x < size; x++)
        {
            top.position.x = -half + (x * scale);
            top.position.z = half - (z * scale);

            Vector3 Pos = top.position;
            Utility::RayIntersectLocalMap(top, map, Pos);
            Tiles[z][x].pos = Pos;
            Tiles[z][x].idxX = x;
            Tiles[z][x].idxZ = z;

            for (auto k = map->children.begin(); k != map->children.end(); k++)
            {
                Tiles[z][x].pass = true;
                if (k->second->collider)
                {
                    //k->second->Update();
                    //�ݶ��̴��� ��ǥ�� �浹�ϸ� ������ ����
                    if (k->second->collider->Intersect(Pos))
                    {
                        Tiles[z][x].pass = false;
                        //cout << Pos.x << "," << Pos.z << endl;
                    }
                }
            }
        }

    }
}

void AStar::ResizeNode(int size)
{
    for (int z = 0; z < this->size; z++)
    {
        Tiles[z].clear();
    }
    Tiles.clear();


    this->size = size;
    scale = (float)target->garo / (float)size;

    Tiles.resize(size);
    for (int z = 0; z < this->size; z++)
    {
        Tiles[z].resize(size);
    }

    //���� ��ġ���� ��� �߰�
    Ray top;
    top.direction = Vector3(0, -1, 0);

    float half = target->garo * 0.5f;
    top.position = Vector3(-half, 10000, half);


    for (int z = 0; z < size; z++)
    {
        for (int x = 0; x < size; x++)
        {
            top.position.x = -half + (x * scale);
            top.position.z = half - (z * scale);

            Vector3 Pos = top.position;
            Utility::RayIntersectLocalMap(top, target, Pos);
            Tiles[z][x].pos = Pos;
            Tiles[z][x].idxX = x;
            Tiles[z][x].idxZ = z;

            for (auto k = target->children.begin(); k != target->children.end(); k++)
            {
                Tiles[z][x].pass = true;
                if (k->second->collider)
                {
                    if (k->second->collider->Intersect(Pos))
                    {
                        Tiles[z][x].pass = false;
                    }
                }
            }
        }

    }
}

bool AStar::GetNearNode(Vector3 coord, int& idxX, int& idxZ)
{
    Matrix inverse = target->W.Invert();
    //w=1
    coord = Vector3::Transform(coord, inverse);

    //Terrain �߽��� 0,0,0�� ����� �ƴ� ���ʻ���� 0,0,0�� �ǰԲ� �̵�

   
    float half = target->garo * 0.5f;
    float TerrainIdxX, TerrainIdxZ;
    TerrainIdxX = coord.x + half;
    TerrainIdxZ = -coord.z + half ;

    TerrainIdxX /= scale;
    TerrainIdxZ /= scale;


    if (TerrainIdxX < 0 or TerrainIdxZ < 0 or
        TerrainIdxX >= size - 1 or TerrainIdxZ >= size - 1)
    {
        //cout << "OutofRange" << endl;
        return false;
    }

    //��ȯ�Ǵ� ��ǥ�� ����
    idxX = TerrainIdxX;
    idxZ = TerrainIdxZ;


    return true;
}

bool AStar::PathFinding(Vector3 start, Vector3 end, OUT vector<Vector3>& way)
{
    //

    int sourX, sourZ, destX, destZ;
    GetNearNode(start, sourX, sourZ);
    GetNearNode(end, destX, destZ);


    if (Tiles[destZ][destX].pass == false ||
        Tiles[sourZ][sourX].pass == false)
    {
        return false;
    }

    //������ �ִ� ���� ���� ����.
    way.clear();

    //����� ������ ������
    if (sourX == destX and sourZ == destZ)
    {
        return false;//���ڸ� ���߱�
    }

    for (int z = 0; z < size; z++)
    {
        for (int x = 0; x < size; x++)
        {
            Tiles[z][x].ClearCost();
        }
    }
    priority_queue<Tile*, vector<Tile*>, compare> List;

    //����Ʈ�� ������� �߰�
    Tile* pTemp = &Tiles[sourZ][sourX];
    pTemp->G = 0;       //����� �������� 0
    pTemp->ClacH(Tiles[destZ][destX], scale); //���������� ������ �����
    pTemp->F = pTemp->G + pTemp->H;
    List.push(pTemp);

    //���������� ����� ���涧 ���� �ݺ�
    while (1)
    {
        //�����ߵ� F���� ���̻� ������
        if (List.empty())
        {
            return false;
        }

        //Ž����� �޾ƿ���
        Tile* Temp = List.top();
        //Ž����Ͽ��� ����
        Temp->isFind = true;
        //������ ��������
        List.pop();

        //�������� �������̸� ����(��ã�� ��������)
        if (Temp->idxX == destX and Temp->idxZ == destZ)
        {
            break;
        }

        //���� Ÿ�� ���˻�
        vector<Int2> LoopIdx;

        //����Ÿ���� �����Ҷ�
        if (Temp->idxX > 0)
        {
            LoopIdx.push_back(Int2(Temp->idxX - 1,
                Temp->idxZ));
        }
        //����Ÿ���� �����Ҷ�
        if (Temp->idxZ > 0)
        {
            LoopIdx.push_back(Int2(Temp->idxX,
                Temp->idxZ - 1));
        }
        //������Ÿ���� �����Ҷ�
        if (Temp->idxX < size - 1)
        {
            LoopIdx.push_back(Int2(Temp->idxX + 1,
                Temp->idxZ));
        }
        //�Ʒ���Ÿ���� �����Ҷ�
        if (Temp->idxZ < size - 1)
        {
            LoopIdx.push_back(Int2(Temp->idxX,
                Temp->idxZ + 1));
        }

        //���� ��Ÿ���� �����Ҷ�
        if (Temp->idxX > 0 and Temp->idxZ > 0)
        {
            LoopIdx.push_back(Int2(Temp->idxX - 1,
                Temp->idxZ -1));
        }
        //���� �Ʒ� �����Ҷ�
        if (Temp->idxZ > 0 and Temp->idxZ < size - 1)
        {
            LoopIdx.push_back(Int2(Temp->idxX - 1,
                Temp->idxZ + 1));
        }
        //������ ��Ÿ���� �����Ҷ�
        if (Temp->idxX < size - 1 and Temp->idxZ > 0)
        {
            LoopIdx.push_back(Int2(Temp->idxX + 1,
                Temp->idxZ - 1));
        }
        //������ �Ʒ� Ÿ���� �����Ҷ�
        if (Temp->idxX < size - 1 and Temp->idxZ < size - 1)
        {
            LoopIdx.push_back(Int2(Temp->idxX + 1,
                Temp->idxZ + 1));
        }


        //�����¿�Ÿ�� ���˻�
        for (int i = 0; i < LoopIdx.size(); i++)
        {
            Tile* loop =
                &Tiles[LoopIdx[i].y][LoopIdx[i].x];

            //���� �ƴҶ� and y���� ���� ���̰� �ȳ��ٸ�
            if (loop->pass != false )
            {
                //������ �����
                loop->ClacH(Tiles[destZ][destX], scale);
                Vector3 dis = loop->pos - Temp->pos;
                float len = dis.Length();
                //���� �������ִ� ����� Ŭ��
                if (loop->G > (Temp->G + len))
                {
                    //��밻��
                    loop->G = Temp->G + len;
                    loop->F = loop->G + loop->H;
                    //�����κ��� �������� ǥ��
                    loop->P = Temp;
                    //�� Ÿ���� ã���� ���� Ÿ���̸� ����Ʈ�� �߰�
                    if (!loop->isFind)
                    {
                        List.push(loop);
                    }
                }
            }
        }
        LoopIdx.clear();

        //�밢��4�� 




    }
    //�������� ������ �Ǿ�����
    Tile* p = &Tiles[destZ][destX];
    //dest 4,1 -> 4,2 -> 4,3 -> 3,3 ->2,3-> 1,3 ->
    while (1)
    {
        way.push_back(p->pos);
        p = p->P;
        if (p == nullptr)
        {
            break;
        }
    }
    return true;
}

void Tile::ClearCost()
{
    //����� ���Ѱ����� �ʱ�ȭ
    F = G = H = INT_MAX;
    P = nullptr;
    isFind = false;
}

void Tile::ClacH(Tile& Dest, float scale)
{
    //���� ������������ �������� ����ض�
    int tempX = abs(idxX - Dest.idxX) * scale;
    int tempY = abs(idxZ - Dest.idxZ) * scale;

    H = tempX + tempY + fabs(pos.y - Dest.pos.y );
}
