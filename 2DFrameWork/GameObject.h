#pragma once

enum class ObType
{
	GameObject,
	Actor,
	Camera,
	Terrain,
	UI,
	Sky,
	Light,
	Billboard,
	Rain,
	Pop,
	Water
};

class GameObject : public Transform
{
	friend class Actor;
	//static -> non static
	//member -> method
	//private -> protected -> pulic

	//static
private:
	static GameObject* axis;
	static Material* defalutMaterial;
public:
	static GameObject* Create(string name = "GameObject");
	static void CreateStaticMember();
	static void DeleteStaticMember();
	static void RenderAxis();


	//non static
protected:
	ObType						type;

public:
	shared_ptr<Mesh>			mesh;
	shared_ptr<Shader>			shader;


	class Material*				material;
	class Collider*				collider;
	string						name;
	bool						visible;
	bool						rootMotion = false;

	class Actor*				root;		//��¥�ֻ���
	class Actor*				skelRoot;	//���̷��� ���� ���
	map<string, GameObject*>	children;
	int                         boneIndex = -1;


protected:
	GameObject();
	//���� ��常 �������
	GameObject(GameObject& src);
	virtual ~GameObject();
	void	SaveObject(Xml::XMLElement* This, Xml::XMLDocument* doc);
	void	LoadObject(Xml::XMLElement* This);
	virtual void        CopyChild(GameObject* src);
public:
	virtual void	Release();
	void			AddChild(GameObject* child);
	void			AddBone(GameObject* child);
	virtual bool	RenderHierarchy();
	virtual void	RenderDetail();

	virtual void	Update();
	virtual void	Render(shared_ptr<Shader> pShader = nullptr);
	bool            Intersect(GameObject* target);
	bool            Intersect(Ray Ray, Vector3& Hit);
};

//���
class Actor : public GameObject
{
	friend class GameObject;
public:
	static Actor* Create(string name = "Actor");
	static Actor* Create(Actor* src);
protected:
	Actor();
	//��Ʈ��常 ����
	Actor(Actor& src);
	virtual ~Actor();
public:
	string			file;
	Skeleton*		skeleton;
	Animations*		anim;
	int             boneIndexCount = 1;
	//�ؽ����̺�
	unordered_map<string, GameObject*>	obList;
public:
	void			ReleaseMember();
	GameObject*		Find(string name);
	bool            DeleteObject(string Name);
	bool            DeleteAllObject(string Name);
	virtual void	RenderDetail();
	void			SaveFile(string file);
	void			LoadFile(string file);
	virtual void	Update();
	virtual void    Render(shared_ptr<Shader> pShader = nullptr);
};