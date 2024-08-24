#include "framework.h"

Collider::Collider(ColliderType type)
{
	this->type = type;
	visible = true;
	switch (type)
	{
	case ColliderType::SPHERE:
		mesh = RESOURCE->meshes.Load("1.SphereCollider.mesh");
		break;
	case ColliderType::BOX:
		mesh = RESOURCE->meshes.Load("1.BoxCollider.mesh");
		break;
	case ColliderType::OBOX:
		mesh = RESOURCE->meshes.Load("1.BoxCollider.mesh");
		break;
	}
	shader = RESOURCE->shaders.Load("1.Collider.hlsl");
}

Collider::~Collider()
{
	SafeReset(mesh);
	SafeReset(shader);
}

void Collider::Update(GameObject* ob)
{
	parent = ob;
	/*if (type == ColliderType::SPHERE)
	{
		scale.z = scale.y = scale.x;
	}*/
	Transform::Update();
	if (type == ColliderType::BOX)
	{
		T = Matrix::CreateTranslation(GetWorldPos());
		W = S * T;
	}
	if (type == ColliderType::SPHERE)
	{
		scale.z = scale.y = scale.x;
		S = Matrix::CreateScale(scale.x);
		T = Matrix::CreateTranslation(GetWorldPos());
		W = S * T;
	}


}

void Collider::Render()
{

	if (visible)
	{
		Transform::Set();
		mesh->Set();
		shader->Set();
		mesh->DrawCall();
	}
}


bool Collider::Intersect(Collider* target)
{
	if (type == ColliderType::BOX)
	{
		BoundingBox box1;
		box1.Center = GetWorldPos();
		box1.Extents = Vector3(S._11, S._22, S._33);
		if (target->type == ColliderType::BOX)
		{
			BoundingBox box2;
			box2.Center = target->GetWorldPos();
			box2.Extents = Vector3(target->S._11, target->S._22, target->S._33);
			return box1.Intersects(box2);
		}
		else if (target->type == ColliderType::OBOX)
		{
			BoundingOrientedBox box2;
			box2.Center = target->GetWorldPos();
			box2.Extents = Vector3(target->S._11, target->S._22, target->S._33);
			box2.Orientation = Quaternion::CreateFromRotationMatrix(target->RT);
			return box1.Intersects(box2);
		}
		else
		{
			BoundingSphere box2;
			box2.Center = target->GetWorldPos();
			box2.Radius = target->scale.x;
			return box1.Intersects(box2);
		}
	}
	else if (type == ColliderType::OBOX)
	{
		BoundingOrientedBox box1;
		box1.Center = GetWorldPos();
		box1.Extents = Vector3(S._11, S._22, S._33);
		box1.Orientation = Quaternion::CreateFromRotationMatrix(RT);
		if (target->type == ColliderType::BOX)
		{
			BoundingBox box2;
			box2.Center = target->GetWorldPos();
			box2.Extents = Vector3(target->S._11, target->S._22, target->S._33);
			return box1.Intersects(box2);
		}
		else if (target->type == ColliderType::OBOX)
		{
			BoundingOrientedBox box2;
			box2.Center = target->GetWorldPos();
			box2.Extents = Vector3(target->S._11, target->S._22, target->S._33);
			box2.Orientation = Quaternion::CreateFromRotationMatrix(target->RT);
			return box1.Intersects(box2);
		}
		else
		{
			BoundingSphere box2;
			box2.Center = target->GetWorldPos();
			box2.Radius = target->scale.x;
			return box1.Intersects(box2);
		}
	}
	else
	{
		BoundingSphere box1;
		box1.Center = GetWorldPos();
		box1.Radius = scale.x;
		if (target->type == ColliderType::BOX)
		{
			BoundingBox box2;
			box2.Center = target->GetWorldPos();
			box2.Extents = Vector3(target->S._11, target->S._22, target->S._33);
			return box1.Intersects(box2);
		}
		else if (target->type == ColliderType::OBOX)
		{
			BoundingOrientedBox box2;
			box2.Center = target->GetWorldPos();
			box2.Extents = Vector3(target->S._11, target->S._22, target->S._33);
			box2.Orientation = Quaternion::CreateFromRotationMatrix(target->RT);
			return box1.Intersects(box2);
		}
		else
		{
			BoundingSphere box2;
			box2.Center = target->GetWorldPos();
			box2.Radius = target->scale.x;
			return box1.Intersects(box2);
		}
	}
	return false;
}

bool Collider::Intersect(Vector3 coord)
{
	float Dis;
	bool result = false;
	if (type == ColliderType::BOX)
	{
		BoundingBox box1;
		box1.Center = GetWorldPos();
		box1.Extents = Vector3(S._11, S._22, S._33);

		//XMVECTOR temp = XMLoadFloat3(&coord);
		result = box1.Contains(coord) >= INTERSECTS;
	}
	else if (type == ColliderType::OBOX)
	{
		BoundingBox box1;
		box1.Center = Vector3(0, 0, 0);
		box1.Extents = Vector3(S._11, S._22, S._33);
		Matrix inverse = S.Invert() * W;
		inverse = inverse.Invert();

		coord = Vector3::Transform(coord, inverse);
		result = box1.Contains(coord) >= INTERSECTS;
	}
	else
	{
		BoundingSphere box1;
		box1.Center = GetWorldPos();
		box1.Radius = scale.x;
		result = box1.Contains(coord) >= INTERSECTS;
	}
	return result;
}

bool Collider::Intersect(Ray Ray, Vector3& Hit)
{
	Ray.direction.Normalize();
	float Dis;
	bool result = false;
	if (type == ColliderType::BOX)
	{
		BoundingBox box1;
		box1.Center = GetWorldPos();
		box1.Extents = Vector3(S._11, S._22, S._33);
		result = Ray.Intersects(box1, Dis);
		Hit = Ray.position + Ray.direction * Dis;
	}
	else if (type == ColliderType::OBOX)
	{
		BoundingBox box1;
		box1.Center = Vector3(0, 0, 0);
		box1.Extents = Vector3(S._11, S._22, S._33);
		Matrix inverse = S.Invert() * W;
		inverse = inverse.Invert();
		Ray.position = Vector3::Transform(Ray.position, inverse);
		Ray.direction = Vector3::TransformNormal(Ray.direction, inverse);
		Ray.direction.Normalize();


		result = Ray.Intersects(box1, Dis);
		Hit = Ray.position + Ray.direction * Dis;
		Hit = Vector3::Transform(Hit, inverse.Invert());
	}
	else
	{
		BoundingSphere box1;
		box1.Center = GetWorldPos();
		box1.Radius = scale.x;
		result = Ray.Intersects(box1, Dis);
		Hit = Ray.position + Ray.direction * Dis;
	}

	return result;
}
