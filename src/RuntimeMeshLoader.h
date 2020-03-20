#pragma once

#include "scene/resources/mesh.h"
#include <vector>

class RuntimeMeshLoader {
public:
	RuntimeMeshLoader();
	~RuntimeMeshLoader();

	Ref<Mesh> loadMeshFromFile(String filepath);
};
