#pragma once

namespace qoi {
    constexpr int HeaderSize = 14;
    constexpr unsigned char Padding[8] {0, 0, 0, 0, 0, 0, 0, 1};
    constexpr unsigned int Magic = static_cast<unsigned int>('q') << 24 | static_cast<unsigned int>('o') << 16 | static_cast<unsigned int>('i') << 8 | static_cast<unsigned int>('f');
    constexpr unsigned int MaxPixels = 400000000;

    constexpr unsigned char TAG_MASK    = 0xc0;    /* 11000000 */

    constexpr unsigned char OP_INDEX    = 0x00;    /* 00xxxxxx */
    constexpr unsigned char OP_DIFF     = 0x40;    /* 01xxxxxx */
    constexpr unsigned char OP_LUMA     = 0x80;    /* 10xxxxxx */
    constexpr unsigned char OP_RUN      = 0xc0;    /* 11xxxxxx */
    constexpr unsigned char OP_RGB      = 0xfe;    /* 11111110 */
    constexpr unsigned char OP_RGBA     = 0xff;    /* 11111111 */

    enum Mode: unsigned char {
        RGB = 3, RGBA = 4
    };

    struct rgba {
        unsigned char r = 0;
        unsigned char g = 0;
        unsigned char b = 0;
        unsigned char a = 255;
    };

    inline int hash(const rgba color) {
        return (color.r * 3 + color.g * 5 + color.b * 7 + color.a * 11) % 64;
    }

    struct Image {
        unsigned int width;
        unsigned int height;
        unsigned char channels;
        unsigned char colorspace;
        unsigned char* data;

        ~Image() {
            delete data;
        }
    };

    unsigned int read32(const unsigned char* data, int* p);
    Image decode(const void* data, int size, Mode mode);
}
