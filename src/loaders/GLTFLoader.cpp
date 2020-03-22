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

				// VERTEX POSITIONS -------------------
				{
					int position_index = primitive.attributes["POSITION"];
					const tinygltf::Accessor &position_acessor = model.accessors[position_index];
					tinygltf::BufferView bv = model.bufferViews[position_acessor.bufferView];
					std::vector<unsigned char> buffer_data = model.buffers[bv.buffer].data;
					for (int i = 0; i < position_acessor.count; i++) {
						Vector3 vertexPosition;
						void *ptr = &buffer_data[bv.byteOffset + position_acessor.byteOffset + (i * 4 * 3)];
						vertexPosition.x = *((float *)ptr + 0);
						vertexPosition.y = *((float *)ptr + 4);
						vertexPosition.z = *((float *)ptr + 8);
						std::cout << "VERTEX POSITION [x: " << vertexPosition.x << ", y: " << vertexPosition.y << ", z: " << vertexPosition.z << "]\n";
						st->add_vertex(vertexPosition);
					}
				}

				// VERTEX INDICES --------------------
				{
					const tinygltf::Accessor &indices_acessor = model.accessors[primitive.indices];
					tinygltf::BufferView bv = model.bufferViews[indices_acessor.bufferView];
					for (int i = 0; i < indices_acessor.count; i++) {
						int index = (int)model.buffers[bv.buffer].data[bv.byteOffset + indices_acessor.byteOffset + (i * tinygltf::GetComponentSizeInBytes(indices_acessor.componentType))];
						std::cout << "VERTEX INDEX [" << index << "]\n";
						st->add_index(index);
					}
				}
			}
		}
		st->generate_normals();
		//st->generate_tangents();
		mesh->add_surface_from_arrays(Mesh::PrimitiveType::PRIMITIVE_TRIANGLES, st->commit_to_arrays());
	} else {
		std::cout << err;
		std::cout << warn;
	}
	return mesh;
}
