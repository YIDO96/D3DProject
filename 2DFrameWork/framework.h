#pragma once
//Popup Console
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console") 

//C
#include <Windows.h>
#include <assert.h>

//C++
#include <string>
#include <vector>
#include <deque>
#include <chrono>
#include <random>
#include <iostream>
#include <unordered_map>
#include <queue>
using namespace std;

//DirectX d3d 11
#include <d3d11.h>
#include <dxgi1_2.h>
#include <d3dcompiler.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

//D2D
#include <d2d1_1.h>
#pragma comment(lib, "d2d1.lib")

//Direct Write
#include <dwrite.h>
#pragma comment(lib, "dwrite.lib")

//DirectXTk 
#include "../Libs/DirectXTK/SimpleMath.h"
using namespace DirectX;
using namespace SimpleMath;

//DirectXTex
#include "../Libs/DirectXTex/DirectXTex.h"
#pragma comment(lib, "../Libs/DirectXTex/DirectXTex.lib")

//Fmod
#include "../Libs/Fmod/fmod.hpp"
#pragma comment(lib, "../Libs/Fmod/fmodL_vc.lib")

//Imgui
#include "../Libs/ImGui/imgui.h"
#include "../Libs/ImGui/imgui_internal.h"
#include "../Libs/ImGui/imgui_impl_dx11.h"
#include "../Libs/ImGui/imgui_impl_win32.h"
#pragma comment(lib, "../Libs/ImGui/example_win32_directx11.lib")

//Dialog
#include "../Libs/ImGui/dirent.h"
#include "../Libs/ImGui/ImGuiFileDialog.h"
#include "../Libs/ImGui/ImGuiFileDialogConfig.h"

//TinyXML2
#include "Xml.h"

//Macro(Pascal)
#define Check(hr)			{ assert(SUCCEEDED(hr)); }
#define SafeRelease(p)		{ if(p){ (p)->Release(); (p) = nullptr; } }
#define SafeDelete(p)		{ if(p){ delete (p); (p) = nullptr; } }
#define SafeDeleteArray(p)	{ if(p){ delete[] (p); (p) = nullptr; } }
#define SafeReset(p)		{ if(p){ (p).reset(); (p) = nullptr; } }

#define	ToRadian			0.0174532f
//Macro(WINAPI Style)
#define	PI					3.1415926f	//180
#define	HALFPI				1.5707963f	//90
#define	QUATERPI			0.7853981f	//45

//2DFramework Header
#include "Types.h"
#include "Window.h"
extern Application App;//Extern Global
#include "Direct3D11.h"
#include "Gui.h"
#include "Input.h"
#include "Timer.h"
#include "BinaryFile.h"

#include "DepthState.h"
#include "BlendState.h"
#include "RasterState.h"

#include "Random.h"
#include "Texture.h"
#include "Sound.h"

#include "Dwrite.h"
#include "SceneManager.h"

#include "VertexType.h"
#include "Shader.h"
#include "Mesh.h"
#include "Transform.h"
#include "Collider.h"
#include "Material.h"
#include "Skeleton.h"
#include "Animation.h"
#include "RenderTarget.h"
#include "CubeRenderTarget.h"
#include "Deferred.h"


#include "ResouceManager.h"
#include "GameObject.h"
#include "Grid.h"
#include "Camara.h"
#include "Terrain.h"
#include "AStar.h"
#include "Sky.h"
#include "UI.h"
#include "PostEffect.h"

#include "Light.h"
#include "Billboard.h"
#include "SlashTrail.h"
#include "Rain.h"
#include "Pop.h"
#include "Water.h"


#include "Utility.h"

//Singleton Macro
#define	D3D			Direct3D11::GetInstance()
#define	WIN			Window::GetInstance()
#define	GUI			Gui::GetInstance()
#define INPUT		Input::GetInstance()
#define TIMER		Timer::GetInstance()
#define DELTA		Timer::GetInstance()->GetDeltaTime()
#define RANDOM		Random::GetInstance()
#define TEXTURE		Texture::GetInstance()
#define SOUND		Sound::GetInstance()
#define LIGHT		LightManager::GetInstance()
#define DWRITE		Dwrite::GetInstance()
#define SCENE       SceneManager::GetInstance()
#define DEPTH		DepthState::GetInstance()
#define BLEND		BlendState::GetInstance()
#define RESOURCE	ResourceManager::GetInstance()
#define RASTER	    RasterState::GetInstance()