#include "stdafx.h"
#include "Scene1.h"



Scene1::Scene1()
{
    grid = Grid::Create();

    cam1 = Camera::Create();
    cam1->LoadFile("Cam.xml");

    Camera::main = cam1;

    vam = Actor::Create();
    vam->LoadFile("Vam.xml");

    terrain = Terrain::Create();
    terrain->LoadFile("Terrain.xml");

    deferred = new Deferred;

    post = UI::Create();
    post->LoadFile("Deferred.xml");
    test = Actor::Create();
    test->LoadFile("Test.xml");
    //RESOURCE->shaders.Load("4.Instance_Deferred.hlsl");

}
Scene1::~Scene1()
{

}

void Scene1::Init()
{

}

void Scene1::Release()
{
}

void Scene1::Update()
{
    deferred->RenderDetail();
    LIGHT->RenderDetail();
    Camera::main->ControlMainCam();
    Camera::main->Update();

    ImGui::Text("FPS: %d", TIMER->GetFramePerSecond());

    ImGui::Begin("Hierarchy");
    grid->RenderHierarchy();
    cam1->RenderHierarchy();
    vam->RenderHierarchy();
    terrain->RenderHierarchy();
    post->RenderHierarchy();
    test->RenderHierarchy();
    ImGui::End();


    grid->Update();
    vam->Update();
    post->Update();
    terrain->Update();
    test->Update();
    

}

void Scene1::LateUpdate()
{
   
}

void Scene1::PreRender()
{
    //디퍼드 렌더타겟 4개에 그림을 렌더링
    Camera::main->Set();
    LIGHT->Set();
    deferred->SetTarget();
    vam->Render(RESOURCE->shaders.Load("4.Instance_Deferred.hlsl"));
    terrain->Render(RESOURCE->shaders.Load("5.Cube_Deferred.hlsl"));
} 

void Scene1::Render()
{
    //디퍼드 렌더타겟에 있던걸 옮겨서 하나의 라이팅계산
    Camera::main->Set();
    LIGHT->Set();
    //grid->Render();
  
    
    deferred->Render();

    //post->Render();
    //vam->Render();
    //terrain->Render();
    //test->Render();
}

void Scene1::ResizeScreen()
{
    cam1->viewport.x = 0.0f;
    cam1->viewport.y = 0.0f;
    cam1->viewport.width = App.GetWidth();
    cam1->viewport.height = App.GetHeight();
    cam1->width = App.GetWidth();
    cam1->height = App.GetHeight();
}
