#pragma once
#include "core/reference.h"
#include "scene/resources/mesh.h"
#include <vector>

class RuntimeMeshLoader : public Reference {
	GDCLASS(RuntimeMeshLoader, Reference);
protected:
    static void _bind_methods();
public:
	Ref<Mesh> loadMeshFromFile(String filepath);
	RuntimeMeshLoader(){}
};
