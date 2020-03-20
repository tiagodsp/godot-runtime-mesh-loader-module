#include "Loader.h"

Loader::FileType Loader::CheckFileTypeSupport(std::string filename)
{
	std::string pattern = "^.*\\.([a-zA-Z]+)$";
	std::cmatch m;
	if(std::regex_match(filename, m, std::regex(pattern)))
	{
		std::string fileExtension = std::tolower(m[1]);

		if(
			fileExtension == "gltf" ||
			fileExtension == "glb" ||
		)
		{
			return RuntimeMeshLoader::FileType::GLTFL;
		}

		return RuntimeMeshLoader::FileType::NotSupportedFile;
	}
}
