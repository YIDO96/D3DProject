#include "framework.h"

Grid* Grid::Create(string name)
{
	Grid* temp = new Grid();
	temp->LoadFile("Grid.xml");
	temp->type = ObType::Actor;
	return temp;
}

Grid::Grid()
{
}

Grid::~Grid()
{
}

void Grid::Update()
{
	scale.z = scale.x;

	float X = (int)(Camera::main->GetWorldPos().x / scale.x);
	X *= scale.x;

	float Z = (int)(Camera::main->GetWorldPos().z / scale.x);
	Z *= scale.x;

	SetWorldPosX(X);
	SetWorldPosZ(Z);

	Actor::Update();

}
