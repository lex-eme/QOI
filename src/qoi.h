#pragma once

namespace qoi {

    enum Mode: int {
        RGB = 3, RGBA = 4
    };

    struct Image {
        unsigned int width;
        unsigned int height;
        unsigned char channels;
        unsigned char colorspace;
        unsigned char* data;
    };

    Image decode(const unsigned char* data, int size, Mode mode);
}
