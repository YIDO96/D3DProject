#include "framework.h"

Application	 App;
Scene* Window::main = nullptr;

WPARAM Window::Run(Scene* main)
{
	Window::main = main;
	Transform::CreateStaticMember();
	GameObject::CreateStaticMember();
	Camera::CreateStaticMember();
	Material::CreateStaticMember();
	Skeleton::CreateStaticMember();
	Terrain::CreateStaticMember();
	CubeRenderTarget::CreateStaticMember();
	Rain::CreateStaticMember();
	Pop::CreateStaticMember();
	MSG msg = { 0 };
	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{


			TIMER->Chronometry(App.fixFrame);
			INPUT->Update();
			GUI->Update();
			SOUND->Update();
			main->Update();
			main->LateUpdate();
			main->PreRender();

			D3D->SetRenderTarget();
			DWRITE->GetDC()->BeginDraw();
			D3D->Clear(App.background);
			{

				main->Render();
				
				DEPTH->Set(false);
				GameObject::RenderAxis();
				DEPTH->Set(true);
				GUI->Render();
			}
			DWRITE->GetDC()->EndDraw();
			D3D->Present();
		}
	}
	WIN->Save();
	Transform::DeleteStaticMember();
	Camera::DeleteStaticMember();
	GameObject::DeleteStaticMember();
	Material::DeleteStaticMember();
	Skeleton::DeleteStaticMember();
	Terrain::DeleteStaticMember();
	CubeRenderTarget::DeleteStaticMember();
	Rain::DeleteStaticMember();
	Pop::DeleteStaticMember();
	TIMER->DeleteSingleton();
	INPUT->DeleteSingleton();
	GUI->DeleteSingleton();
	RANDOM->DeleteSingleton();
	DWRITE->DeleteSingleton();
	SOUND->DeleteSingleton();
	Destroy();

	return msg.wParam;
}

void Window::Load()
{
	ifstream fin;
	string file = "window.ini";
	fin.open(file.c_str(), ios::in);
	string temp;
	if (fin.is_open())
	{
		string temp;
		fin >> temp >> App.width >> App.height;
		fin >> temp >> App.x >> App.y;
		fin.close();
	}

	RECT rect = { 0, 0, (LONG)App.GetWidth(), (LONG)App.GetHeight() };
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
	MoveWindow
	(
		App.handle
		, (LONG)App.x, (LONG)App.y
		, rect.right - rect.left, rect.bottom - rect.top
		, TRUE
	);
}

void Window::Save()
{
	RECT rc;
	GetWindowRect(App.handle, &rc);
	App.x = (float)rc.left;
	App.y = (float)rc.top;
	ofstream fout;
	string file = "window.ini";
	fout.open(file.c_str(), ios::out);
	if (fout.is_open())
	{
		fout << "Size " << App.width << " " << App.height << endl;
		fout << "Pos " << App.x << " " << App.y << endl;
		fout.close();
	}
}

void Window::Create()
{

	WNDCLASSEXW wndClass;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wndClass.hIconSm = wndClass.hIcon;
	wndClass.hInstance = App.instance;
	wndClass.lpfnWndProc = (WNDPROC)WndProc;
	wndClass.lpszClassName = App.appName.c_str();
	wndClass.lpszMenuName = NULL;
	wndClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndClass.cbSize = sizeof(WNDCLASSEX);

	WORD wHr = RegisterClassExW(&wndClass);
	assert(wHr != 0);

	if (App.fullScreen == true)
	{
		DEVMODE devMode = { 0 };
		devMode.dmSize = sizeof(DEVMODE);
		devMode.dmPelsWidth = (DWORD)App.GetWidth();
		devMode.dmPelsHeight = (DWORD)App.GetHeight();
		devMode.dmBitsPerPel = 32;
		devMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		ChangeDisplaySettings(&devMode, CDS_FULLSCREEN);
	}
	App.handle = CreateWindowExW
	(
		WS_EX_APPWINDOW
		, App.appName.c_str()
		, App.appName.c_str()
		, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW
		, CW_USEDEFAULT
		, CW_USEDEFAULT
		, CW_USEDEFAULT
		, CW_USEDEFAULT
		, NULL
		, (HMENU)NULL
		, App.instance
		, NULL
	);
	assert(App.handle != NULL);


	ShowWindow(App.handle, SW_SHOWNORMAL);
	SetForegroundWindow(App.handle);
	SetFocus(App.handle);

	ShowCursor(true);
	WIN->Load();
}

void Window::Destroy()
{

	if (App.fullScreen == true)
		ChangeDisplaySettings(NULL, 0);

	DestroyWindow(App.handle);

	UnregisterClass(App.appName.c_str(), App.instance);
}


LRESULT Window::WndProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
{
	INPUT->InputProc(message, wParam, lParam);

	if (Gui::MsgProc(handle, message, wParam, lParam))
		return true;

	if (message == WM_SIZE)
	{
		if (D3D->GetCreated())
		{
			float width = (float)LOWORD(lParam);
			float height = (float)HIWORD(lParam);
			D3D->ResizeScreen(width, height);
			GUI->ResizeScreen();
			if (main)
				main->ResizeScreen();
		}
	}
	if (message == WM_KEYDOWN)
	{
		if (wParam == VK_ESCAPE)
		{
			//exit(0);
			PostQuitMessage(0);
		}
	}

	if (message == WM_CLOSE || message == WM_DESTROY)
	{
		PostQuitMessage(0);

		return 0;
	}

	return DefWindowProc(handle, message, wParam, lParam);
}

