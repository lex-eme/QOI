#include "qoi.h"

#include <cstring>

#include "ProfileTimer.h"

unsigned int qoi::read32(const unsigned char *data, int *p) {
    const unsigned int a = data[(*p)++];
    const unsigned int b = data[(*p)++];
    const unsigned int c = data[(*p)++];
    const unsigned int d = data[(*p)++];
    return a << 24 | b << 16 | c << 8 | d;
}

qoi::Image qoi::decode(const void *data, const int size, const Mode mode) {
    PROFILE_FUNCTION();
    rgba cache[64];
    memset(cache, 0, sizeof(cache));
    rgba pixel;
    int p = 0, run = 0;

    if (data == nullptr || size < HeaderSize + sizeof(Padding)) {
        return Image {};
    }

    const auto *bytes = static_cast<const unsigned char *>(data);

    Image image {};

    const unsigned int headerMagic = read32(bytes, &p);
    image.width = read32(bytes, &p);
    image.height = read32(bytes, &p);
    image.channels = bytes[p++];
    image.colorspace = bytes[p++];

    if (image.width == 0 || image.height == 0 || image.channels < 3 || image.channels > 4 || image.colorspace > 1 || headerMagic != Magic || image.height >= MaxPixels / image.height) {
        return image;
    }

    image.channels = mode;

    const unsigned int pixelsLength = image.width * image.height * image.channels;
    auto *pixels = new unsigned char[pixelsLength];

    const int chunksLength = size - sizeof(Padding);
    for (int pixelPos = 0; pixelPos < pixelsLength; pixelPos += image.channels) {
        if (run > 0) {
            run--;
        } else if (p < chunksLength) {
            if (const int b1 = bytes[p++]; b1 == OP_RGB) {
                pixel.r = bytes[p++];
                pixel.g = bytes[p++];
                pixel.b = bytes[p++];
            } else if (b1 == OP_RGBA) {
                pixel.r = bytes[p++];
                pixel.g = bytes[p++];
                pixel.b = bytes[p++];
                pixel.a = bytes[p++];
            } else if ((b1 & TAG_MASK) == OP_DIFF) {
                pixel.r += (b1 >> 4 & 0x03) - 2;
                pixel.g += (b1 >> 2 & 0x03) - 2;
                pixel.b += (b1 & 0x03) - 2;
            } else if ((b1 & TAG_MASK) == OP_LUMA) {
                const int b2 = bytes[p++];
                const int vg = (b1 & 0x3f) - 32;
                pixel.r += vg - 8 + (b2 >> 4 & 0x0f);
                pixel.g += vg;
                pixel.b += vg - 8 + (b2 & 0x0f);
            } else if ((b1 & TAG_MASK) == OP_RUN) {
                run = b1 & 0x3f;
            } else if ((b1 & TAG_MASK) == OP_INDEX) {
                pixel = cache[b1];
            }
            cache[hash(pixel)] = pixel;
        }

        pixels[pixelPos + 0] = pixel.r;
        pixels[pixelPos + 1] = pixel.g;
        pixels[pixelPos + 2] = pixel.b;

        if (mode == RGBA) {
            pixels[pixelPos + 3] = pixel.a;
        }
    }

    image.data = pixels;

    return image;
}
