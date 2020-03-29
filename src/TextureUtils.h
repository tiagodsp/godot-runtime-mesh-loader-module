#pragma once
#include "core/image.h"

class TextureUtils
{
    public:
        static bool ConvertRGBA8ToDXT(Ref<Image> destination, const Ref<Image> source);
};
