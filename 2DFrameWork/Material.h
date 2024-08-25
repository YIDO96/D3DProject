#pragma once

class MaterialBuffer
{
public:
	Color	ambient;
	Color	diffuse;
	Color	specular;
	Color	emissive;

	float	shininess;		//스펙큘러 밀집도
	float	opacity;		//투명도
	float   roughness;
	float	metallic;

	float	shadow;
	Vector3 padding;
};

class Material : public MaterialBuffer
{
public:
	static ID3D11Buffer* materialBuffer;
	static void CreateStaticMember();
	static void DeleteStaticMember();
public:
	shared_ptr<Texture>			normalMap;
	shared_ptr<Texture>			diffuseMap;
	shared_ptr<Texture>			specularMap;
	shared_ptr<Texture>			emissiveMap;
	string						file;
public:
	Material();
	~Material();

	void RenderDetail();
	virtual void Set();
	void LoadFile(string file);
	void SaveFile(string file);
};

