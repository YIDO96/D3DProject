#include "stdafx.h"
#include "Main.h"

Main::Main()
{
	

	//투명 이미지출력 경호형123123
}

Main::~Main()
{
	
}
void Main::Init()
{
	grid = Grid::Create();

	cam1 = Camera::Create();
	cam1->LoadFile("Cam.xml");
	Camera::main = cam1;


}

void Main::Release()
{
}

void Main::Update()

{
	ImGui::Begin("Hierarchy");
	grid->RenderHierarchy();
	ImGui::End();

	cam1->ControlMainCam();

	cam1->Update();
	grid->Update();
}

void Main::LateUpdate()
{
	

}
void Main::PreRender()
{
}

void Main::Render()
{
	cam1->Set();
	LIGHT->Set();
	
	grid->Render();
	
}

void Main::ResizeScreen()
{
	cam1->viewport.x = 0.0f;
	cam1->viewport.y = 0.0f;
	cam1->viewport.width = App.GetWidth();
	cam1->viewport.height = App.GetHeight();
	cam1->width = App.GetWidth();
	cam1->height = App.GetHeight();
}

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE prevInstance, LPWSTR param, int command)
{
	App.SetAppName(L"Game2");
	App.SetInstance(instance);
	WIN->Create();
	D3D->Create();
	Main* main = new Main();
	main->Init();

	int wParam = (int)WIN->Run(main);


	main->Release();
	SafeDelete(main);
	D3D->DeleteSingleton();
	WIN->DeleteSingleton();

	return wParam;
}