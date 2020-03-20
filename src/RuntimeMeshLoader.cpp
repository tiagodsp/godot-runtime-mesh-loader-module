#include "runtimemeshloader.h"

#include <iostream>
#include <ifstream>
#include <vector>
#include <regex>

#include "tinygltf/tiny_gltf.h"

RuntimeMeshLoader::RuntimeMeshLoader(/* args */)
{
}

RuntimeMeshLoader::~RuntimeMeshLoader()
{
}

Ref<Mesh> RuntimeMeshLoader::loadMeshFromFile(String filepath)
{
	std::vector<unsigned char> filedata;

	// read data from file
	ifstream file(filepath.c_str(), ios::binary);
	if(file.is_open())
	{
		uint64_t size = file.tellg();
		filedata = std::vector<unsigned char>(size);
		file.seekg(0);
		file.read(filedata.data(), size);
		file.close();
	}

	switch (CheckFileTypeSupport(filepath))
	{
	case RuntimeMeshLoader::FileType::GLTFL
		return
	default:
		return nullptr;
	}

	// load from buffer
	if(filedata.size() > 0)
		return _loadMeshFromByteArray(filedata);
}
