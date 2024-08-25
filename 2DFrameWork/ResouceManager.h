#pragma once

template<typename T>
class Resource
{
private:
	unordered_map<string, shared_ptr<T>> list;
public:
	const shared_ptr<T>& Load(string file)
	{

		auto it = list.find(file);
		if (it != list.end())
		{
			return it->second;
		}
		shared_ptr<T> temp = make_shared<T>();
		temp->LoadFile(file);
		list[file] = temp;
		return list[file];
	}

	void ReleaseList()
	{
		for (auto it = list.begin(); it != list.end(); it++)
		{
			SafeReset(it->second);
		}
		list.clear();
	}
};

class ResourceManager : public Singleton<ResourceManager>
{
public:
	Resource<Mesh>			meshes;
	Resource<Shader>		shaders;
	Resource<Texture>		textures;
	Resource<Animation>		animations;
	void ReleaseAll();
};
