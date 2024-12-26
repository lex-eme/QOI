#include <iostream>
#include <fstream>
#include <memory>
#include <SFML/Graphics.hpp>

#include "qoi.h"

bool loadQoiImageFromFile(sf::Image& image, const std::string& path) {
    std::ifstream file { path, std::ios::in | std::ios::ate | std::ios::binary };

    if (!file.is_open()) {
        std::cerr << "Failed to open file: \"" << path << "\"." << std::endl;
        return false;
    }

    const std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    const auto buffer = std::make_unique<unsigned char[]>(size);
    if (buffer == nullptr) {
        std::cerr << "Failed to allocate buffer." << std::endl;
        return false;
    }

    if (!file.read(reinterpret_cast<std::istream::char_type *>(buffer.get()), size)) {
        std::cerr << "Failed to read file \"" << path << "\"." << std::endl;
        return false;
    }

    file.close();

    const qoi::Image qoiImage = decode(buffer.get(), static_cast<int>(size), qoi::Mode::RGBA);
    if (qoiImage.data == nullptr) {
        return false;
    }
    image.create(qoiImage.width, qoiImage.height, qoiImage.data);

    return true;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        std::cerr << "Usage: qoi [path]" << std::endl;
    }
    std::string path = argv[1];

    sf::Image pngImage;
    if (!pngImage.loadFromFile(path + ".png")) {
        std::cerr << "Cannot load png image." << std::endl;
        return 1;
    }
    sf::Texture texture;
    texture.loadFromImage(pngImage);
    sf::Sprite sprite(texture);
    sprite.setPosition(0.0f, 0.0f);

    sf::Image qoiImage;
    if (!loadQoiImageFromFile(qoiImage, path + ".qoi")) {
        std::cerr << "Cannot load qoi image." << std::endl;
    }
    sf::Texture qoiTexture;
    qoiTexture.loadFromImage(qoiImage);
    sf::Sprite qoiSprite(qoiTexture);
    qoiSprite.setPosition(static_cast<float>(sprite.getTextureRect().width), 0.0f);

    auto window = sf::RenderWindow({static_cast<unsigned int>(sprite.getTextureRect().width) * 2, static_cast<unsigned int>(sprite.getTextureRect().height)}, "QOI Decoder");
    window.setVerticalSyncEnabled(true);

    while (window.isOpen()) {
        for (auto event = sf::Event(); window.pollEvent(event);) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        window.clear(sf::Color::Cyan);
        window.draw(sprite);
        window.draw(qoiSprite);
        window.display();
    }
}
