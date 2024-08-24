#include "stdafx.h"
#include "Main.h"

Main::Main()
{

    grid = Grid::Create();

    cam1 = Camera::Create();
    cam1->LoadFile("Cam.xml");
    Camera::main = cam1;

    temp = Actor::Create();


}

Main::~Main()
{

}

void Main::Init()
{
	
}

void Main::Release()
{
   
}

void Main::Update()
{
    Camera::main->ControlMainCam();
    Camera::main->Update();

    ImGui::Begin("Hierarchy");
    grid->RenderHierarchy();
    cam1->RenderHierarchy();
    temp->RenderHierarchy();
    ImGui::End();

    ImGui::Begin("AssetImporter");
    if (GUI->FileImGui("BonelessChicken", "BonelessChicken",
        ".fbx,.obj,.x,.dae", "../Assets"))
    {
        Modelfile = ImGuiFileDialog::Instance()->GetCurrentFileName();
        string path = "../Assets/" + Modelfile;

        importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);
        scene = importer.ReadFile
        (
            path,
            aiProcess_ConvertToLeftHanded
            | aiProcess_Triangulate
            | aiProcess_GenUVCoords
            | aiProcess_GenNormals
            | aiProcess_CalcTangentSpace
        );
        assert(scene != NULL and "Import Error");

        //�޽��� �о�ͼ� ���ͱ���

        temp = Actor::Create("Root");

        //���͸��� �б�
        ReadMaterial();

        temp->AddChild(GameObject::Create(scene->mRootNode->mName.C_Str()));
        //��� �б�
        ReadNode(temp->Find(scene->mRootNode->mName.C_Str()), scene->mRootNode);

        //�޽� �������б�
        ReadMesh(temp->Find(scene->mRootNode->mName.C_Str()), scene->mRootNode);
        importer.FreeScene();
    }

    if (GUI->FileImGui("BoneChicken", "BoneChicken",
        ".fbx,.obj,.x,.dae", "../Assets"))
    {
        Modelfile = ImGuiFileDialog::Instance()->GetCurrentFileName();
        string path = "../Assets/" + Modelfile;

        importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);
        scene = importer.ReadFile
        (
            path,
            aiProcess_ConvertToLeftHanded
            | aiProcess_Triangulate
            | aiProcess_GenUVCoords
            | aiProcess_GenNormals
            | aiProcess_CalcTangentSpace
        );
        assert(scene != NULL and "Import Error");

        //�޽��� �о�ͼ� ���ͱ���

        temp = Actor::Create("Root");
        temp->skeleton = new Skeleton();
        temp->boneIndex = 0;

      

        ////���͸��� �б�
        ReadMaterial();

        temp->AddBone(GameObject::Create(scene->mRootNode->mName.C_Str()));
        //��� �б�
        ReadBoneNode(temp->Find(scene->mRootNode->mName.C_Str()), scene->mRootNode);
        {
            int tok = Modelfile.find_last_of(".");
            string checkPath = "../Contents/Skeleton/" + Modelfile.substr(0, tok);
            if (!PathFileExistsA(checkPath.c_str()))
            {
                CreateDirectoryA(checkPath.c_str(), NULL);
            }

            string filePath = Modelfile.substr(0, tok) + "/";
            temp->skeleton->file = filePath + Modelfile.substr(0, tok) + ".skel";
            temp->skeleton->SaveFile(temp->skeleton->file);

        }

        for (auto it = temp->obList.begin();
            it != temp->obList.end(); it++)
        {
            it->second->skelRoot = temp;
        }
        ////�޽� �������б�
        ReadSkinMesh(temp->Find(scene->mRootNode->mName.C_Str()), scene->mRootNode);
        
        importer.FreeScene();
    }

    if (GUI->FileImGui("AnimationImporter", "AnimationImporter",
        ".fbx,.obj,.x", "../Assets"))
    {
        Animfile = ImGuiFileDialog::Instance()->GetCurrentFileName();
        string path = "../Assets/" + Animfile;
        importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);
        scene = importer.ReadFile
        (
            path,
            aiProcess_ConvertToLeftHanded
            | aiProcess_Triangulate
            | aiProcess_GenUVCoords
            | aiProcess_GenNormals
            | aiProcess_CalcTangentSpace
        );
        assert(scene != NULL and "Import Error");

        if (not temp->anim)
        {
            temp->anim = new Animations();
        }
        //�ִϸ��̼� ����
        for (UINT i = 0; i < scene->mNumAnimations; i++)
        {
            shared_ptr<Animation> Anim = make_shared<Animation>();

            aiAnimation* srcAnim = scene->mAnimations[i];


            size_t tok2 = Animfile.find_last_of(".");
            Anim->file = Animfile.substr(0, tok2) + to_string(i);
            //  0, �����������ӱ��� ����
            Anim->frameMax = (int)srcAnim->mDuration + 1;
            Anim->tickPerSecond = srcAnim->mTicksPerSecond != 0.0 ? (float)srcAnim->mTicksPerSecond : 25.0f;
            Anim->boneMax = temp->boneIndexCount;
            Anim->arrFrameBone = new Matrix * [Anim->frameMax];
            for (UINT j = 0; j < Anim->frameMax; j++)
            {
                Anim->arrFrameBone[j] = new Matrix[temp->boneIndexCount];
            }




            //ä�ΰ��� -> ���� ����
            for (UINT j = 0; j < srcAnim->mNumChannels; j++)
            {
                AnimNode* animNode = new AnimNode();
                aiNodeAnim* srcAnimNode = srcAnim->mChannels[j];

                animNode->name = srcAnimNode->mNodeName.C_Str();
                //Scale
                for (UINT k = 0; k < srcAnimNode->mNumScalingKeys; k++)
                {
                    AnimScale srcScale;
                    srcScale.time = (float)srcAnimNode->mScalingKeys[k].mTime;
                    srcScale.scale.x = (float)srcAnimNode->mScalingKeys[k].mValue.x;
                    srcScale.scale.y = (float)srcAnimNode->mScalingKeys[k].mValue.y;
                    srcScale.scale.z = (float)srcAnimNode->mScalingKeys[k].mValue.z;
                    animNode->scale.push_back(srcScale);
                }
                //Position
                for (UINT k = 0; k < srcAnimNode->mNumPositionKeys; k++)
                {
                    AnimPosition srcPosition;
                    srcPosition.time = (float)srcAnimNode->mPositionKeys[k].mTime;
                    srcPosition.pos.x = (float)srcAnimNode->mPositionKeys[k].mValue.x;
                    srcPosition.pos.y = (float)srcAnimNode->mPositionKeys[k].mValue.y;
                    srcPosition.pos.z = (float)srcAnimNode->mPositionKeys[k].mValue.z;
                    animNode->position.push_back(srcPosition);
                }
                //Rotation
                for (UINT k = 0; k < srcAnimNode->mNumRotationKeys; k++)
                {
                    AnimRotation srcRotation;
                    srcRotation.time = (float)srcAnimNode->mRotationKeys[k].mTime;
                    srcRotation.quater.x = (float)srcAnimNode->mRotationKeys[k].mValue.x;
                    srcRotation.quater.y = (float)srcAnimNode->mRotationKeys[k].mValue.y;
                    srcRotation.quater.z = (float)srcAnimNode->mRotationKeys[k].mValue.z;
                    srcRotation.quater.w = (float)srcAnimNode->mRotationKeys[k].mValue.w;
                    animNode->rotation.push_back(srcRotation);
                }

                GameObject* chanel = temp->Find(animNode->name);
                if (chanel)
                {
                    Matrix S, R, T;
                    Quaternion quter;
                    Vector3 pos, scale;
                    for (UINT k = 0; k < Anim->frameMax; k++)
                    {
                        pos = Interpolated::CalcInterpolatedPosition(animNode, (float)k, Anim->frameMax);
                        scale = Interpolated::CalcInterpolatedScaling(animNode, (float)k, Anim->frameMax);
                        quter = Interpolated::CalcInterpolatedRotation(animNode, (float)k, Anim->frameMax);

                        S = Matrix::CreateScale(scale);
                        R = Matrix::CreateFromQuaternion(quter);
                        T = Matrix::CreateTranslation(pos);
                        Matrix W = S * R * T;
                        Anim->arrFrameBone[k][chanel->boneIndex] = chanel->GetLocal().Invert() * W;
                    }
                }
                //���⼭ ä�γ�(��)

            }
            //���⼭ �ִԳ�
            temp->anim->playList.push_back(Anim);

            //��� �߰��� �ִϸ��̼� ���Ϸ� ����
            {
                size_t tok = Modelfile.find_last_of(".");
                string checkPath = "../Contents/Animation/" + Modelfile.substr(0, tok);
                if (!PathFileExistsA(checkPath.c_str()))
                {
                    CreateDirectoryA(checkPath.c_str(), NULL);
                }

                string filePath = Modelfile.substr(0, tok) + "/";
                Anim->file = filePath + Anim->file + ".anim";
                Anim->SaveFile(Anim->file);
            }

        }

        importer.FreeScene();
    }


    ImGui::End();


    grid->Update();
    temp->Update();
  
}

