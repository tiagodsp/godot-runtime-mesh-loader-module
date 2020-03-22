#include "Loader.h"
#include "GLTFLoader.h"

#include <regex>
#include <string>

Loader *Loader::Create(FileType type) {
	switch (type) {
		case FileType::GLTF:
			return new GLTFLoader();
		case FileType::NotSupportedFile:
			return nullptr;
		default:
			return nullptr;
	}
}

Loader::FileType Loader::CheckFileTypeSupport(std::string filename) {
	std::string pattern = "^.*\\.([a-zA-Z]+)$";
	std::smatch m;

	std::for_each(filename.begin(), filename.end(), [](char &c) {
		c = std::tolower(c);
	});

	if (std::regex_match(filename, m, std::regex(pattern))) {

		std::string fileExtension = m[1];

		if (
				fileExtension == "gltf" ||
				fileExtension == "glb") {
			return Loader::FileType::GLTF;
		}

		return Loader::FileType::NotSupportedFile;
	}
}
