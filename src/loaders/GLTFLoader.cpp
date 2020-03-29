#include "GLTFLoader.h"

#include "scene/resources/surface_tool.h"
#include <math.h>
#include <fstream>
#include <iostream>
#include <vector>

#include "core/array.h"

#include "TextureUtils.h"
#include "tinygltf/tiny_gltf.h"

Ref<Mesh> GLTFLoader::LoadMesh(std::string filepath) {
	std::vector<unsigned char> filedata;

	// read data from file

	std::ifstream file(filepath.c_str(), std::ios::in | std::ios::binary);
	file.seekg(0, file.end);
	size_t size = file.tellg();
	file.seekg(0, file.beg);

	filedata = std::vector<unsigned char>(size);

	file.read((char *)filedata.data(), size);
	file.close();

	return LoadMesh(filedata);
}

Ref<Mesh> GLTFLoader::LoadMesh(std::vector<unsigned char> membuffer) {
	tinygltf::TinyGLTF loader;
	tinygltf::Model model;
	std::string err;
	std::string warn;

	Ref<ArrayMesh> mesharray;
	mesharray.instance();

	if (loader.LoadBinaryFromMemory(&model, &err, &warn, membuffer.data(), membuffer.size())) {
	} else if (loader.LoadASCIIFromString(&model, &err, &warn, (char *)membuffer.data(), membuffer.size(), ".")) {
	} else {
		std::cout << err;
		std::cout << warn;
		return mesharray;
	}

	// Loading textures ---------------------------
	std::vector<Ref<Texture> > textures = {};
	for (tinygltf::Texture tg_texture : model.textures) {

		Ref<ImageTexture> texture;
		texture.instance();

		// Start to load the texture images.
		// TODO - Implement texture load from URI.
		if (tg_texture.source != -1) {
			tinygltf::Image tg_img = model.images[tg_texture.source];
			tinygltf::BufferView bv = model.bufferViews[tg_img.bufferView];

			std::vector<unsigned char> &buffer = model.buffers[bv.buffer].data;

			PoolVector<uint8_t> b;
			for (unsigned int i = 0; i < bv.byteLength; i++) {
				b.push_back(buffer[bv.byteOffset + i]);
			}

			Ref<Image> img;
			img.instance();

			// Load and compress image to DXT1 image format
			if (tg_img.mimeType.compare("image/jpeg") == 0) {
				img->load_jpg_from_buffer(b);
			} else if (tg_img.mimeType.compare("image/png") == 0) {
				img->load_png_from_buffer(b);
			} else {
				ERR_PRINT("Failed to load texture: mimeType not supported.");
				continue;
			}

			img->decompress();
			img->convert(Image::Format::FORMAT_RGBA8);

			Ref<Image> compressedImg;
			compressedImg.instance();
			TextureUtils::ConvertRGBA8ToDXT(compressedImg, img);
			img = compressedImg;

			texture->create_from_image(img);
			textures.push_back(texture);
		} else // Until we have implemented the URI load mode, we should use this else condition as fallback.
		{
			ERR_PRINT("Image source not found. Creating dummy texture.");
			texture->create(128, 128, Image::Format::FORMAT_RGBA8);
			textures.push_back(texture);
		}
	}

	// Load meshes --------------------------------
	for (auto &mesh : model.meshes) {
		// Load each mesh surface section -------------------
		for (int pi = 0; pi < mesh.primitives.size(); pi++) {

			Ref<SurfaceTool> st;
			st.instance();
			st->begin(Mesh::PRIMITIVE_TRIANGLES);

			tinygltf::Primitive &primitive = mesh.primitives[pi];
			int acessor_index = primitive.indices;
			int total_vertices = model.accessors[acessor_index].count;

			// Load Vertex Array information for this surface section. ------------------------------
			for (int vi = 0; vi < total_vertices; vi++) {
				// NORMALS --------------------------
				{
					std::string key = "NORMAL";
					std::map<std::string, int> attr = primitive.attributes;
					int acessor_index = attr.find(key) != attr.end() ? attr[key] : -1;
					if (acessor_index >= 0) {
						const tinygltf::Accessor &acessor = model.accessors[acessor_index];
						tinygltf::BufferView bv = model.bufferViews[acessor.bufferView];
						std::vector<unsigned char> &buffer = model.buffers[bv.buffer].data;
						size_t offset = bv.byteOffset + acessor.byteOffset;

						Vector3 vertexNormal;
						void *ptr = &buffer[offset + (vi * tinygltf::GetNumComponentsInType(acessor.type) * tinygltf::GetComponentSizeInBytes(acessor.componentType))];
						unsigned int mask = (0xFFFFFFFF >> (32 - 8 * tinygltf::GetComponentSizeInBytes(acessor.componentType)));
						//std::cout << "MASK [" << mask << "]\n";
						for (int i = 0; i < tinygltf::GetNumComponentsInType(acessor.type); i++) {
							unsigned int bytevalue = *(reinterpret_cast<unsigned int *>(ptr) + i) & mask;
							vertexNormal[i] = *reinterpret_cast<float *>(&bytevalue);
						}

						//std::cout << vi << " - VERTEX NORMAL [x: " << vertexNormal.x << ", y: " << vertexNormal.y << ", z: " << vertexNormal.z << "]\n";
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
						std::vector<unsigned char> &buffer = model.buffers[bv.buffer].data;
						size_t offset = bv.byteOffset + acessor.byteOffset;

						Vector2 texcoord;
						void *ptr = &buffer[offset + (vi * tinygltf::GetNumComponentsInType(acessor.type) * tinygltf::GetComponentSizeInBytes(acessor.componentType))];
						unsigned int mask = (0xFFFFFFFF >> (32 - 8 * tinygltf::GetComponentSizeInBytes(acessor.componentType)));
						//std::cout << "MASK [" << mask << "]\n";
						for (int i = 0; i < tinygltf::GetNumComponentsInType(acessor.type); i++) {
							unsigned int bytevalue = *(reinterpret_cast<unsigned int *>(ptr) + i) & mask;
							texcoord[i] = *reinterpret_cast<float *>(&bytevalue);
						}
						//std::cout << vi << " - TEXTURE COORD [x: " << texcoord.x << ", y: " << texcoord.y << "]\n";
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
						std::vector<unsigned char> &buffer = model.buffers[bv.buffer].data;
						size_t offset = bv.byteOffset + acessor.byteOffset;

						Color color;
						void *ptr = &buffer[offset + (vi * tinygltf::GetNumComponentsInType(acessor.type) * tinygltf::GetComponentSizeInBytes(acessor.componentType))];
						unsigned int mask = (0xFFFFFFFF >> (32 - 8 * tinygltf::GetComponentSizeInBytes(acessor.componentType)));
						//std::cout << "MASK [" << mask << "]\n";
						for (int i = 0; i < tinygltf::GetNumComponentsInType(acessor.type); i++) {
							unsigned int bytevalue = *(reinterpret_cast<unsigned int *>(ptr) + i) & mask;
							color.components[i] = *reinterpret_cast<float *>(&bytevalue);
						}

						//std::cout << vi << " - VERTEX COLOR [r: " << color.r << ", g: " << color.g << ", b: " << color.b << ", a: " << color.a << "]\n";
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
						std::vector<unsigned char> &buffer = model.buffers[bv.buffer].data;
						size_t offset = bv.byteOffset + acessor.byteOffset;

						Vector3 vertexPosition;
						void *ptr = &buffer[offset + (vi * tinygltf::GetNumComponentsInType(acessor.type) * tinygltf::GetComponentSizeInBytes(acessor.componentType))];
						unsigned int mask = (0xFFFFFFFF >> (32 - 8 * tinygltf::GetComponentSizeInBytes(acessor.componentType)));
						//std::cout << "MASK [" << mask << "]\n";
						for (int i = 0; i < tinygltf::GetNumComponentsInType(acessor.type); i++) {
							vertexPosition.coord[i] = *(reinterpret_cast<float *>(ptr) + i);
						}
						//std::cout << vi << " - VERTEX POSITION [x: " << vertexPosition.x << ", y: " << vertexPosition.y << ", z: " << vertexPosition.z << "]\n";
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
					std::vector<unsigned char> &buffer = model.buffers[bv.buffer].data;
					size_t offset = bv.byteOffset + acessor.byteOffset;

					for (int vi = 0; vi < acessor.count; vi++) {
						void *ptr = &buffer[offset + (vi * tinygltf::GetNumComponentsInType(acessor.type) * tinygltf::GetComponentSizeInBytes(acessor.componentType))];
						unsigned int mask = (0xFFFFFFFF >> (32 - 8 * tinygltf::GetComponentSizeInBytes(acessor.componentType)));
						//std::cout << "MASK [" << mask << "]\n";
						int value = *(reinterpret_cast<int *>(ptr)) & mask;
						st->add_index(value);
						//std::cout << vi << " - VERTEX INDEX [" << value << "]\n";
					}
				}
			}
			// --------------------------------------------------
			// Loading material for of this mesh section
			Ref<SpatialMaterial> mat;
			mat.instance();
			if (primitive.material != -1) {
				tinygltf::Material &tg_mat = model.materials[primitive.material];

				// Albedo
				{
					int tex_index = tg_mat.pbrMetallicRoughness.baseColorTexture.index;
					if (tex_index >= 0) {
						Ref<Texture> tex = textures[tex_index];
						mat->set_texture(SpatialMaterial::TextureParam::TEXTURE_ALBEDO, tex);
					}
					std::vector<double> color = tg_mat.pbrMetallicRoughness.baseColorFactor;
					mat->set_albedo(Color(color[0], color[1], color[2], 1.0f));
				}
				// Metallic
				{
					int tex_index = tg_mat.pbrMetallicRoughness.metallicRoughnessTexture.index;
					if (tex_index >= 0) {
						Ref<Texture> tex = textures[tex_index];
						mat->set_texture(SpatialMaterial::TextureParam::TEXTURE_METALLIC, tex);
					}
					mat->set_metallic(tg_mat.pbrMetallicRoughness.metallicFactor);
				}
				// Roughness
				{
					int tex_index = tg_mat.pbrMetallicRoughness.metallicRoughnessTexture.index;
					if (tex_index >= 0) {
						Ref<Texture> tex = textures[tex_index];
						mat->set_texture(SpatialMaterial::TextureParam::TEXTURE_ROUGHNESS, tex);
					}
					mat->set_roughness(tg_mat.pbrMetallicRoughness.roughnessFactor);
				}
				// Normal
				{
					int tex_index = tg_mat.normalTexture.index;
					if (tex_index >= 0) {
						Ref<Texture> tex = textures[tex_index];
						mat->set_texture(SpatialMaterial::TextureParam::TEXTURE_NORMAL, tex);
					}
				}
				// Occlusion
				{
					int tex_index = tg_mat.occlusionTexture.index;
					if (tex_index >= 0) {
						Ref<Texture> tex = textures[tex_index];
						mat->set_texture(SpatialMaterial::TextureParam::TEXTURE_AMBIENT_OCCLUSION, tex);
					}
				}
				// Emissive
				{
					int tex_index = tg_mat.occlusionTexture.index;
					if (tex_index >= 0) {
						Ref<Texture> tex = textures[tex_index];
						mat->set_texture(SpatialMaterial::TextureParam::TEXTURE_EMISSION, tex);
					}
					std::vector<double> color = tg_mat.emissiveFactor;
					mat->set_emission(Color(color[0], color[1], color[2], 1.0f));
				}
			}

			mesharray->add_surface_from_arrays(Mesh::PrimitiveType::PRIMITIVE_TRIANGLES, st->commit_to_arrays());
			mat->set_cull_mode(SpatialMaterial::CULL_DISABLED);
			mesharray->surface_set_material(pi, mat);
		}
	}

	return mesharray;
}