void Main::LateUpdate()
{
   
    
}
void Main::PreRender()
{
}

void Main::Render()
{
    Camera::main->Set();
    BLEND->Set(false);

    grid->Render();
    temp->Render();
   
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

void Main::ReadMaterial()
{
    for (UINT i = 0; i < scene->mNumMaterials; i++)
    {
        aiMaterial* srcMtl = scene->mMaterials[i];
        
        Material* destMtl = new Material();
        //�̸� -Ű��
        destMtl->file = srcMtl->GetName().C_Str();

        aiColor3D ambient;
        srcMtl->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
        destMtl->ambient.x = ambient.r;
        destMtl->ambient.y = ambient.g;
        destMtl->ambient.z = ambient.b;


        aiColor3D diffuse;
        srcMtl->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
        destMtl->diffuse.x = diffuse.r;
        destMtl->diffuse.y = diffuse.g;
        destMtl->diffuse.z = diffuse.b;


        aiColor3D specular;
        srcMtl->Get(AI_MATKEY_COLOR_SPECULAR, specular);
        destMtl->specular.x = specular.r;
        destMtl->specular.y = specular.g;
        destMtl->specular.z = specular.b;


        aiColor3D emissive;
        srcMtl->Get(AI_MATKEY_COLOR_EMISSIVE, emissive);
        destMtl->emissive.x = emissive.r;
        destMtl->emissive.y = emissive.g;
        destMtl->emissive.z = emissive.b;

        //Shininess
        srcMtl->Get(AI_MATKEY_SHININESS, destMtl->shininess);
        //opacity
        srcMtl->Get(AI_MATKEY_OPACITY, destMtl->opacity);

        //Normal
        {
            aiString aifile;
            string TextureFile;
            aiReturn texFound;
            texFound = srcMtl->GetTexture(aiTextureType_NORMALS, 0, &aifile);
            TextureFile = aifile.C_Str();
            size_t index = TextureFile.find_last_of('/');
            TextureFile = TextureFile.substr(index + 1, TextureFile.length());



            //�ؽ��İ� �ִ�.
            if (texFound == AI_SUCCESS && Modelfile != "")
            {
                destMtl->ambient.w = 1.0f;
                destMtl->normalMap = make_shared<Texture>();

                size_t tok = Modelfile.find_last_of(".");
                string checkPath = "../Contents/Texture/" + Modelfile.substr(0, tok);
                if (!PathFileExistsA(checkPath.c_str()))
                {
                    CreateDirectoryA(checkPath.c_str(), NULL);
                }
                string orgin = "../Assets/" + TextureFile;
                string copy = "../Contents/Texture/" + Modelfile.substr(0, tok) + "/" + TextureFile;
                bool isCheck = true;
                CopyFileA(orgin.c_str(), copy.c_str(), isCheck);

                destMtl->normalMap->LoadFile(Modelfile.substr(0, tok) + "/" + TextureFile);

            }
        }

        //Diffuse
        {
            aiString aifile;
            string TextureFile;
            aiReturn texFound;
            texFound = srcMtl->GetTexture(aiTextureType_DIFFUSE, 0, &aifile);
            TextureFile = aifile.C_Str();
            size_t index = TextureFile.find_last_of('/');
            TextureFile = TextureFile.substr(index + 1, TextureFile.length());

            //�ؽ��İ� �ִ�.
            if (texFound == AI_SUCCESS && Modelfile != "")
            {
                destMtl->diffuse.w = 1.0f;
                destMtl->diffuseMap = make_shared<Texture>();

                size_t tok = Modelfile.find_last_of(".");
                string checkPath = "../Contents/Texture/" + Modelfile.substr(0, tok);
                if (!PathFileExistsA(checkPath.c_str()))
                {
                    CreateDirectoryA(checkPath.c_str(), NULL);
                }
                string orgin = "../Assets/" + TextureFile;
                string copy = "../Contents/Texture/" + Modelfile.substr(0, tok) + "/" + TextureFile;
                bool isCheck = true;
                CopyFileA(orgin.c_str(), copy.c_str(), isCheck);

                destMtl->diffuseMap->LoadFile(Modelfile.substr(0, tok) + "/" + TextureFile);

            }
        }

        //specular
        {
            aiString aifile;
            string TextureFile;
            aiReturn texFound;
            texFound = srcMtl->GetTexture(aiTextureType_SPECULAR, 0, &aifile);
            TextureFile = aifile.C_Str();
            size_t index = TextureFile.find_last_of('/');
            TextureFile = TextureFile.substr(index + 1, TextureFile.length());

            //�ؽ��İ� �ִ�.
            if (texFound == AI_SUCCESS && Modelfile != "")
            {
                destMtl->specular.w = 1.0f;
                destMtl->specularMap = make_shared<Texture>();

                size_t tok = Modelfile.find_last_of(".");
                string checkPath = "../Contents/Texture/" + Modelfile.substr(0, tok);
                if (!PathFileExistsA(checkPath.c_str()))
                {
                    CreateDirectoryA(checkPath.c_str(), NULL);
                }
                string orgin = "../Assets/" + TextureFile;
                string copy = "../Contents/Texture/" + Modelfile.substr(0, tok) + "/" + TextureFile;
                bool isCheck = true;
                CopyFileA(orgin.c_str(), copy.c_str(), isCheck);

                destMtl->specularMap->LoadFile(Modelfile.substr(0, tok) + "/" + TextureFile);

            }
        }

        //emissive
        {
            aiString aifile;
            string TextureFile;
            aiReturn texFound;
            texFound = srcMtl->GetTexture(aiTextureType_EMISSIVE, 0, &aifile);
            TextureFile = aifile.C_Str();
            size_t index = TextureFile.find_last_of('/');
            TextureFile = TextureFile.substr(index + 1, TextureFile.length());

            //�ؽ��İ� �ִ�.
            if (texFound == AI_SUCCESS && Modelfile != "")
            {
                destMtl->emissive.w = 1.0f;
                destMtl->emissiveMap = make_shared<Texture>();

                size_t tok = Modelfile.find_last_of(".");
                string checkPath = "../Contents/Texture/" + Modelfile.substr(0, tok);
                if (!PathFileExistsA(checkPath.c_str()))
                {
                    CreateDirectoryA(checkPath.c_str(), NULL);
                }
                string orgin = "../Assets/" + TextureFile;
                string copy = "../Contents/Texture/" + Modelfile.substr(0, tok) + "/" + TextureFile;
                bool isCheck = true;
                CopyFileA(orgin.c_str(), copy.c_str(), isCheck);

                destMtl->emissiveMap->LoadFile(Modelfile.substr(0, tok) + "/" + TextureFile);

            }
        }






        size_t tok = Modelfile.find_last_of(".");
        string checkPath = "../Contents/Material/" + Modelfile.substr(0, tok);
        if (!PathFileExistsA(checkPath.c_str()))
        {
            CreateDirectoryA(checkPath.c_str(), NULL);
        }

        string filePath = Modelfile.substr(0, tok) + "/";
        destMtl->file = filePath + destMtl->file + ".mtl";
        destMtl->SaveFile(destMtl->file);

    }

}

void Main::ReadNode(GameObject* dest, aiNode* src)
{
    Matrix tempMat = ToMatrix(src->mTransformation);
    Vector3 s, r, t; Quaternion q;
    tempMat.Decompose(s, q, t);
    r = Utility::QuaternionToYawPtichRoll(q);
    dest->scale = s; dest->rotation = r; dest->SetLocalPos(t);

    temp->Update();
   
    for (UINT i = 0; i < src->mNumChildren; i++)
    {
        GameObject* child =
            GameObject::Create(src->mChildren[i]->mName.C_Str());

        //�ӽ�
        child->shader = RESOURCE->shaders.Load("4.Cube.hlsl");

        dest->AddChild(child);

        ReadNode(child, src->mChildren[i]);
    }
}

void Main::ReadBoneNode(GameObject* dest, aiNode* src)
{

    Matrix tempMat = ToMatrix(src->mTransformation);
    Vector3 s, r, t; Quaternion q;
    tempMat.Decompose(s, q, t);
    r = Utility::QuaternionToYawPtichRoll(q);
    dest->scale = s; dest->rotation = r; dest->SetLocalPos(t);

    temp->Update();
    dest->root->skeleton->bonesOffset[dest->boneIndex] = dest->W.Invert();

    for (UINT i = 0; i < src->mNumChildren; i++)
    {
        GameObject* child =
            GameObject::Create(src->mChildren[i]->mName.C_Str());

        //�ӽ�
        child->shader = RESOURCE->shaders.Load("4.Cube.hlsl");

        dest->AddBone(child);

        ReadBoneNode(child, src->mChildren[i]);
    }

}

//���,�ؽ�����ǥ
void Main::ReadMesh(GameObject* dest, aiNode* src)
{
    //��尡 �������ִ� �޽� �� ��ŭ
    for (UINT i = 0; i < src->mNumMeshes; i++)
    {
        UINT index = src->mMeshes[i];
        aiMesh* srcMesh = scene->mMeshes[index];
        
        vector<VertexModel>* VertexList = new vector<VertexModel>();
        vector<UINT>*        indexList = new vector<UINT>;

        string mtlFile = 
            scene->mMaterials[srcMesh->mMaterialIndex]->GetName().C_Str();
        int tok = Modelfile.find_last_of(".");
        string filePath = Modelfile.substr(0, tok) + "/";
        mtlFile = filePath + mtlFile + ".mtl";
        string meshFile = src->mName.C_Str();

        //���� ������ŭ �ø���
        VertexList->resize(srcMesh->mNumVertices);

        //����������ŭ �ݺ�
        for (UINT j = 0; j < VertexList->size(); j++)
        {
            (*VertexList)[j].position.x = srcMesh->mVertices[j].x;
            (*VertexList)[j].position.y = srcMesh->mVertices[j].y;
            (*VertexList)[j].position.z = srcMesh->mVertices[j].z;

            (*VertexList)[j].normal.x = srcMesh->mNormals[j].x;
            (*VertexList)[j].normal.y = srcMesh->mNormals[j].y;
            (*VertexList)[j].normal.z = srcMesh->mNormals[j].z;

            (*VertexList)[j].tangent.x = srcMesh->mTangents[j].x;
            (*VertexList)[j].tangent.y = srcMesh->mTangents[j].y;
            (*VertexList)[j].tangent.z = srcMesh->mTangents[j].z;

            (*VertexList)[j].uv.x = srcMesh->mTextureCoords[0][j].x; 
            (*VertexList)[j].uv.y = srcMesh->mTextureCoords[0][j].y; 
        }
        UINT IndexCount = srcMesh->mNumFaces;
        for (UINT j = 0; j < IndexCount; j++)
        {
            aiFace& face = srcMesh->mFaces[j];
            for (UINT k = 0; k < face.mNumIndices; k++)
            {
                (*indexList).push_back(face.mIndices[k]);
            }
        }
        if (i == 0)
        {
            dest->mesh = make_shared<Mesh>(&(*VertexList)[0], VertexList->size(),
                &(*indexList)[0], indexList->size(), VertexType::MODEL);

            dest->material = new Material();
            dest->material->LoadFile(mtlFile);

            {
                int tok = Modelfile.find_last_of(".");
                string checkPath = "../Contents/Mesh/" + Modelfile.substr(0, tok);
                if (!PathFileExistsA(checkPath.c_str()))
                {
                    CreateDirectoryA(checkPath.c_str(), NULL);
                }

                string filePath = Modelfile.substr(0, tok) + "/";
                dest->mesh->file = filePath + meshFile + ".mesh";
                dest->mesh->SaveFile(dest->mesh->file);
            }
          

        }
        else
        {
            string Name = srcMesh->mName.C_Str() + string("MeshObject") + to_string(i);
            GameObject* temp2 = GameObject::Create(Name);
            dest->AddChild(temp2);
            temp2->shader = RESOURCE->shaders.Load("4.Cube.hlsl");
            temp2->mesh = make_shared<Mesh>(&(*VertexList)[0], VertexList->size(),
                &(*indexList)[0], indexList->size(), VertexType::MODEL);
            temp2->material = new Material();
            temp2->material->LoadFile(mtlFile);

            {
                int tok = Modelfile.find_last_of(".");
                string checkPath = "../Contents/Mesh/" + Modelfile.substr(0, tok);
                if (!PathFileExistsA(checkPath.c_str()))
                {
                    CreateDirectoryA(checkPath.c_str(), NULL);
                }

                string filePath = Modelfile.substr(0, tok) + "/";
                temp2->mesh->file = filePath + Name + ".mesh";
                temp2->mesh->SaveFile(temp2->mesh->file);
            }
        }

    }
    for (UINT i = 0; i < src->mNumChildren; i++)
    {
        ReadMesh(dest->children[src->mChildren[i]->mName.C_Str()],
            src->mChildren[i]);
    }
}

void Main::ReadSkinMesh(GameObject* dest, aiNode* src)
{
    //��尡 �������ִ� �޽� �� ��ŭ
    for (UINT i = 0; i < src->mNumMeshes; i++)
    {
        UINT index = src->mMeshes[i];
        aiMesh* srcMesh = scene->mMeshes[index];

        vector<VertexModel>* VertexList = new vector<VertexModel>();
        vector<UINT>* indexList = new vector<UINT>;
        vector<VertexWeights>	VertexWeights;

        string mtlFile =
            scene->mMaterials[srcMesh->mMaterialIndex]->GetName().C_Str();
        int tok = Modelfile.find_last_of(".");
        string filePath = Modelfile.substr(0, tok) + "/";
        mtlFile = filePath + mtlFile + ".mtl";
        string meshFile = src->mName.C_Str();

        //���� ������ŭ �ø���
        VertexList->resize(srcMesh->mNumVertices);
        VertexWeights.resize(srcMesh->mNumVertices);

        ReadBoneData(srcMesh, VertexWeights);

        //����������ŭ �ݺ�
        for (UINT j = 0; j < VertexList->size(); j++)
        {
            (*VertexList)[j].position.x = srcMesh->mVertices[j].x;
            (*VertexList)[j].position.y = srcMesh->mVertices[j].y;
            (*VertexList)[j].position.z = srcMesh->mVertices[j].z;

            (*VertexList)[j].normal.x = srcMesh->mNormals[j].x;
            (*VertexList)[j].normal.y = srcMesh->mNormals[j].y;
            (*VertexList)[j].normal.z = srcMesh->mNormals[j].z;

            (*VertexList)[j].tangent.x = srcMesh->mTangents[j].x;
            (*VertexList)[j].tangent.y = srcMesh->mTangents[j].y;
            (*VertexList)[j].tangent.z = srcMesh->mTangents[j].z;

            (*VertexList)[j].uv.x = srcMesh->mTextureCoords[0][j].x;
            (*VertexList)[j].uv.y = srcMesh->mTextureCoords[0][j].y;

            //�������Ͱ� ������
            if (!VertexWeights.empty())
            {
                VertexWeights[j].Normalize();

                (*VertexList)[j].indices.x = (float)VertexWeights[j].boneIdx[0];
                (*VertexList)[j].indices.y = (float)VertexWeights[j].boneIdx[1];
                (*VertexList)[j].indices.z = (float)VertexWeights[j].boneIdx[2];
                (*VertexList)[j].indices.w = (float)VertexWeights[j].boneIdx[3];

                (*VertexList)[j].weights.x = VertexWeights[j].boneWeights[0];
                (*VertexList)[j].weights.y = VertexWeights[j].boneWeights[1];
                (*VertexList)[j].weights.z = VertexWeights[j].boneWeights[2];
                (*VertexList)[j].weights.w = VertexWeights[j].boneWeights[3];
            }


        }
        UINT IndexCount = srcMesh->mNumFaces;
        for (UINT j = 0; j < IndexCount; j++)
        {
            aiFace& face = srcMesh->mFaces[j];
            for (UINT k = 0; k < face.mNumIndices; k++)
            {
                (*indexList).push_back(face.mIndices[k]);
            }
        }
        if (i == 0)
        {
            dest->mesh = make_shared<Mesh>(&(*VertexList)[0], VertexList->size(),
                &(*indexList)[0], indexList->size(), VertexType::MODEL);

            dest->material = new Material();
            dest->material->LoadFile(mtlFile);

            {
                int tok = Modelfile.find_last_of(".");
                string checkPath = "../Contents/Mesh/" + Modelfile.substr(0, tok);
                if (!PathFileExistsA(checkPath.c_str()))
                {
                    CreateDirectoryA(checkPath.c_str(), NULL);
                }

                string filePath = Modelfile.substr(0, tok) + "/";
                dest->mesh->file = filePath + meshFile + ".mesh";
                dest->mesh->SaveFile(dest->mesh->file);
            }
        }
        else
        {
            string Name = srcMesh->mName.C_Str() + string("MeshObject") + to_string(i);
            GameObject* temp2 = GameObject::Create(Name);
            dest->AddChild(temp2);
            temp2->shader = RESOURCE->shaders.Load("4.Cube.hlsl");
            temp2->mesh = make_shared<Mesh>(&(*VertexList)[0], VertexList->size(),
                &(*indexList)[0], indexList->size(), VertexType::MODEL);
            temp2->material = new Material();
            temp2->material->LoadFile(mtlFile);
            {
                int tok = Modelfile.find_last_of(".");
                string checkPath = "../Contents/Mesh/" + Modelfile.substr(0, tok);
                if (!PathFileExistsA(checkPath.c_str()))
                {
                    CreateDirectoryA(checkPath.c_str(), NULL);
                }

                string filePath = Modelfile.substr(0, tok) + "/";
                temp2->mesh->file = filePath + Name + ".mesh";
                temp2->mesh->SaveFile(temp2->mesh->file);
            }
        }

      
    }
    for (UINT i = 0; i < src->mNumChildren; i++)
    {
        ReadSkinMesh(dest->children[src->mChildren[i]->mName.C_Str()],
            src->mChildren[i]);
    }
}

void Main::ReadBoneData(aiMesh* mesh, vector<class VertexWeights>& vertexWeights)
{
    //�޽��� ������ �ִ� �� ���� ��ŭ
    for (UINT i = 0; i < mesh->mNumBones; i++)
    {
        //���纻�� ���̾����Ű���� ���° �ε����ΰ�?
        string boneName = mesh->mBones[i]->mName.C_Str();
        int boneIndex = temp->Find(boneName)->boneIndex;
        for (UINT j = 0; j < mesh->mBones[i]->mNumWeights; j++)
        {
            UINT vertexID = mesh->mBones[i]->mWeights[j].mVertexId;
            vertexWeights[vertexID].AddData(boneIndex, mesh->mBones[i]->mWeights[j].mWeight);
        }
    }
}

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE prevInstance, LPWSTR param, int command)
{
    App.SetAppName(L"Game1");
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


Vector3 Interpolated::CalcInterpolatedScaling(AnimNode* iter, float time, int Duration)
{
    //��������� 1,1,1��ȯ
    if (iter->scale.empty())
        return Vector3(1.0f, 1.0f, 1.0f);
    //�Ѱ��� �ִ³��� ù��° �� ��ȯ
    if (iter->scale.size() == 1)
        return iter->scale.front().scale;
    //������ ���� �������� ��ȯ
    if (time == Duration - 1)
    {
        return iter->scale.back().scale;
    }
    //���� ����
    int scaling_index = FindScale(iter, time);
    //-1 �ε����� �����Ƿ� ������ ������ ��ȯ
    if (scaling_index == -1)
    {
        return iter->scale.back().scale;
    }
    //���� ��
    UINT next_scaling_index = scaling_index + 1;
    assert(next_scaling_index < iter->scale.size());

    //���� ���� �ð���
    float delta_time = (float)(iter->scale[next_scaling_index].time
        - iter->scale[scaling_index].time);
    //������
    float factor = (time - (float)(iter->scale[scaling_index].time)) / delta_time;
    //�굵 �ϳ��� �ִ¾�
    if (factor < 0.0f)
    {
        return iter->scale.front().scale;
    }

    auto start = iter->scale[scaling_index].scale;
    auto end = iter->scale[next_scaling_index].scale;

    start = Vector3::Lerp(start, end, factor);
    return start;
}
Quaternion Interpolated::CalcInterpolatedRotation(AnimNode* iter, float time, int Duration)
{
    //auto rot_frames = iter->second->rotation;

    if (iter->rotation.empty())
        return Quaternion(0.0f, 0.0f, 0.0f, 1.0f);

    if (iter->rotation.size() == 1)
        return iter->rotation.front().quater;

    if (time == Duration - 1)
    {
        return iter->rotation.back().quater;
    }

    int quter_index = FindRot(iter, time);
    if (quter_index == -1)
    {
        return iter->rotation.back().quater;
    }
    UINT next_quter_index = quter_index + 1;
    assert(next_quter_index < iter->rotation.size());

    float delta_time = static_cast<float>(iter->rotation[next_quter_index].time - iter->rotation[quter_index].time);
    float factor = (time - static_cast<float>(iter->rotation[quter_index].time)) / delta_time;
    if (factor < 0.0f)
    {
        return iter->rotation.front().quater;
    }

    auto start = iter->rotation[quter_index].quater;
    auto end = iter->rotation[next_quter_index].quater;

    start = Quaternion::Slerp(start, end, factor);
    return start;
}
Vector3 Interpolated::CalcInterpolatedPosition(AnimNode* iter, float time, int Duration)
{
    //auto pos_frames = iter->second->position;

    if (iter->position.empty())
        return Vector3(0.0f, 0.0f, 0.0f);

    if (iter->position.size() == 1)
        return iter->position.front().pos;

    if (time == Duration - 1)
    {
        return iter->position.back().pos;
    }

    int position_index = FindPos(iter, time);

    if (position_index == -1)
    {
        return iter->position.back().pos;
    }

    UINT next_position_index = position_index + 1;
    assert(next_position_index < iter->position.size());

    float delta_time = static_cast<float>(iter->position[next_position_index].time - iter->position[position_index].time);
    float factor = (time - static_cast<float>(iter->position[position_index].time)) / delta_time;

    if (factor < 0.0f)
    {
        return iter->position.front().pos;
    }

    auto start = iter->position[position_index].pos;
    auto end = iter->position[next_position_index].pos;

    start = Vector3::Lerp(start, end, factor);
    return start;
}


int Interpolated::FindScale(AnimNode* iter, float time)
{
    //vector<AnimScale> scale_frames = iter->second->scale;

    if (iter->scale.empty())
        return -1;//-1 �� ��ȯ�ϸ� ������ ������ ��

    for (UINT i = 0; i < iter->scale.size() - 1; i++)
    {
        if (time < (float)(iter->scale[i + 1].time))
            return i;
    }

    return -1;//-1 �� ��ȯ�ϸ� ������ ������ ��
}
int Interpolated::FindRot(AnimNode* iter, float time)
{
    //auto pos_frames = iter->second->rotation;

    if (iter->rotation.empty())
        return -1;

    for (UINT i = 0; i < iter->rotation.size() - 1; i++)
    {
        if (time < static_cast<float>(iter->rotation[i + 1].time))
            return i;
    }

    return -1;
}
int Interpolated::FindPos(AnimNode* iter, float time)
{
    //auto pos_frames = iter->second->position;

    if (iter->position.empty())
        return -1;

    for (UINT i = 0; i < iter->position.size() - 1; i++)
    {
        if (time < static_cast<float>(iter->position[i + 1].time))
            return i;
    }

    return -1;
}

