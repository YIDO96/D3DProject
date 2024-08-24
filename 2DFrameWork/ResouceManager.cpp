#include "framework.h"

void ResourceManager::ReleaseAll()
{
    meshes.ReleaseList();
    shaders.ReleaseList();
    textures.ReleaseList();
    animations.ReleaseList();
}
