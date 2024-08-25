#include "framework.h"

GameObject* GameObject::Create(string name)
{
	GameObject* temp = new GameObject();
	temp->name = name;
	temp->type = ObType::GameObject;
	return temp;
}

GameObject* GameObject::axis = nullptr;
Material* GameObject::defalutMaterial = nullptr;
void GameObject::CreateStaticMember()
{
	axis = new GameObject();
	axis->mesh = RESOURCE->meshes. Load("1.Transform.mesh");
	axis->shader = RESOURCE->shaders.Load("1.Cube.hlsl");
	defalutMaterial = new Material();
}

void GameObject::DeleteStaticMember()
{
	GameObject A;
	
	GameObject* p = new GameObject(A);

	SafeDelete(axis);
	SafeDelete(defalutMaterial);
}

void GameObject::RenderAxis()
{
	if (not GUI->target)return;
	GameObject* target = GUI->target;
	axis->S = Matrix::CreateScale(App.GetHeight() * 0.1f);
	if (not target->parent)
		axis->W = axis->S * target->T;
	else if (worldPos)
		axis->W = axis->S * Matrix::CreateTranslation(target->GetWorldPos());
	else
		axis->W = axis->S * target->T * target->parent->W;

	axis->Render();
}

GameObject::GameObject()
{
	root = nullptr;
	
	visible = true;
	name = " ";

	mesh = nullptr;
	shader = nullptr;
	collider = nullptr;
	material = nullptr;
	skelRoot = nullptr;

	//mesh = Mesh::CreateMesh();

	/*mesh = new Mesh();
	shader = new Shader();
	shader->LoadFile("0.Cube.hlsl");*/
}

GameObject::GameObject(GameObject& src)
{
	if (src.collider)
		collider = new Collider(*src.collider);
	else
		collider = nullptr;

	if (src.material)
		material = new Material(*src.material);
	else
		material = nullptr;

	mesh = src.mesh;
	shader = src.shader;
	type = src.type;
	name = src.name;
	position = src.position;
	scale = src.scale;
	rotation = src.rotation;
	visible = src.visible;
	boneIndex = src.boneIndex;
}

Actor::Actor(Actor& src)
	:GameObject(src)
{
	root = this;
	parent = nullptr;
	obList[src.name] = this;
	type = ObType::Actor;
	file = src.file;
	boneIndex = src.boneIndex;
	boneIndexCount = src.boneIndexCount;

}

GameObject::~GameObject()
{
	SafeReset(mesh);
	SafeReset(shader);
	SafeDelete(collider);
	SafeDelete(material);
}

void GameObject::Release()
{
	for (auto it = children.begin(); it != children.end(); it++)
	{
		SafeRelease(it->second);
	}
	delete this;
}


void GameObject::AddChild(GameObject* child)
{
	if (root->Find(child->name))
		return;

	root->obList[child->name] = child;
	children[child->name] = child;

	child->parent = this;
	child->root = root;
}

void GameObject::AddBone(GameObject* child)
{
	if (root->Find(child->name))
		return;
	root->obList[child->name] = child;
	children[child->name] = child;
	child->parent = this;
	child->root = root;
	child->boneIndex = root->boneIndexCount;
	root->boneIndexCount++;
}

void GameObject::Update()
{
	//내가 위에 스켈레톤이 있을때
	if (skelRoot and boneIndex != -1)
	{
		//애니메이션이 있을떄
		if (skelRoot->anim)
		{
			if (rootMotion)
			{
				Matrix temp = skelRoot->anim->GetFrameBone(boneIndex);
				temp._41 = 0.0f;
				temp._42 = 0.0f;
				temp._43 = 0.0f;
				Transform::UpdateAnim(std::move(temp));
			}
			else
			{
				Transform::UpdateAnim(skelRoot->anim->GetFrameBone(boneIndex));
			}
			
				
		}
		else
		{
			Transform::Update();
		}
		//                      W역행렬 *  W 
		Matrix temp = skelRoot->skeleton->bonesOffset[boneIndex] * W;
		//행우선->열우선
		skelRoot->skeleton->bones[boneIndex] = temp.Transpose();
	}
	else
	{
		Transform::Update();
	}
	if (collider)
		collider->Update(this);

	for (auto it = children.begin(); it != children.end(); it++)
		it->second->Update();



}

void GameObject::Render(shared_ptr<Shader> pShader)
{
	shared_ptr<Shader> shader;

	if (pShader)
		shader = pShader;
	else
		shader = this->shader;

	if (visible)
	{
		if (mesh and shader)
		{
			mesh->Set();
			Transform::Set();

			shader->Set();

			if (material)
				material->Set();
			else
				defalutMaterial->Set();

			mesh->DrawCall();
		}

		for (auto it = children.begin(); it != children.end(); it++)
			it->second->Render(shader);
	}

	if (collider and pShader == nullptr)
	{
		collider->Render();
	}
}

bool GameObject::Intersect(GameObject* target)
{
	if (collider and target->collider)
	{
		return collider->Intersect(target->collider);
	}
	return false;
}

bool GameObject::Intersect(Ray Ray, Vector3& Hit)
{
	if (collider)
	{
		return collider->Intersect(Ray, Hit);
	}
	return false;
}

Actor* Actor::Create(string name)
{
	Actor* temp = new Actor();
	temp->name = name;
	temp->type = ObType::Actor;
	return temp;
}

Actor* Actor::Create(Actor* src)
{
	//루트만 복사
	Actor* temp = new Actor(*src);
	temp->CopyChild(src);
	return temp;
}

Actor::Actor()
{
	root = this;
	skeleton = nullptr;
	anim = nullptr;
}



Actor::~Actor()
{
}


//하위노드 삭제
void Actor::ReleaseMember()
{
	for (auto it = children.begin(); it != children.end(); it++)
	{
		it->second->Release();
	}
	obList.clear();
	children.clear();
}

GameObject* Actor::Find(string name)
{
	auto it = obList.find(name);
	//찾았다
	if (it != obList.end())
	{
		return it->second;
	}
	return nullptr;
}

bool Actor::DeleteObject(string Name)
{
	if (Name == name) return false;

	auto temp = obList.find(Name);

	if (temp == obList.end())  return false;

	//지워질대상,지워질대상의 부모 포인터
	GameObject* Target = temp->second;
	GameObject* Parent = temp->second->parent;
	//부모리스트에서 타겟 비우기
	Parent->children.erase(Parent->children.find(Name));

	for (auto it = Target->children.begin();
		it != Target->children.end(); it++)
	{
		Parent->children[it->second->name] = it->second;
		it->second->parent = Parent;
		//SafeRelease(it->second);
	}
	SafeDelete(Target);
	obList.erase(temp);
	return true;
}

bool Actor::DeleteAllObject(string Name)
{
	//루트삭제 방지
	if (Name == name) return false;

	//리스트에서 찾기
	auto temp = obList.find(Name);
	//없으면 되돌아기
	if (temp == obList.end())  return false;

	//지워질대상,지워질대상의 부모 포인터
	GameObject* Target = temp->second;
	GameObject* Parent = temp->second->parent;
	//부모리스트에서 타겟 비우기
	Parent->children.erase(Parent->children.find(Name));
	root->obList.erase(root->obList.find(Name));

	//number;

	while (1)
	{
		if (Target->children.empty())break;

		auto it = Target->children.begin();

		DeleteAllObject(it->first);
	}
	SafeDelete(Target);
	return true;



}

void Actor::Render(shared_ptr<Shader> pShader)
{
	if (skeleton)
	{
		skeleton->Set();
	}
	GameObject::Render(pShader);
}
