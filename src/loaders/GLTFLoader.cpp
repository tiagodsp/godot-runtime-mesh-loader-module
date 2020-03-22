#include "GLTFLoader.h"

#include <fstream>
#include <iostream>
#include <vector>

#include "scene/resources/surface_tool.h"

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
					int normals_index = primitive.attributes["NORMAL"];
					const tinygltf::Accessor &acessor = model.accessors[normals_index];
					tinygltf::BufferView bv = model.bufferViews[acessor.bufferView];
					std::vector<unsigned char> buffer_data = model.buffers[bv.buffer].data;
					for (int i = 0; i < acessor.count; i++) {
						Vector3 vertexNormal;
						void *ptr = &buffer_data[bv.byteOffset + acessor.byteOffset + (i * 4 * 3)];
						vertexNormal.x = *(reinterpret_cast<float*>(ptr) + 0);
						vertexNormal.y = *(reinterpret_cast<float*>(ptr) + 1);
						vertexNormal.z = *(reinterpret_cast<float*>(ptr) + 2);
						std::cout << "VERTEX NORMAL [x: " << vertexNormal.x << ", y: " << vertexNormal.y << ", z: " << vertexNormal.z << "]\n";
						st->add_normal(vertexNormal);
					}
				}

				// UV TEX COORDINATES --------------------------
				{
					int texcoord_index = primitive.attributes["TEXCOORD_0"];
					const tinygltf::Accessor &acessor = model.accessors[texcoord_index];
					tinygltf::BufferView bv = model.bufferViews[acessor.bufferView];
					std::vector<unsigned char> buffer_data = model.buffers[bv.buffer].data;
					for (int i = 0; i < acessor.count; i++) {
						Vector2 texcoord;
						void *ptr = &buffer_data[bv.byteOffset + acessor.byteOffset + (i * 4 * 2)];
						texcoord.x = *(reinterpret_cast<float*>(ptr) + 0);
						texcoord.y = *(reinterpret_cast<float*>(ptr) + 1);
						std::cout << "TEXTURE COORD [x: " << texcoord.x << ", y: " << texcoord.y << "]\n";
						st->add_uv(texcoord);
					}
				}

				// POSITIONS -------------------
				{
					int position_index = primitive.attributes["POSITION"];
					const tinygltf::Accessor &acessor = model.accessors[position_index];
					tinygltf::BufferView bv = model.bufferViews[acessor.bufferView];
					std::vector<unsigned char> buffer_data = model.buffers[bv.buffer].data;
					for (int i = 0; i < acessor.count; i++) {
						Vector3 vertexPosition;
						void *ptr = &buffer_data[bv.byteOffset + acessor.byteOffset + (i * 4 * 3)];
						
						vertexPosition.x = *(reinterpret_cast<float*>(ptr) + 0);
						vertexPosition.y = *(reinterpret_cast<float*>(ptr) + 1);
						vertexPosition.z = *(reinterpret_cast<float*>(ptr) + 2);
						std::cout << "VERTEX POSITION [x: " << vertexPosition.x << ", y: " << vertexPosition.y << ", z: " << vertexPosition.z << "]\n";
						st->add_vertex(vertexPosition);
					}
				}

				// INDICES --------------------
				{
					const tinygltf::Accessor &acessor = model.accessors[primitive.indices];
					tinygltf::BufferView bv = model.bufferViews[acessor.bufferView];
					for (int i = 0; i < acessor.count; i++) {
						void *ptr;
						switch (acessor.componentType) {
							case TINYGLTF_COMPONENT_TYPE_BYTE:
							case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
								ptr = &model.buffers[bv.buffer].data[bv.byteOffset + acessor.byteOffset + (i * 1)];
								st->add_index(*(unsigned char *)ptr);
								break;
							case TINYGLTF_COMPONENT_TYPE_SHORT:
							case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
								ptr = &model.buffers[bv.buffer].data[bv.byteOffset + acessor.byteOffset + (i * 2)];
								st->add_index(*(unsigned short *)ptr);
								std::cout << "VERTEX INDEX [" << *(unsigned short *)ptr << "]\n";
								break;
							case TINYGLTF_COMPONENT_TYPE_INT:
							case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
							case TINYGLTF_COMPONENT_TYPE_FLOAT:
								ptr = &model.buffers[bv.buffer].data[bv.byteOffset + acessor.byteOffset + (i * 4)];
								st->add_index(*(unsigned int *)ptr);
								break;
							default:
								break;
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
