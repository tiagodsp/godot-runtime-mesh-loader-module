#include "TextureUtils.h"
#include "RygsDXTc/stb_dxt.h"
#include "core/pool_vector.h"
#include <vector>

bool TextureUtils::ConvertRGBA8ToDXT(Ref<Image> *destination, const Ref<Image>& source)
{
    std::cout << "# Compression: RGBA8 to DXT" << std::endl;
    

    // Create destination vector, get a reference to source pointer, get width and height
    
    std::vector<unsigned char> src = std::vector<unsigned char>();
    for (unsigned int i = 0; i < source->get_data().size(); i++)
    {
        src.push_back(source->get_data().get(i));
    }

    // Info: Pointer to the Destination memory buffer. The buffer must already be allocated. The required size of the buffer is : for DXT1 : dstSize = srcSize / 8 for DXT5 : dstSize = srcSize / 4
    const int src_size = src.size();
    const int dst_size = src_size / 8;
    std::cout << "Original texture size: " << src_size << std::endl;
    std::cout << "Compressed texture size: " << dst_size << std::endl;

    std::vector<unsigned char> dst = std::vector<unsigned char>(dst_size);

    unsigned char* srcPtr = &src[0];
    unsigned char* dstPtr = &dst[0];
    int src_width = source->get_width();
    int src_height = source->get_height();
    std::cout << "Image dimension size: " << src_width << "x" << src_height << std::endl;

    // Compress data
    std::cout << "Compressing image..." << std::endl;
    rygCompress(dstPtr, srcPtr, src_width, src_height, 0);

    // Copy compressed data to a Godot usable format
    PoolVector<uint8_t> dst_pv = PoolVector<uint8_t>();
    for (unsigned int i = 0; i < dst_size; i++)
    {
        dst_pv.push_back(dst[i]);
    }
    std::cout << "Image compressed. Creating image..." << std::endl;
    (*destination)->create(src_width, src_height, false, Image::FORMAT_DXT1, dst_pv);

    return true;
}
