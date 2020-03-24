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

	Ref<ArrayMesh> mesharray;
	mesharray.instance();

	Ref<SurfaceTool> st;
	st.instance();
	st->begin(Mesh::PRIMITIVE_TRIANGLES);

	std::cout << "TRYING TO LOADDDDDDDDDDDDDDDDDDDDDDDDDDDDD!!!!!";
	if (loader.LoadBinaryFromMemory(&model, &err, &warn, membuffer.data(), membuffer.size())) {

		for (auto &mesh : model.meshes) {
			for (auto &primitive : mesh.primitives) {
				int acessor_index = primitive.indices;
				int total_vertices = model.accessors[acessor_index].count;

				for (int vi = 0; vi < total_vertices; vi++) {
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

							Vector3 vertexNormal;
							void *ptr = &buffer[offset + (vi * tinygltf::GetNumComponentsInType(acessor.type) * tinygltf::GetComponentSizeInBytes(acessor.componentType))];
							unsigned int mask = (0xFFFFFFFF >> (32 - (int)std::pow(2, 8 * tinygltf::GetComponentSizeInBytes(acessor.componentType))));
							std::cout << "MASK [" << mask << "]\n";
							for (int i = 0; i < tinygltf::GetNumComponentsInType(acessor.type); i++) {
								unsigned int bytevalue = *(reinterpret_cast<unsigned int *>(ptr) + i) & mask;
								vertexNormal[i] = *reinterpret_cast<float *>(&bytevalue);
							}

							std::cout << vi << " - VERTEX NORMAL [x: " << vertexNormal.x << ", y: " << vertexNormal.y << ", z: " << vertexNormal.z << "]\n";
							st->add_normal(vertexNormal * -1);
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

							Vector2 texcoord;
							void *ptr = &buffer[offset + (vi * tinygltf::GetNumComponentsInType(acessor.type) * tinygltf::GetComponentSizeInBytes(acessor.componentType))];
							unsigned int mask = (0xFFFFFFFF >> (32 - (int)std::pow(2, 8 * tinygltf::GetComponentSizeInBytes(acessor.componentType))));
							std::cout << "MASK [" << mask << "]\n";
							for (int i = 0; i < tinygltf::GetNumComponentsInType(acessor.type); i++) {
								unsigned int bytevalue = *(reinterpret_cast<unsigned int *>(ptr) + i) & mask;
								texcoord[i] = *reinterpret_cast<float *>(&bytevalue);
							}
							std::cout << vi << " - TEXTURE COORD [x: " << texcoord.x << ", y: " << texcoord.y << "]\n";
							st->add_uv(Vector2(texcoord.x, texcoord.y));
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

							Color color;
							void *ptr = &buffer[offset + (vi * tinygltf::GetNumComponentsInType(acessor.type) * tinygltf::GetComponentSizeInBytes(acessor.componentType))];
							unsigned int mask = (0xFFFFFFFF >> (32 - (int)std::pow(2, 8 * tinygltf::GetComponentSizeInBytes(acessor.componentType))));
							std::cout << "MASK [" << mask << "]\n";
							for (int i = 0; i < tinygltf::GetNumComponentsInType(acessor.type); i++) {
								unsigned int bytevalue = *(reinterpret_cast<unsigned int *>(ptr) + i) & mask;
								color.components[i] = *reinterpret_cast<float *>(&bytevalue);
							}

							std::cout << vi << " - VERTEX COLOR [r: " << color.r << ", g: " << color.g << ", b: " << color.b << ", a: " << color.a << "]\n";
							st->add_color(color);
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

							Vector3 vertexPosition;
							void *ptr = &buffer[offset + (vi * tinygltf::GetNumComponentsInType(acessor.type) * tinygltf::GetComponentSizeInBytes(acessor.componentType))];
							unsigned int mask = (0xFFFFFFFF >> (32 - (int)std::pow(2, 8 * tinygltf::GetComponentSizeInBytes(acessor.componentType))));
							std::cout << "MASK [" << mask << "]\n";
							for (int i = 0; i < tinygltf::GetNumComponentsInType(acessor.type); i++) {
								vertexPosition.coord[i] = *(reinterpret_cast<float *>(ptr) + i);
							}
							std::cout << vi << " - VERTEX POSITION [x: " << vertexPosition.x << ", y: " << vertexPosition.y << ", z: " << vertexPosition.z << "]\n";
							st->add_vertex(vertexPosition);
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

							void *ptr = &buffer[offset + (vi * tinygltf::GetNumComponentsInType(acessor.type) * tinygltf::GetComponentSizeInBytes(acessor.componentType))];
							unsigned int mask = (0xFFFFFFFF >> (32 - (int)std::pow(2, 8 * tinygltf::GetComponentSizeInBytes(acessor.componentType))));
							std::cout << "MASK [" << mask << "]\n";
							unsigned int value = *(reinterpret_cast<unsigned int *>(ptr)) & mask;
							st->add_index(value);
							std::cout << vi << " - VERTEX INDEX [" << value << "]\n";
						}
					}
				}
			}
		}
		mesharray->add_surface_from_arrays(Mesh::PrimitiveType::PRIMITIVE_TRIANGLES, st->commit_to_arrays());
		//st->generate_normals();
		//st->generate_tangents();
	} else {
		std::cout << err;
		std::cout << warn;
	}
	return mesharray;
}
