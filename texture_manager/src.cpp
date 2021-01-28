#include "def.hpp"

void TextureManager::load_texture(const std::string& name, const std::string& filename) {
    sf::Texture tex;
    tex.loadFromFile(filename);
    textures[name] = std::move(tex);
}

sf::Texture& TextureManager::get_ref(const std::string& texture) {
    return textures[texture];
}