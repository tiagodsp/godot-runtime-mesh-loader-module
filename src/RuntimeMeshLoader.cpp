#include "RuntimeMeshLoader.h"
#include "core/class_db.h"
#include "loaders/Loader.h"
#include <string>

Ref<Mesh> RuntimeMeshLoader::loadMeshFromFile(String filepath)
{
	std::wstring ws_filepath = filepath.c_str();
	std::string s_filepath(ws_filepath.begin(), ws_filepath.end());
	Loader* loader = Loader::Create(Loader::CheckFileTypeSupport(s_filepath));
	if(loader)
		return loader->LoadMesh(s_filepath);
	return nullptr;
}

void RuntimeMeshLoader::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("loadMeshFromFile", "filepath"), &RuntimeMeshLoader::loadMeshFromFile);
}