#include "TextureUtils.h"
#include "RygsDXTc/stb_dxt.h"
#include "core/pool_vector.h"
#include <vector>

bool TextureUtils::ConvertRGBA8ToDXT(Ref<Image> *destination, const Ref<Image>& source)
{
    std::cout << "# Compression: RGBA8 to DXT" << std::endl;

    // Info: Pointer to the Destination memory buffer. The buffer must already be allocated. The required size of the buffer is : for DXT1 : dstSize = srcSize / 8 for DXT5 : dstSize = srcSize / 4
    const int src_size = source->get_data().size();
    const int dst_size = src_size / 8;
    std::cout << "Original texture size: " << src_size << std::endl;
    std::cout << "Compressed texture size: " << dst_size << std::endl;
    std::cout << "Image dimension size: " << source->get_width() << "x" << source->get_height() << std::endl;

    // Create usables vectors for usage in compression source and destination arguments
    std::vector<unsigned char> dst = std::vector<unsigned char>(dst_size);
    {
        // Make a copy of the source vector into a scope, deleting it after usage.
        std::vector<unsigned char> src = std::vector<unsigned char>();
        for (unsigned int i = 0; i < source->get_data().size(); i++)
        {
            src.push_back(source->get_data().get(i));
        }

        // Compress data with RygsDXT library
        std::cout << "Compressing image..." << std::endl;
        rygCompress((unsigned char *) &dst[0], (unsigned char *) &src[0], source->get_width(), source->get_height(), false);
    }

    // Copy compressed data to a Godot usable format
    PoolVector<uint8_t> dst_pv = PoolVector<uint8_t>();
    for (unsigned int i = 0; i < dst_size; i++)
    {
        dst_pv.push_back(dst[i]);
    }

    // Create image with data
    std::cout << "Image compressed. Creating image..." << std::endl;
    (*destination)->create(source->get_width(), source->get_height(), false, Image::FORMAT_DXT1, dst_pv);

    return true;
}
