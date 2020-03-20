#include "Loader.h"

/**
 * Loader implementation for GLTF file format.
 */
class GLTFLoader : public Loader {
public:
	virtual Ref<Mesh> LoadMesh(std::vector<unsigned char> membuffer) override;
	virtual Ref<Mesh> LoadMesh(std::string filepath) override;
}
