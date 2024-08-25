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

    //분할 위치마다 노드 추가
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
                    //콜라이더랑 좌표랑 충돌하면 벽으로 만듦
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

    //분할 위치마다 노드 추가
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

    //Terrain 중심점 0,0,0이 가운데가 아닌 왼쪽상단이 0,0,0이 되게끔 이동

   
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

    //변환되는 좌표가 존재
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

    //기존에 있던 길은 전부 비운다.
    way.clear();

    //출발지 목적지 같으면
    if (sourX == destX and sourZ == destZ)
    {
        return false;//제자리 멈추기
    }

    for (int z = 0; z < size; z++)
    {
        for (int x = 0; x < size; x++)
        {
            Tiles[z][x].ClearCost();
        }
    }
    priority_queue<Tile*, vector<Tile*>, compare> List;

    //리스트에 출발지를 추가
    Tile* pTemp = &Tiles[sourZ][sourX];
    pTemp->G = 0;       //출발지 현재비용은 0
    pTemp->ClacH(Tiles[destZ][destX], scale); //목적지까지 예상비용 만들기
    pTemp->F = pTemp->G + pTemp->H;
    List.push(pTemp);

    //도착점까지 비용이 생길때 까지 반복
    while (1)
    {
        //꺼내야될 F값이 더이상 없을때
        if (List.empty())
        {
            return false;
        }

        //탐색노드 받아오기
        Tile* Temp = List.top();
        //탐색목록에서 제외
        Temp->isFind = true;
        //맨윗값 빼버리기
        List.pop();

        //맨윗값이 도착점이면 종료(길찾기 종료조건)
        if (Temp->idxX == destX and Temp->idxZ == destZ)
        {
            break;
        }

        //인접 타일 비용검사
        vector<Int2> LoopIdx;

        //왼쪽타일이 존재할때
        if (Temp->idxX > 0)
        {
            LoopIdx.push_back(Int2(Temp->idxX - 1,
                Temp->idxZ));
        }
        //위쪽타일이 존재할때
        if (Temp->idxZ > 0)
        {
            LoopIdx.push_back(Int2(Temp->idxX,
                Temp->idxZ - 1));
        }
        //오른쪽타일이 존재할때
        if (Temp->idxX < size - 1)
        {
            LoopIdx.push_back(Int2(Temp->idxX + 1,
                Temp->idxZ));
        }
        //아래쪽타일이 존재할때
        if (Temp->idxZ < size - 1)
        {
            LoopIdx.push_back(Int2(Temp->idxX,
                Temp->idxZ + 1));
        }

        //왼쪽 위타일이 존재할때
        if (Temp->idxX > 0 and Temp->idxZ > 0)
        {
            LoopIdx.push_back(Int2(Temp->idxX - 1,
                Temp->idxZ -1));
        }
        //왼쪽 아래 존재할때
        if (Temp->idxZ > 0 and Temp->idxZ < size - 1)
        {
            LoopIdx.push_back(Int2(Temp->idxX - 1,
                Temp->idxZ + 1));
        }
        //오른쪽 위타일이 존재할때
        if (Temp->idxX < size - 1 and Temp->idxZ > 0)
        {
            LoopIdx.push_back(Int2(Temp->idxX + 1,
                Temp->idxZ - 1));
        }
        //오른쪽 아래 타일이 존재할때
        if (Temp->idxX < size - 1 and Temp->idxZ < size - 1)
        {
            LoopIdx.push_back(Int2(Temp->idxX + 1,
                Temp->idxZ + 1));
        }


        //상하좌우타일 비용검사
        for (int i = 0; i < LoopIdx.size(); i++)
        {
            Tile* loop =
                &Tiles[LoopIdx[i].y][LoopIdx[i].x];

            //벽이 아닐때 and y값이 많이 차이가 안난다면
            if (loop->pass != false )
            {
                //예상비용 만들기
                loop->ClacH(Tiles[destZ][destX], scale);
                Vector3 dis = loop->pos - Temp->pos;
                float len = dis.Length();
                //현재 가지고있는 비용이 클때
                if (loop->G > (Temp->G + len))
                {
                    //비용갱신
                    loop->G = Temp->G + len;
                    loop->F = loop->G + loop->H;
                    //누구로부터 갱신인지 표시
                    loop->P = Temp;
                    //이 타일이 찾은적 없던 타일이면 리스트에 추가
                    if (!loop->isFind)
                    {
                        List.push(loop);
                    }
                }
            }
        }
        LoopIdx.clear();

        //대각선4개 




    }
    //도착지가 설정이 되었을때
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
    //비용을 무한값으로 초기화
    F = G = H = INT_MAX;
    P = nullptr;
    isFind = false;
}

void Tile::ClacH(Tile& Dest, float scale)
{
    //너의 목적지까지의 예상비용을 계산해라
    int tempX = abs(idxX - Dest.idxX) * scale;
    int tempY = abs(idxZ - Dest.idxZ) * scale;

    H = tempX + tempY + fabs(pos.y - Dest.pos.y );
}
