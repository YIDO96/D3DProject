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
	//깊이 끄기
	DEPTH->Set(false);
	//블렌드 켜기
	BLEND->Set(true);
	//앞면 추리기
	RASTER->Set(D3D11_CULL_FRONT);
	if(texCube)
	texCube->Set(4);
	Actor::Render(pShader);
	//뒷면 추리기
	RASTER->Set(D3D11_CULL_BACK);
	//블렌드끄기
	BLEND->Set(false);
	//깊이 켜기
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
