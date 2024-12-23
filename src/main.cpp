#include <iostream>
#include <fstream>
#include <memory>
#include <bit>
#include <cstring>
#include <SFML/Graphics.hpp>

constexpr unsigned char TAG_MASK    = 0xc0;    /* 11000000 */

constexpr unsigned char OP_INDEX    = 0x00;    /* 00xxxxxx */
constexpr unsigned char OP_DIFF     = 0x40;    /* 01xxxxxx */
constexpr unsigned char OP_LUMA     = 0x80;    /* 10xxxxxx */
constexpr unsigned char OP_RUN      = 0xc0;    /* 11xxxxxx */
constexpr unsigned char OP_RGB      = 0xfe;    /* 11111110 */
constexpr unsigned char OP_RGBA     = 0xfe;    /* 11111111 */

struct QOIHeader {
    char magic[4];
    uint32_t width;
    uint32_t height;
    uint8_t channels;
    uint8_t colorspace;

    void reverseBytes() {
        width = std::byteswap(width);
        height = std::byteswap(height);
    }
};

struct ImBuffer {
    sf::Uint32 width;
    sf::Uint32 height;
    sf::Uint8* data;
};

int hash(const sf::Color& color) {
    return (color.r * 3 + color.g * 5 + color.b * 7 + color.a * 11) % 64;
}

ImBuffer loadQOI(const std::string& path) {
    std::ifstream file { path, std::ios::in | std::ios::ate | std::ios::binary };

    if (!file.is_open()) {
        std::cerr << "Failed to open file: \"" << path << "\"." << std::endl;
        std::exit(74);
    }

    const std::streamsize size = file.tellg();
    std::cout << "file size: " << size << std::endl;
    file.seekg(0, std::ios::beg);

    const auto buffer = std::make_unique<unsigned char[]>(size);
    if (buffer == nullptr) {
        std::cerr << "Failed to allocate buffer." << std::endl;
        std::exit(74);
    }

    if (!file.read(reinterpret_cast<std::istream::char_type *>(buffer.get()), size)) {
        std::cerr << "Failed to read file \"" << path << "\"." << std::endl;
        std::exit(74);
    }

    file.close();

    QOIHeader header {};
    std::memcpy(&header, buffer.get(), 14);
    header.reverseBytes();

    const unsigned char* byte = &buffer.get()[14];

    sf::Color pixelCache[64];
    sf::Color previousPixel {0, 0, 0, 255};

    auto* pixels = new sf::Uint8[header.width * header.height * sizeof(sf::Color)];
    int pos = 0;
    int p = 0;
    int run = 0;

    while (pos < header.width * header.height * sizeof(sf::Color)) {
        if (run > 0) {
            run--;
        } else {
            if (int b1 = byte[p++]; b1 == OP_RGB) {
                previousPixel.r = byte[p++];
                previousPixel.g = byte[p++];
                previousPixel.b = byte[p++];
            } else if (b1 == OP_RGBA) {
                previousPixel.r = byte[p++];
                previousPixel.g = byte[p++];
                previousPixel.b = byte[p++];
                previousPixel.a = byte[p++];
            } else if ((b1 & TAG_MASK) == OP_DIFF) {
                previousPixel.r += ((b1 >> 4) & 0x03) - 2;
                previousPixel.g += ((b1 >> 2) & 0x03) - 2;
                previousPixel.b += (b1 & 0x03) - 2;
            } else if ((b1 & TAG_MASK) == OP_LUMA) {
                int b2 = byte[p++];
                int vg = (b1 & 0x3f) - 32;
                previousPixel.r += vg - 8 + ((b2 >> 4) & 0x0f);
                previousPixel.g += vg;
                previousPixel.b += vg - 8 + ((b2) & 0x0f);
            } else if ((b1 & TAG_MASK) == OP_RUN) {
                run = (b1 & 0x3f);
            } else if ((b1 & TAG_MASK) == OP_INDEX) {
                previousPixel = pixelCache[b1];
            } else {
                break;
            }
            pixelCache[hash(previousPixel)] = previousPixel;
        }

        pixels[pos + 0] = previousPixel.r;
        pixels[pos + 1] = previousPixel.g;
        pixels[pos + 2] = previousPixel.b;
        pixels[pos + 3] = previousPixel.a;
        pos += 4;
    }

    return ImBuffer {header.width, header.height, pixels};
}

int main()
{
    sf::Image image;

    if (!image.loadFromFile(R"(C:\Users\emiln\Downloads\qoi_test_images\qoi_test_images\dice.png)")) {
        std::cerr << "Cannot load image." << std::endl;
        return 1;
    }
    sf::Texture texture;
    texture.loadFromImage(image);
    sf::Sprite sprite(texture);
    sprite.setPosition(0.0f, 0.0f);

    auto imbuffer = loadQOI(R"(C:\Users\emiln\Downloads\qoi_test_images\qoi_test_images\dice.qoi)");
    sf::Image qoiImage;
    qoiImage.create(imbuffer.width, imbuffer.height, imbuffer.data);

    sf::Texture qoiTexture;
    qoiTexture.loadFromImage(qoiImage);
    sf::Sprite qoiSprite(qoiTexture);
    qoiSprite.setPosition(sprite.getTextureRect().width, 0.0f);

    auto window = sf::RenderWindow({static_cast<unsigned int>(sprite.getTextureRect().width) * 2, static_cast<unsigned int>(sprite.getTextureRect().height)}, "QOI Decoder");
    window.setVerticalSyncEnabled(true);

    while (window.isOpen())
    {
        for (auto event = sf::Event(); window.pollEvent(event);)
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }

        window.clear();
        window.draw(sprite);
        window.draw(qoiSprite);
        window.display();
    }
}
