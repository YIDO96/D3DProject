#include "framework.h"
//#include "Sky.h"

Sky* Sky::Create(string name)
{
	Sky* temp = new Sky();
	temp->name = name;
	temp->type = ObType::Sky;
	/*temp->mesh = RESOURCE->meshes.Load("0.Cube.mesh");
	temp->shader = RESOURCE->shaders.Load("0.Sky.hlsl");
	temp->texCube = make_shared<Texture>();
	temp->texCube->LoadFile("sky.dds");*/
	return temp;
}


void Sky::Render(shared_ptr<Shader> pShader)
{
	//���� ����
	DEPTH->Set(false);
	//���� �ѱ�
	BLEND->Set(true);
	//�ո� �߸���
	RASTER->Set(D3D11_CULL_FRONT);
	if(texCube)
	texCube->Set(4);
	Actor::Render(pShader);
	//�޸� �߸���
	RASTER->Set(D3D11_CULL_BACK);
	//�������
	BLEND->Set(false);
	//���� �ѱ�
	DEPTH->Set(true);
}

void Sky::RenderDetail()
{
	Actor::RenderDetail();
	if (ImGui::BeginTabBar("MyTabBar3"))
	{
		if (ImGui::BeginTabItem("Sky"))
		{

			if (GUI->FileImGui("Load texCube", "Load texCube",
				".dds,.jpg,.tga,.png,.bmp", "../Contents/Texture"))
			{
				string path = ImGuiFileDialog::Instance()->GetCurrentPath();
				Utility::Replace(&path, "\\", "/");
				if (path.find("/Texture/") != -1)
				{
					size_t tok = path.find("/Texture/") + 9;
					path = path.substr(tok, path.length())
						+ "/" + ImGuiFileDialog::Instance()->GetCurrentFileName();
				}
				else
				{
					path = ImGuiFileDialog::Instance()->GetCurrentFileName();
				}
				SafeReset(texCube);
				texCube = RESOURCE->textures.Load(path);
			}
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}


}
