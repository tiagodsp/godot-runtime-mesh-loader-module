#include "TextureUtils.h"
#include "RygsDXTc/stb_dxt.h"
#include "core/pool_vector.h"
#include <vector>

bool TextureUtils::ConvertRGBA8ToDXT(Ref<Image> destination, const Ref<Image> source)
{
    // Info: Pointer to the Destination memory buffer. The buffer must already be allocated. The required size of the buffer is : for DXT1 : dstSize = srcSize / 8 for DXT5 : dstSize = srcSize / 4
    int src_size = source->get_data().size();
    int dst_size = src_size / 8;

    // Allocate destination PoolVector memory space and get its pointer
    PoolVector<uint8_t> dst_pv = PoolVector<uint8_t>();
    dst_pv.resize(dst_size);
    PoolVector<uint8_t>::Write w_dst_pv =  dst_pv.write();
    
    {
        // Create usables vectors for usage in compression source and destination arguments
        std::vector<unsigned char> dst = std::vector<unsigned char>(dst_size);

        // Get source PoolVector memory pointer
        std::vector<unsigned char> src = std::vector<unsigned char>();
        PoolVector<uint8_t>::Read r_src = source->get_data().read();
        
        // Compress data with RygsDXT library
        rygCompress((unsigned char *) &w_dst_pv[0], (unsigned char *) &r_src[0], source->get_width(), source->get_height(), false);
    }

    // Create image with data
    destination->create(source->get_width(), source->get_height(), false, Image::FORMAT_DXT1, dst_pv);

    return true;
}
