#include "GLTFLoader.h"

#include "scene/resources/surface_tool.h"
#include <math.h>
#include <fstream>
#include <iostream>
#include <vector>

#include "tinygltf/tiny_gltf.h"

Ref<Mesh> GLTFLoader::LoadMesh(std::string filepath) {
	std::vector<unsigned char> filedata;

	// read data from file
	std::cout << "FILEEEEEE: " << filepath;
	std::ifstream file(filepath.c_str(), std::ios::in | std::ios::binary);
	file.seekg(0, file.end);
	size_t size = file.tellg();
	file.seekg(0, file.beg);

	std::cout << "TAMANHOOOOOOO: " << size;
	filedata = std::vector<unsigned char>(size);

	file.read((char *)filedata.data(), size);
	file.close();
	std::cout << "DATA READED: " << filedata.size();

	return LoadMesh(filedata);
}

Ref<Mesh> GLTFLoader::LoadMesh(std::vector<unsigned char> membuffer) {
	tinygltf::TinyGLTF loader;
	tinygltf::Model model;
	std::string err;
	std::string warn;

	Ref<ArrayMesh> mesh;
	mesh.instance();

	Ref<SurfaceTool> st;
	st.instance();
	st->begin(Mesh::PRIMITIVE_TRIANGLES);
	// st->add_vertex(Vector3(-10, 0, 0));
	// st->add_vertex(Vector3(0, 10, 0));
	// st->add_vertex(Vector3(10, 0, 0));
	// st->generate_normals();
	// st->generate_tangents();
	// mesh->add_surface_from_arrays(Mesh::PrimitiveType::PRIMITIVE_TRIANGLES, st->commit_to_arrays());
	std::cout << "TRYING TO LOADDDDDDDDDDDDDDDDDDDDDDDDDDDDD!!!!!";
	if (loader.LoadBinaryFromMemory(&model, &err, &warn, membuffer.data(), membuffer.size())) {

		for (auto &mesh : model.meshes) {
			for (auto &primitive : mesh.primitives) {

				// NORMALS --------------------------
				{
					std::string key = "NORMAL";
					std::map<std::string, int> attr = primitive.attributes;
					int acessor_index = attr.find(key) != attr.end() ? attr[key] : -1;
					if (acessor_index >= 0) {
						const tinygltf::Accessor &acessor = model.accessors[acessor_index];
						tinygltf::BufferView bv = model.bufferViews[acessor.bufferView];
						std::vector<unsigned char> buffer = model.buffers[bv.buffer].data;
						size_t offset = bv.byteOffset + acessor.byteOffset;
						for (int i = 0; i < acessor.count; i++) {
							Vector3 vertexNormal;
							void *ptr = &buffer[offset + (i * tinygltf::GetNumComponentsInType(acessor.type) * tinygltf::GetComponentSizeInBytes(acessor.componentType))];
							unsigned int mask = (0xFFFFFFFF >> (32 - (int)std::pow(2, 8 * tinygltf::GetComponentSizeInBytes(acessor.componentType))));
							for (int i = 0; i < tinygltf::GetNumComponentsInType(acessor.type); i++) {
								unsigned int bytevalue = *(reinterpret_cast<unsigned int *>(ptr) + i) & mask;
								vertexNormal[i] = *reinterpret_cast<float *>(&bytevalue);
							}
							std::cout << "VERTEX NORMAL [x: " << vertexNormal.x << ", y: " << vertexNormal.y << ", z: " << vertexNormal.z << "]\n";
							st->add_normal(vertexNormal);
						}
					}
				}

				// UV TEX COORDINATES --------------------------
				{
					std::string key = "TEXCOORD_0";
					std::map<std::string, int> attr = primitive.attributes;
					int acessor_index = attr.find(key) != attr.end() ? attr[key] : -1;
					if (acessor_index >= 0) {
						const tinygltf::Accessor &acessor = model.accessors[acessor_index];
						tinygltf::BufferView bv = model.bufferViews[acessor.bufferView];
						std::vector<unsigned char> buffer = model.buffers[bv.buffer].data;
						size_t offset = bv.byteOffset + acessor.byteOffset;
						for (int i = 0; i < acessor.count; i++) {
							Vector2 texcoord;
							void *ptr = &buffer[offset + (i * tinygltf::GetNumComponentsInType(acessor.type) * tinygltf::GetComponentSizeInBytes(acessor.componentType))];
							unsigned int mask = (0xFFFFFFFF >> (32 - (int)std::pow(2, 8 * tinygltf::GetComponentSizeInBytes(acessor.componentType))));
							for (int i = 0; i < tinygltf::GetNumComponentsInType(acessor.type); i++) {
								unsigned int bytevalue = *(reinterpret_cast<unsigned int *>(ptr) + i) & mask;
								texcoord[i] = *reinterpret_cast<float *>(&bytevalue);
							}
							std::cout << "TEXTURE COORD [x: " << texcoord.x << ", y: " << texcoord.y << "]\n";
							st->add_uv(texcoord);
						}
					}
				}

				// VERTEX COLORS --------------------------
				{
					std::string key = "COLOR_0";
					std::map<std::string, int> attr = primitive.attributes;
					int acessor_index = attr.find(key) != attr.end() ? attr[key] : -1;
					if (acessor_index >= 0) {
						const tinygltf::Accessor &acessor = model.accessors[acessor_index];
						tinygltf::BufferView bv = model.bufferViews[acessor.bufferView];
						std::vector<unsigned char> buffer = model.buffers[bv.buffer].data;
						size_t offset = bv.byteOffset + acessor.byteOffset;
						for (int i = 0; i < acessor.count; i++) {

							Color color = { 0, 0, 0, 1 };
							void *ptr = &buffer[offset + (i * tinygltf::GetNumComponentsInType(acessor.type) * tinygltf::GetComponentSizeInBytes(acessor.componentType))];
							unsigned int mask = (0xFFFFFFFF >> (32 - (int)std::pow(2, 8 * tinygltf::GetComponentSizeInBytes(acessor.componentType))));
							for (int i = 0; i < tinygltf::GetNumComponentsInType(acessor.type); i++) {
								unsigned int bytevalue = *(reinterpret_cast<unsigned int *>(ptr) + i) & mask;
								color.components[i] = *reinterpret_cast<float *>(&bytevalue);
							}
							std::cout << "VERTEX COLOR [r: " << color.r << ", g: " << color.g << ", b: " << color.b << ", a: " << color.a << "]\n";
							st->add_color(color);
						}
					}
				}

				// POSITIONS -------------------
				{
					std::string key = "POSITION";
					std::map<std::string, int> attr = primitive.attributes;
					int acessor_index = attr.find(key) != attr.end() ? attr[key] : -1;
					if (acessor_index >= 0) {
						const tinygltf::Accessor &acessor = model.accessors[acessor_index];
						tinygltf::BufferView bv = model.bufferViews[acessor.bufferView];
						std::vector<unsigned char> buffer = model.buffers[bv.buffer].data;
						size_t offset = bv.byteOffset + acessor.byteOffset;
						for (int i = 0; i < acessor.count; i++) {
							Vector3 vertexPosition;
							void *ptr = &buffer[offset + (i * tinygltf::GetNumComponentsInType(acessor.type) * tinygltf::GetComponentSizeInBytes(acessor.componentType))];
							unsigned int mask = (0xFFFFFFFF >> (32 - (int)std::pow(2, 8 * tinygltf::GetComponentSizeInBytes(acessor.componentType))));
							for (int i = 0; i < tinygltf::GetNumComponentsInType(acessor.type); i++) {
								vertexPosition.coord[i] = *(reinterpret_cast<float *>(ptr) + i);
							}
							std::cout << "VERTEX POSITION [x: " << vertexPosition.x << ", y: " << vertexPosition.y << ", z: " << vertexPosition.z << "]\n";
							st->add_vertex(vertexPosition);
						}
					}
				}

				// INDICES --------------------
				{
					int acessor_index = primitive.indices;
					if (acessor_index >= 0) {
						const tinygltf::Accessor &acessor = model.accessors[acessor_index];
						tinygltf::BufferView bv = model.bufferViews[acessor.bufferView];
						std::vector<unsigned char> buffer = model.buffers[bv.buffer].data;
						size_t offset = bv.byteOffset + acessor.byteOffset;
						for (int i = 0; i < acessor.count; i++) {
							void *ptr = &buffer[offset + (i * tinygltf::GetNumComponentsInType(acessor.type) * tinygltf::GetComponentSizeInBytes(acessor.componentType))];
							unsigned int mask = (0xFFFFFFFF >> (32 - (int)std::pow(2, 8 * tinygltf::GetComponentSizeInBytes(acessor.componentType))));
							std::cout << "MASK [" << mask << "]\n";
							unsigned int value = *(reinterpret_cast<unsigned int *>(ptr)) & mask;
							st->add_index(value);
							std::cout << "VERTEX INDEX [" << value << "]\n";
						}
					}
				}
			}
		}
		//st->generate_normals();
		//st->generate_tangents();
		mesh->add_surface_from_arrays(Mesh::PrimitiveType::PRIMITIVE_TRIANGLES, st->commit_to_arrays());
	} else {
		std::cout << err;
		std::cout << warn;
	}
	return mesh;
}
