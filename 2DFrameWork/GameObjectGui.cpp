#include "framework.h"


bool GameObject::RenderHierarchy()
{
	ImGui::PushID(this);
	if (ImGui::TreeNode(name.c_str()))
	{
		if (ImGui::IsItemToggledOpen() or
			ImGui::IsItemClicked())
		{
			GUI->target = this;
		}


		if (ImGui::Button("addChild"))
		{
			ImGui::OpenPopup("childName");
		}
		if (ImGui::BeginPopup("childName"))
		{
			static char childName[32] = "None";
			ImGui::InputText("childName", childName, 32);
			if (ImGui::Button("G.O"))
			{
				AddChild(GameObject::Create(childName));
			}
			ImGui::SameLine();
			if (ImGui::Button("Actor"))
			{
				AddChild(Actor::Create(childName));
			}
			ImGui::SameLine();
			if (ImGui::Button("Cam"))
			{
				AddChild(Camera::Create(childName));
			}
			ImGui::SameLine();
			if (ImGui::Button("UI"))
			{
				AddChild(UI::Create(childName));
			}
			ImGui::SameLine();
			if (ImGui::Button("PointLight"))
			{
				AddChild(Light::Create(childName,0));
			}
			ImGui::SameLine();
			if (ImGui::Button("SpotLight"))
			{
				AddChild(Light::Create(childName,1));
			}
			ImGui::SameLine();
			if (ImGui::Button("Billboard"))
			{
				AddChild(Billboard::Create(childName));
			}
			ImGui::EndPopup();
		}

		ImGui::SameLine();
		if (ImGui::Button("delete"))
		{
			ImGui::OpenPopup("DeleteChild");
		}
		if (ImGui::BeginPopup("DeleteChild"))
		{
			if (ImGui::Button("DeleteAll"))
			{
				if (parent)
				{
					root->DeleteAllObject(name);
					ImGui::EndPopup();
					ImGui::TreePop();
					ImGui::PopID();
					GUI->target = nullptr;
					return true;
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("DeleteThis"))
			{
				if (parent)
				{
					//root->Find(name)->Release();
					root->DeleteObject(name);
					ImGui::EndPopup();
					ImGui::TreePop();
					ImGui::PopID();
					GUI->target = nullptr;
					return true;
				}
			}
			ImGui::EndPopup();
		}
		for (auto it = children.begin(); it != children.end(); it++)
		{
			if (it->second->RenderHierarchy())
			{
				ImGui::TreePop();
				ImGui::PopID();
				return true;
			}
		}
		ImGui::TreePop();
	}
	ImGui::PopID();
	return false;
}

void Collider::RenderDetail()
{
	switch (type)
	{
	case ColliderType::SPHERE:
	{
		ImGui::Text("Sphere");
		break;
	}
	case ColliderType::BOX:
	{
		ImGui::Text("Box");
		break;
	}
	case ColliderType::OBOX:
	{
		ImGui::Text("OBox");
		break;
	}
	}
	ImGui::Checkbox("isVisible", &visible);
	Transform::RenderDetail();
}

void Transform::RenderDetail()
{
	ImGui::Checkbox("WorldPos", &worldPos);
	if (worldPos)
	{
		//get
		Vector3 wol = GetWorldPos();
		if (ImGui::DragFloat3("WorldPos", (float*)&wol, 0.05f))
			//set
			SetWorldPos(wol);
	}
	else
	{
		Vector3 loc = GetLocalPos();
		if (ImGui::DragFloat3("LocalPos", (float*)&loc, 0.05f))
			SetLocalPos(loc);
	}
	Vector3 temp = rotation / ToRadian;
	if (ImGui::DragFloat3("rotation", (float*)&temp,0.05f))
	{
		rotation = temp * ToRadian;
	}
	ImGui::DragFloat3("scale", (float*)&scale, 0.05f);

}
void GameObject::RenderDetail()
{
	ImGui::Text(name.c_str());
	ImGui::Checkbox("visible", &visible);
	if (ImGui::BeginTabBar("MyTabBar"))
	{
		if (ImGui::BeginTabItem("Transform"))
		{
			Transform::RenderDetail();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Mesh"))
		{
			if (mesh)
			{
				ImGui::Text(mesh->file.c_str());
				mesh->RenderDetail();
			}
			else
			{
				if (ImGui::Button("Create"))
				{
					mesh = Mesh::CreateMesh();
				}
			}
			if (GUI->FileImGui("Save", "Save Mesh",
				".mesh", "../Contents/Mesh"))
			{
				string path = ImGuiFileDialog::Instance()->GetCurrentPath();
				Utility::Replace(&path, "\\", "/");
				if (path.find("/Mesh/") != -1)
				{
					size_t tok = path.find("/Mesh/") + 6;
					path = path.substr(tok, path.length())
						+ "/" + ImGuiFileDialog::Instance()->GetCurrentFileName();
				}
				else
				{
					path = ImGuiFileDialog::Instance()->GetCurrentFileName();
				}
				mesh->SaveFile(path);
			}
			ImGui::SameLine();
			if (GUI->FileImGui("Load", "Load Mesh",
				".mesh", "../Contents/Mesh"))
			{
				string path = ImGuiFileDialog::Instance()->GetCurrentPath();
				Utility::Replace(&path, "\\", "/");
				if (path.find("/Mesh/") != -1)
				{
					size_t tok = path.find("/Mesh/") + 6;
					path = path.substr(tok, path.length())
						+ "/" + ImGuiFileDialog::Instance()->GetCurrentFileName();
				}
				else
				{
					path = ImGuiFileDialog::Instance()->GetCurrentFileName();
				}
				SafeReset(mesh);


				mesh = RESOURCE->meshes.Load(path);

				//mesh->file;
				//mesh.use_count();
				//mesh.reset();
				//mesh = nullptr;
			}
			ImGui::SameLine();
			if (ImGui::Button("Delete"))
			{
				SafeReset(mesh);
			}
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Shader"))
		{

			if (shader)
			{
				ImGui::Text(shader->file.c_str());
			}
			if (GUI->FileImGui("Load", "Load Shader",
				".hlsl", "../Shaders"))
			{
				string path = ImGuiFileDialog::Instance()->GetCurrentPath();
				Utility::Replace(&path, "\\", "/");
				if (path.find("/Shaders/") != -1)
				{
					size_t tok = path.find("/Shaders/") + 9;
					path = path.substr(tok, path.length())
						+ "/" + ImGuiFileDialog::Instance()->GetCurrentFileName();
				}
				else
				{
					path = ImGuiFileDialog::Instance()->GetCurrentFileName();
				}
				SafeReset(shader);
				shader = RESOURCE->shaders.Load(path);
			}
			ImGui::SameLine();
			if (ImGui::Button("Delete"))
			{
				SafeReset(shader);
			}
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Collider"))
		{
			if (collider)
			{
				collider->RenderDetail();
				if (ImGui::Button("Delete"))
				{
					SafeDelete(collider);
				}
			}
			else
			{
				ImGui::Text("Create");
				if (ImGui::Button("Box"))
				{
					collider = new Collider(ColliderType::BOX);
				}
				ImGui::SameLine();
				if (ImGui::Button("OBox"))
				{
					collider = new Collider(ColliderType::OBOX);
				}
				ImGui::SameLine();
				if (ImGui::Button("Sphere"))
				{
					collider = new Collider(ColliderType::SPHERE);
				}
			}
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Material"))
		{
			if (material)
			{
				ImGui::Text(material->file.c_str());
				material->RenderDetail();
			}
			if (GUI->FileImGui("Save", "Save Material",
				".mtl", "../Contents/Material"))
			{
				string path = ImGuiFileDialog::Instance()->GetCurrentPath();
				Utility::Replace(&path, "\\", "/");
				if (path.find("/Material/") != -1)
				{
					size_t tok = path.find("/Material/") + 10;
					path = path.substr(tok, path.length())
						+ "/" + ImGuiFileDialog::Instance()->GetCurrentFileName();
				}
				else
				{
					path = ImGuiFileDialog::Instance()->GetCurrentFileName();
				}
				material->SaveFile(path);
			}
			ImGui::SameLine();
			if (GUI->FileImGui("Load", "Load Material",
				".mtl", "../Contents/Material"))
			{

				string path = ImGuiFileDialog::Instance()->GetCurrentPath();
				Utility::Replace(&path, "\\", "/");
				if (path.find("/Material/") != -1)
				{
					size_t tok = path.find("/Material/") + 10;
					path = path.substr(tok, path.length())
						+ "/" + ImGuiFileDialog::Instance()->GetCurrentFileName();
				}
				else
				{
					path = ImGuiFileDialog::Instance()->GetCurrentFileName();
				}
				SafeDelete(material);
				material = new Material();
				material->LoadFile(path);
			}
			ImGui::SameLine();
			if (ImGui::Button("Create"))
			{
				SafeDelete(material);
				material = new Material();
			}
			ImGui::SameLine();
			if (ImGui::Button("Delete"))
			{
				SafeDelete(material);
				material = nullptr;
			}

			ImGui::EndTabItem();
		}
	

		ImGui::EndTabBar();
	}

}
void Actor::RenderDetail()
{
	GameObject::RenderDetail();
	if (ImGui::BeginTabBar("MyTabBar2"))
	{
		if (root == this)
		{
			if (ImGui::BeginTabItem("Actor"))
			{
				if (GUI->FileImGui("Save", "Save Actor",
					".xml", "../Contents/GameObject"))
				{
					string path = ImGuiFileDialog::Instance()->GetCurrentPath();
					Utility::Replace(&path, "\\", "/");
					if (path.find("/GameObject/") != -1)
					{
						size_t tok = path.find("/GameObject/") + 12;
						path = path.substr(tok, path.length())
							+ "/" + ImGuiFileDialog::Instance()->GetCurrentFileName();
					}
					else
					{
						path = ImGuiFileDialog::Instance()->GetCurrentFileName();
					}
					SaveFile(path);

					//여기에 세이브

				}
				ImGui::SameLine();
				if (GUI->FileImGui("Load", "Load Actor",
					".xml", "../Contents/GameObject"))
				{
					string path = ImGuiFileDialog::Instance()->GetCurrentPath();
					Utility::Replace(&path, "\\", "/");
					if (path.find("/GameObject/") != -1)
					{
						size_t tok = path.find("/GameObject/") + 12;
						path = path.substr(tok, path.length())
							+ "/" + ImGuiFileDialog::Instance()->GetCurrentFileName();
					}
					else
					{
						path = ImGuiFileDialog::Instance()->GetCurrentFileName();
					}
					LoadFile(path);

					//여기에 로드
				}
				ImGui::EndTabItem();
			}
		}

		if (ImGui::BeginTabItem("Skeleton"))
		{
			if (skeleton)
			{
				ImGui::Text(skeleton->file.c_str());
			}
			if (GUI->FileImGui("Load", "Load Skeleton",
				".skel", "../Contents/Skeleton/"))
			{
				string path = ImGuiFileDialog::Instance()->GetCurrentPath();
				Utility::Replace(&path, "\\", "/");
				if (path.find("/Skeleton/") != -1)
				{
					size_t tok = path.find("/Skeleton/") + 10;
					path = path.substr(tok, path.length())
						+ "/" + ImGuiFileDialog::Instance()->GetCurrentFileName();
				}
				else
				{
					path = ImGuiFileDialog::Instance()->GetCurrentFileName();
				}
				SafeDelete(skeleton);
				skeleton = new Skeleton();
				skeleton->LoadFile(path);
			}
			/*ImGui::SameLine();
			if (ImGui::Button("Delete"))
			{
				SafeDelete(skeleton);
			}*/
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Animations"))
		{
			if (anim)
			{
				anim->RenderDetail();
			}

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

}

void Camera::RenderDetail()
{
	Actor::RenderDetail();
	if (ImGui::BeginTabBar("MyTabBar3"))
	{
		if (ImGui::BeginTabItem("Camera"))
		{
			if (ImGui::Button("ChangeMain"))
			{
				Camera::main = this;
			}
			ImGui::DragFloat("CamSpeed", &mainCamSpeed, 0.05f, 1.0f);
			ImGui::Checkbox("ortho", &ortho);
			ImGui::SliderAngle("fov", &fov, 0, 180.0f);
			ImGui::DragFloat("nearZ", &nearZ, 0.05f, 0.00001f);
			ImGui::DragFloat("farZ", &farZ, 0.05f, 0.00001f, 1.0f);
			ImGui::DragFloat("width", &width, 0.05f, 1.0f);
			ImGui::DragFloat("height", &height, 0.05f, 1.0f);
			ImGui::DragFloat("x", &viewport.x, 0.05f, 0.0f);
			ImGui::DragFloat("y", &viewport.y, 0.05f, 0.0f);
			ImGui::DragFloat("w", &viewport.width, 0.05f, 1.0f);
			ImGui::DragFloat("h", &viewport.height, 0.05f, 1.0f);

			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
}

bool Camera::Intersect(Vector3 coord)
{
	BoundingFrustum Frustum;

	Frustum.Origin = GetWorldPos();
	Frustum.Orientation = Quaternion::CreateFromRotationMatrix(R);
	Frustum.RightSlope = tanf(fov);
	Frustum.LeftSlope = -Frustum.RightSlope;
	Frustum.TopSlope = Frustum.RightSlope * App.GetHeight() / App.GetWidth();
	Frustum.BottomSlope = -Frustum.TopSlope;
	Frustum.Near = nearZ;
	Frustum.Far = farZ;

	//Frustum.Contains()

	if (Frustum.Contains(coord) != DISJOINT)
	{
		return true;
	}

	return false;
}

bool Camera::Intersect(Collider* target)
{
	BoundingFrustum Frustum;

	Frustum.Origin = GetWorldPos();
	Frustum.Orientation = Quaternion::CreateFromRotationMatrix(R);
	Frustum.RightSlope = tanf(fov);
	Frustum.LeftSlope = -Frustum.RightSlope;
	Frustum.TopSlope = Frustum.RightSlope * App.GetHeight() / App.GetWidth();
	Frustum.BottomSlope = -Frustum.TopSlope;
	Frustum.Near = nearZ;
	Frustum.Far = farZ;


	if (target->type == ColliderType::BOX)
	{
		BoundingBox box2;
		box2.Center = target->GetWorldPos();
		box2.Extents = Vector3(target->S._11, target->S._22, target->S._33);

		if (Frustum.Contains(box2) != DISJOINT)
		{
			return true;
		}
	}
	else if (target->type == ColliderType::OBOX)
	{
		BoundingOrientedBox box2;
		box2.Center = target->GetWorldPos();
		box2.Extents = Vector3(target->S._11, target->S._22, target->S._33);
		box2.Orientation = Quaternion::CreateFromRotationMatrix(target->RT);
		if (Frustum.Contains(box2) != DISJOINT)
		{
			return true;
		}
	}
	else
	{
		BoundingSphere box2;
		box2.Center = target->GetWorldPos();
		box2.Radius = target->S._11;
		if (Frustum.Contains(box2) != DISJOINT)
		{
			return true;
		}
	}

	return false;
}

void Texture::RenderDetail()
{
	ImGui::Text(file.c_str());
	ImVec2 size(400, 400);
	ImGui::Image((void*)srv, size);

	ImGui::PushID(this);
	if (ImGui::Button("SamplerButton"))
	{
		ImGui::OpenPopup("Sampler");
	}
	if (ImGui::BeginPopup("Sampler"))
	{
		if (ImGui::Button("Filter POINT"))
		{
			SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
			CreateSampler();
		}
		if (ImGui::Button("Filter LINEAR"))
		{
			SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			CreateSampler();
		}
		if (ImGui::Button("AddressU Clamp"))
		{
			SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
			CreateSampler();
		}
		if (ImGui::Button("AddressU Wrap"))
		{
			SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			CreateSampler();
		}
		if (ImGui::Button("AddressU Mirror"))
		{
			SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
			CreateSampler();
		}
		if (ImGui::Button("AddressV Clamp"))
		{
			SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
			CreateSampler();
		}
		if (ImGui::Button("AddressV Wrap"))
		{
			SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			CreateSampler();
		}
		if (ImGui::Button("AddressV Mirror"))
		{
			SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
			CreateSampler();
		}
		ImGui::EndPopup();
	}
	ImGui::PopID();
}