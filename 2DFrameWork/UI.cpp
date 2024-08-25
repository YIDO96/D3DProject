#include "framework.h"

UI* UI::Create(string name)
{
    UI* temp = new UI();
    temp->name = name;
    temp->type = ObType::UI;
    return temp;
}

void UI::Update()
{
	float left = GetWorldPos().x - S._11;
	float right = GetWorldPos().x + S._11;
	float top = GetWorldPos().y + S._22;
	float bottom = GetWorldPos().y - S._22;
	Vector3 ndcMouse = Utility::MouseToNDC(Camera::main);
	if (left < ndcMouse.x and ndcMouse.x < right
		and bottom < ndcMouse.y and ndcMouse.y < top)
	{
		//함수포인터가 할당 되었다면 실행
		if (mouseOver) mouseOver();

		if (INPUT->KeyDown(VK_LBUTTON))
		{
			Press = true;
			if (mouseDown) mouseDown();
		}
	}

	if (Press)
	{
		if (INPUT->KeyPress(VK_LBUTTON))
		{
			if (mousePress) mousePress();
		}
		if (INPUT->KeyUp(VK_LBUTTON))
		{
			Press = false;

			if (mouseUp) mouseUp();
		}
	}


	Matrix Pi;
	Pi = Matrix::CreateTranslation(pivot.x, pivot.y, 0.0f);
	S = Matrix::CreateScale(scale.x, scale.y, scale.z);
	// Ry*Rx*Rz
	R = Matrix::CreateFromYawPitchRoll(rotation.y, rotation.x, rotation.z);
	T = Matrix::CreateTranslation(position.x, position.y, position.z);
	RT = R * T;
	W = Pi * S * RT;
	if (parent)
	{
		S *= parent->S;
		RT *= parent->RT;
		W *= parent->W;
	}

	for (auto it = children.begin(); it != children.end(); it++)
		it->second->Update();
}

void UI::Render(shared_ptr<Shader> pShader)
{
	BLEND->Set(true);
	DEPTH->Set(false);
	Actor::Render(pShader);
	DEPTH->Set(true);
	BLEND->Set(false);
}

bool UI::MouseOver(Camera* cam)
{
    float left = GetWorldPos().x - S._11 ;
    float right = GetWorldPos().x + S._11 ;
    float top = GetWorldPos().y + S._22 ;
    float bottom = GetWorldPos().y - S._22 ;

	Vector3 ndcMouse = Utility::MouseToNDC(cam);
	if (left < ndcMouse.x and ndcMouse.x < right
		and bottom < ndcMouse.y and ndcMouse.y < top)
	{
		return true;
	}
	return false;
}

void UI::RenderDetail()
{
	Actor::RenderDetail();
	if (ImGui::BeginTabBar("MyTabBar3"))
	{
		if (ImGui::BeginTabItem("UI"))
		{
			if (ImGui::Button("LT"))
			{
				pivot = Vector2(1.0f, -1.0f);
			}
			ImGui::SameLine();
			if (ImGui::Button("T"))
			{
				pivot = Vector2(0.0f, -1.0f);
			}
			ImGui::SameLine();
			if (ImGui::Button("RT"))
			{
				pivot = Vector2(-1.0f, -1.0f);
			}

			if (ImGui::Button("L "))
			{
				pivot = Vector2(1.0f, 0.0f);
			}
			ImGui::SameLine();
			if (ImGui::Button("N"))
			{
				pivot = Vector2(0.0f, 0.0f);
			}
			ImGui::SameLine();
			if (ImGui::Button("R "))
			{
				pivot = Vector2(-1.0f, 0.0f);
			}

			if (ImGui::Button("LB"))
			{
				pivot = Vector2(1.0f, 1.0f);
			}
			ImGui::SameLine();
			if (ImGui::Button("B"))
			{
				pivot = Vector2(0.0f, 1.0f);
			}
			ImGui::SameLine();
			if (ImGui::Button("RB"))
			{
				pivot = Vector2(-1.0f, 1.0f);
			}


			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
}
