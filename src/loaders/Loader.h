#pragma once
#include "scene/resources/mesh.h"
#include <vector>
#include <string>

/**
 * Loader base class.
 */
class Loader {
	enum class FileType : uint8_t {
		None,
		GLTF,
		NotSupportedFile
	};

public:
	static Loader* Create(FileType type);
	static Loader::FileType CheckFileTypeSupport(std::string filename);

	virtual Ref<Mesh> LoadMesh(std::vector<unsigned char> membuffer) = 0;
	virtual Ref<Mesh> LoadMesh(std::string filepath) = 0;
};


