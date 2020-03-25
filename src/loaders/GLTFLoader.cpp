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
							//std::cout << "MASK [" << mask << "]\n";
							unsigned int value = *(reinterpret_cast<unsigned int *>(ptr)) & mask;
							st->add_index(value);
							std::cout << vi << " - VERTEX INDEX [" << value << "]\n";
						}
					}
				}

				// MATERIAL
				{
					const int materialIndex = primitive.material;
					const tinygltf::Material& meshMaterial = model.materials[materialIndex];
					
					// Get material properties
					enum E_TextureTypes
					{
						TT_ALBEDO,
						TT_METALLIC_ROUGHNESS,
						TT_NORMAL,
						TT_EMISSIVE,
						TT_OCCLUSION
					};
					// Get material properties
					enum E_MaterialTextureValues
					{
						MTV_ALBEDO_BASECOLOR,
						MTV_METALLIC_FACTOR,
						MTV_ROUGHNESS_FACTOR,
						MTV_NORMAL_FACTOR,
						MTV_EMISSIVE_FACTOR,
						MTV_OCCLUSION_FACTOR
					};

					// Map attributes for material properties and texture loading
					std::map<E_TextureTypes, tinygltf::Parameter> textureMap {}; // Map a texture type to its reference
					std::map<E_MaterialTextureValues, tinygltf::Parameter> materialPropertiesMap {}; // Map a texture type to its reference

					if (&meshMaterial.values != nullptr)
					{
						std::for_each(meshMaterial.values.cbegin(), meshMaterial.values.cend(), [&textureMap, &materialPropertiesMap](const auto& value){
							std::cout << "Value found:" << std::endl;
							std::cout << "- " << value.first << std::endl;

							// Material properties texture-relative items
							if (value.first == "baseColorTexture")
							{
								std::cout << "Found material texture type: " << value.first << std::endl;
								textureMap[E_TextureTypes::TT_ALBEDO] = value.second;
							}
							else if (value.first == "metallicRoughnessTexture")
							{
								std::cout << "Found material texture type: " << value.first << std::endl;
								textureMap[E_TextureTypes::TT_METALLIC_ROUGHNESS] = value.second;
							}
							else if (value.first == "normalTexture")
							{
								std::cout << "Found material texture type: " << value.first << std::endl;
								textureMap[E_TextureTypes::TT_NORMAL] = value.second;
							}
							else if (value.first == "emissiveTexture")
							{
								std::cout << "Found material texture type: " << value.first << std::endl;
								textureMap[E_TextureTypes::TT_EMISSIVE] = value.second;
							}
							else if (value.first == "occlusionTexture")
							{
								std::cout << "Found material texture type: " << value.first << std::endl;
								textureMap[E_TextureTypes::TT_OCCLUSION] = value.second;
							}
							// Material properties factor-relative items
							else if (value.first == "baseColorFactor")
							{
								std::cout << "Found material texture type: " << value.first << std::endl;
								materialPropertiesMap[E_MaterialTextureValues::MTV_ALBEDO_BASECOLOR] = value.second;
							}
							else if (value.first == "metallicFactor")
							{
								std::cout << "Found material texture type: " << value.first << std::endl;
								materialPropertiesMap[E_MaterialTextureValues::MTV_METALLIC_FACTOR] = value.second;
							}
							else if (value.first == "roughnessFactor")
							{
								std::cout << "Found material texture type: " << value.first << std::endl;
								materialPropertiesMap[E_MaterialTextureValues::MTV_ROUGHNESS_FACTOR] = value.second;
							}
							else if (value.first == "normalFactor")
							{
								std::cout << "Found material texture type: " << value.first << std::endl;
								materialPropertiesMap[E_MaterialTextureValues::MTV_NORMAL_FACTOR] = value.second;
							}
							else if (value.first == "emissiveFactor")
							{
								std::cout << "Found material texture type: " << value.first << std::endl;
								materialPropertiesMap[E_MaterialTextureValues::MTV_EMISSIVE_FACTOR] = value.second;
							}
							else if (value.first == "occlusionFactor")
							{
								std::cout << "Found material texture type: " << value.first << std::endl;
								materialPropertiesMap[E_MaterialTextureValues::MTV_OCCLUSION_FACTOR] = value.second;
							}
						});
					}					

					Ref<SpatialMaterial> generatedSpatialMaterial = memnew(SpatialMaterial);
					generatedSpatialMaterial.instance();

					// For each material property factor found, set its properties accordingly:
					for (const auto& materialProperty : materialPropertiesMap)
					{
						if (materialProperty.first == E_MaterialTextureValues::MTV_ALBEDO_BASECOLOR)
						{
							std::cout << "Found " << "Albedo/Basecolor" << " factor." << std::endl;
							const auto& rgbaArray =  materialProperty.second.number_array;
							Color albedoColor = Color(rgbaArray[0], rgbaArray[1], rgbaArray[2], rgbaArray[3]);
							generatedSpatialMaterial->set_albedo(albedoColor);
						}
						if (materialProperty.first == E_MaterialTextureValues::MTV_METALLIC_FACTOR)
						{
							std::cout << "Found " << "Metallic" << " factor." << std::endl;
							const auto& metallicValue = materialProperty.second.Factor();
							if (metallicValue >= 0)
								generatedSpatialMaterial->set_metallic(metallicValue);
						}
						if (materialProperty.first == E_MaterialTextureValues::MTV_ROUGHNESS_FACTOR)
						{
							std::cout << "Found " << "Roughness" << " factor." << std::endl;
							const auto& roughnessValue = materialProperty.second.Factor();
							if (roughnessValue >= 0)
								generatedSpatialMaterial->set_roughness(roughnessValue);
						}
						if (materialProperty.first == E_MaterialTextureValues::MTV_NORMAL_FACTOR)
						{
							std::cout << "Found " << "Normal" << " factor." << std::endl;
							const auto& scale = materialProperty.second.TextureScale();
							if (scale >= 0)
								generatedSpatialMaterial->set_normal_scale(scale);
						}
						if (materialProperty.first == E_MaterialTextureValues::MTV_EMISSIVE_FACTOR)
						{
							std::cout << "Found " << "Emissive" << " factor." << std::endl;
							const auto& emissiveArray =  materialProperty.second.number_array;
							Color emissiveColor = Color(emissiveArray[0], emissiveArray[1], emissiveArray[2]);
							generatedSpatialMaterial->set_emission(emissiveColor);
						}
						if (materialProperty.first == E_MaterialTextureValues::MTV_OCCLUSION_FACTOR)
						{
							std::cout << "Found " << "Occlusion/Specular" << " factor." << std::endl;
							const auto& strength = materialProperty.second.TextureStrength();
							if (strength >= 0)
								generatedSpatialMaterial->set_specular(strength);
						}
					}			

					// For each texture type found, load it's image
					for (const auto& textureMapItem : textureMap)
					{
						std::cout << "Loading texture item..." << std::endl;
						// Load texture item and its image data
						int textureIndex = textureMapItem.second.TextureIndex();
						const tinygltf::Texture texture = model.textures[textureIndex];
						int textureImageIndex = texture.source;
						const tinygltf::Image textureImage = model.images[textureImageIndex];

						// Copy image bytes from texture image data vector to a PoolByteArray, a Godot-specific class.
						PoolByteArray imageBytes {};
						for (unsigned char byte : textureImage.image) imageBytes.append(byte);

						// Removed code: another way to load image data. Doesn't work.
						/*
						const tinygltf::BufferView& bufferView = model.bufferViews[textureImage.bufferView];
						const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];
						const int byteOffset = bufferView.byteOffset;
						const std::vector<unsigned char>& bufferData = buffer.data;
						for (int i = 0; i < bufferView.byteLength; i++)
						{
							imageBytes.push_back(bufferData[byteOffset + i]);
						}
						*/

						std::cout << "textureImage bytes count: " << textureImage.image.size() << std::endl;
						std::cout << "imageBytes bytes count: " << imageBytes.size() << std::endl;

						// Image generation for assign it to a Texture
						Ref<Image> generatedImage = memnew(Image);
						generatedImage.instance();
						Image::Format imageFormat;
						switch(textureImage.pixel_type)
						{
							case TINYGLTF_COMPONENT_TYPE_BYTE:
							case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
								std::cout << "Image format recognized as " << "RGBA8" << std::endl;
								imageFormat = Image::Format::FORMAT_RGBA8;
								break;
							case TINYGLTF_COMPONENT_TYPE_SHORT:
							case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
								std::cout << "Image format recognized as " << "RGBAH" << std::endl;
								imageFormat = Image::Format::FORMAT_RGBAH;
								break;
							case TINYGLTF_COMPONENT_TYPE_INT:
							case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
							case TINYGLTF_COMPONENT_TYPE_FLOAT:
								std::cout << "Image format recognized as " << "RGBAF" << std::endl;
								imageFormat = Image::Format::FORMAT_RGBAF;
								break;
							case TINYGLTF_COMPONENT_TYPE_DOUBLE:
								std::cout << "Warning: This format is experimental here and should handled with caution." << std::endl;
								std::cout << "Bypass: Image format recognized as " << "RGBAF" << std::endl;
								imageFormat = Image::Format::FORMAT_RGBAF;
								break;
							default:
								std::cout << "Warning: Image format not recognized. Assuming RGBA8..." << std::endl;
								imageFormat = Image::Format::FORMAT_RGBA8;
								break;
						}
						generatedImage->create(textureImage.width, textureImage.height, false, imageFormat, imageBytes);
						
						// Removed code: Format-specific image loading
						/*
						const std::string& imageType = textureImage.mimeType;
						std::cout << "Image type: " << imageType << std::endl;
						if (imageType == "image/jpeg")
						{
							std::cout << "Loading image type: " << imageType << std::endl;
							generatedImage->load_jpg_from_buffer(imageBytes);
						}
						else if (imageType == "image/png")
						{
							std::cout << "Loading image type: " << imageType << std::endl;
							generatedImage->load_png_from_buffer(imageBytes);
						}
						else
						{
							std::cout << "Warning: Invalid texture image mimetype" << std::endl;
						}
						*/

						// Get generated image and assign it to a texture
						Ref<ImageTexture> generatedImageTexture = memnew(ImageTexture);
						generatedImageTexture.instance();
						generatedImageTexture->create_from_image(generatedImage);

						// Test: write image bytes to a file.
						std::string filename = "test_";
						if (textureImage.mimeType == "image/png")
						{
							filename += std::string("png");
						}
						if (textureImage.mimeType == "image/jpeg")
						{
							filename += std::string("jpeg");
						}
						

						// Set texture properties
						switch(textureMapItem.first)
						{
							case E_TextureTypes::TT_ALBEDO:
								filename += std::string("albedo");
								std::cout << "Loaded texture type: " << "ALBEDO" << std::endl;
								generatedSpatialMaterial->set_texture(SpatialMaterial::TEXTURE_ALBEDO, generatedImageTexture);
								break;
							case E_TextureTypes::TT_METALLIC_ROUGHNESS:
								filename += std::string("roughnessOrMetallic");
								std::cout << "Loaded texture type: " << "METALLIC/ROUGHNESS" << std::endl;
								generatedSpatialMaterial->set_texture(SpatialMaterial::TEXTURE_METALLIC, generatedImageTexture);
								generatedSpatialMaterial->set_texture(SpatialMaterial::TEXTURE_ROUGHNESS, generatedImageTexture);
								break;
							case E_TextureTypes::TT_NORMAL:
								filename += std::string("normal");
								std::cout << "Loaded texture type: " << "NORMAL" << std::endl;
								generatedSpatialMaterial->set_texture(SpatialMaterial::TEXTURE_NORMAL, generatedImageTexture);
								break;
							case E_TextureTypes::TT_EMISSIVE:
								filename += std::string("emission");
								std::cout << "Loaded texture type: " << "EMISSIVE" << std::endl;
								generatedSpatialMaterial->set_texture(SpatialMaterial::TEXTURE_EMISSION, generatedImageTexture);
								break;
							case E_TextureTypes::TT_OCCLUSION:
								filename += std::string("occlusion");
								std::cout << "Loaded texture type: " << "AMBIENT OCCLUSION" << std::endl;
								generatedSpatialMaterial->set_texture(SpatialMaterial::TEXTURE_AMBIENT_OCCLUSION, generatedImageTexture);
								break;
							default:
								std::cout << "Warning: Tried to load a unsupported texture type." << std::endl;
								filename += std::string("unrecognized");
								break;
						}

						filename += std::string(".");

						if (textureImage.mimeType == "image/png")
						{
							filename += std::string("png");
						}
						if (textureImage.mimeType == "image/jpeg")
						{
							filename += std::string("jpeg");
						}

						// TEST: write image data to file
						{
							std::FILE *file;
							file = std::fopen(filename.c_str(), "wb+");
							int size = generatedImage->get_data().size();
							std::vector<uint8_t> datavector {};
							for (int i = 0; i < size; i++) datavector.push_back(generatedImage->get_data()[i]);
							std::fwrite(&(datavector[0]), sizeof(uint8_t), size, file);
							std::fclose(file);
							std::cout << "Written a texture image file at " << filename << std::endl;
						}
					}

					// Result Debugging
					{
						const auto& data1 = generatedSpatialMaterial->get_albedo();
						const auto& data2 = generatedSpatialMaterial->get_metallic();
						const auto& data3 = generatedSpatialMaterial->get_roughness();
						const auto& data4 = generatedSpatialMaterial->get_normal_scale();
						const auto& data5 = generatedSpatialMaterial->get_emission();
						const auto& data6 = generatedSpatialMaterial->get_specular();

						std::cout << "Final material result" << std::endl;
						std::cout << "- " << "Basecolor (Albedo) RGBA: " << data1.r  << " " << data1.g << " " << data1.b << " " << data1.a << std::endl;
						std::cout << "- " << "Metallic Value: " << data2 << std::endl;
						std::cout << "- " << "Roughness Value: " << data3 << std::endl;
						std::cout << "- " << "Normal Scale Value: " << data4 << std::endl;
						std::cout << "- " << "Emission Value (RGBA): " << data5.r << " " << data5.g << " " << data5.b << " " << data5.a << std::endl;
						std::cout << "- " << "Specular Value: " << data6 << std::endl;
					}

					std::cout << "Setting material..." << std::endl;
					st->set_material(generatedSpatialMaterial);
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
