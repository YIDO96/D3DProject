#pragma once

class Main : public Scene
{
private:

	Camera*				cam1;
	Grid*				grid;

	Actor*				temp;

	string				Modelfile;
	string				Animfile;
	const aiScene*		scene;		//모델의 모든데이터 접근 가능
	Assimp::Importer	importer;	//파일을 읽어올객체
public:
	Main();
	~Main();
	virtual void Init() override;
	virtual void Release() override; //해제
	virtual void Update() override;
	virtual void LateUpdate() override;//갱신
	virtual void PreRender() override;
	virtual void Render() override;
	virtual void ResizeScreen() override;

	//					gameobject <-data
	void ReadMesh(GameObject* dest, aiNode* src);

	void ReadMaterial();

	void ReadSkinMesh(GameObject* dest, aiNode* src);
	void ReadBoneNode(GameObject* dest, aiNode* src);
	void ReadBoneData(aiMesh* mesh, vector<class VertexWeights>& vertexWeights);

	Matrix ToMatrix(aiMatrix4x4& value)
	{
		return Matrix
		(
			value.a1, value.b1, value.c1, value.d1,
			value.a2, value.b2, value.c2, value.d2,
			value.a3, value.b3, value.c3, value.d3,
			value.a4, value.b4, value.c4, value.d4
		);
	};
};




#define MAX_WEIGHTS 4
struct VertexWeights
{
	UINT	boneIdx[MAX_WEIGHTS];
	float	boneWeights[MAX_WEIGHTS];


	VertexWeights()
	{
		ZeroMemory(boneIdx, sizeof(UINT) * MAX_WEIGHTS);
		ZeroMemory(boneWeights, sizeof(float) * MAX_WEIGHTS);
	}
	void AddData(UINT boneId, float weight)
	{
		for (UINT i = 0; i < MAX_WEIGHTS; i++)
		{
			if (boneWeights[i] == 0.0f)
			{
				boneIdx[i] = boneId;
				boneWeights[i] = weight;
				return;
			}
		}
	}
	void Normalize()
	{
		float total = 0.0f;
		for (UINT i = 0; i < MAX_WEIGHTS; i++)
		{
			if (boneWeights[i] != 0.0f)
			{
				total += boneWeights[i];
			}
		}
		for (UINT i = 0; i < MAX_WEIGHTS; i++)
		{
			if (boneWeights[i] != 0.0f)
			{
				boneWeights[i] /= total;
			}
		}
	}
};


struct AnimPosition
{
	float time;
	Vector3 pos;
};

struct AnimScale
{
	float time;
	Vector3 scale;
};

struct AnimRotation
{
	float time;
	Quaternion quater;
};

struct AnimNode
{
	string					name;
	//각 벡터 사이즈가 다를수있다.
	vector<AnimPosition>	position;
	vector<AnimScale>		scale;
	vector<AnimRotation>	rotation;
};

namespace Interpolated
{
	//typedef map<string, AnimNode*>::iterator AnimIter;

	Vector3 CalcInterpolatedScaling(AnimNode* iter, float time, int Duration);
	Quaternion CalcInterpolatedRotation(AnimNode* iter, float time, int Duration);
	Vector3 CalcInterpolatedPosition(AnimNode* iter, float time, int Duration);
	int FindScale(AnimNode* iter, float time);
	int FindRot(AnimNode* iter, float time);
	int FindPos(AnimNode* iter, float time);
}