#include "stdafx.h"
#include "Main.h"

Main::Main()
{
	D3D11_BUFFER_DESC desc = { 0 };
	desc.ByteWidth = sizeof(Brush);
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;//상수버퍼
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;
	HRESULT hr = D3D->GetDevice()->CreateBuffer(&desc, NULL, &brushBuffer);
	assert(SUCCEEDED(hr));
	D3D->GetDC()->PSSetConstantBuffers(10, 1, &brushBuffer);

	//투명 이미지출력
}

Main::~Main()
{

}


void Main::Init()
{
	grid = Grid::Create();

	cam1 = Camera::Create();
	cam1->LoadFile("Cam.xml");

	map = Terrain::Create();
	map->shader = RESOURCE->shaders.Load("5.MapEditor.hlsl");
	map->CreateStructuredBuffer();

	Camera::main = cam1;
	cam1->viewport.x = 0.0f;
	cam1->viewport.y = 0.0f;
	cam1->viewport.width = App.GetWidth();
	cam1->viewport.height = App.GetHeight();
	cam1->width = App.GetWidth();
	cam1->height = App.GetHeight();
}

void Main::Release()
{
}

void Main::Update()

{
	LIGHT->RenderDetail();

	ImGui::Combo("brushShape", (int*)&brushShape, "Circle\0Rect\0");
	ImGui::Combo("brushType", (int*)&brushType, "Linear\0Smooth\0Add\0Flat\0");

	

	ImGui::SliderFloat("BrushRange", &_brush.range, 1, 100);
	ImGui::SliderFloat("buildSpeed", &buildSpeed,-100, 100);

	if (ImGui::Button("paint0"))
	{
		paint = Vector4(1, -1, -1, -1);
	}
	if (ImGui::Button("paint1"))
	{
		paint = Vector4(-1, 1, -1, -1);
	}
	if (ImGui::Button("paint2"))
	{
		paint = Vector4(-1, -1, 1, -1);
	}
	if (ImGui::Button("paint3"))
	{
		paint = Vector4(-1, -1, -1, 1);
	}
	ImGui::SliderFloat("paintSpeed", &paintSpeed,0.001f, 3.0f);


	ImGui::Begin("Hierarchy");
	map->RenderHierarchy();
	ImGui::End();

	cam1->ControlMainCam();

	grid->Update();
	map->Update();

	cam1->Update();

	


}

void Main::LateUpdate()
{
	Vector3 Hit;
	if(map->ComPutePicking(Utility::MouseToRay(), Hit))
	{
		_brush.point = Hit;

		if (INPUT->KeyPress(VK_MBUTTON))
		{
			VertexTerrain* vertices = (VertexTerrain*)map->mesh->vertices;
			Matrix Inverse = map->W.Invert();
			Hit = Vector3::Transform(Hit, Inverse);
			for (UINT i = 0; i < map->mesh->vertexCount; i++)
			{
				Vector3 v1 = Vector3(Hit.x, 0.0f, Hit.z);
				Vector3 v2 = Vector3(vertices[i].position.x,
					0.0f, vertices[i].position.z);
				Vector3 temp = v2 - v1;
				float Dis = temp.Length();
				float w;

				if (brushShape == BrushShape::Rect)
				{
					float DisX = fabs(v1.x - v2.x);
					float DisZ = fabs(v1.z - v2.z);

					if (DisX < _brush.range and
						DisZ < _brush.range)
					{
						//nomalize
						//w = Dis / (_brush.range * 1.414f);
						// 
						w = ((DisX > DisZ) ? DisX : DisZ) / _brush.range;



						// 0 ~ 1
						w = Utility::Saturate(w);

						w = (1.0f - w);
					}
					else
					{
						w = 0.0f;
					}
				}
				if (brushShape == BrushShape::Circle)
				{
					w = Dis / _brush.range;
					w = Utility::Saturate(w);
					w = (1.0f - w);
				}
				






				if (brushType == BrushType::Linear)
				{
					if (w)
					{
						vertices[i].position.y += w * buildSpeed * DELTA;
					}
				}
				if (brushType == BrushType::Smooth)
				{
					if (w)
					{
						vertices[i].position.y += sin(w * PI * 0.5f) * buildSpeed * DELTA;
					}
				}
				if (brushType == BrushType::Add)
				{
					if (w)
					{
						vertices[i].position.y += buildSpeed * DELTA;
					}
				}
				if (brushType == BrushType::Flat)
				{
					if (w)
					{
						float dd = vertices[i].position.y > Hit.y ? -1 : 1;
						vertices[i].position.y += w * dd * buildSpeed * DELTA;
					}
				}

				if (w)
				{
					vertices[i].weights += paint *w * paintSpeed * DELTA;
					NormalizeWeight(vertices[i].weights);

				}


			}

			map->mesh->UpdateBuffer();
			map->UpdateNormal();
		}
		if (INPUT->KeyUp(VK_MBUTTON))
		{
			map->UpdateNormal();
			map->UpdateStructuredBuffer();
		}

	}





}
void Main::PreRender()
{
}

void Main::Render()
{
	_brush.shape = (int)brushShape;
	_brush.type = (int)brushType;

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	D3D->GetDC()->Map(brushBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy_s(mappedResource.pData, sizeof(Brush), &_brush, sizeof(Brush));
	D3D->GetDC()->Unmap(brushBuffer, 0);

	LIGHT->Set();
	cam1->Set();
	//grid->Render();
	map->Render();
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

void Main::NormalizeWeight(Vector4& in)
{
	float* weight = (float*)&in;
	float	sum = 0;
	for (int i = 0; i < 4; i++)
	{
		weight[i] = Utility::Saturate(weight[i]);
		sum += weight[i];
	}
	for (int i = 0; i < 4; i++)
	{
		weight[i] /= sum;
	}
}

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE prevInstance, LPWSTR param, int command)
{
	App.SetAppName(L"MapEditor");
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