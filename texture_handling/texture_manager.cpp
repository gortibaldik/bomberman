#include "texture_manager.hpp"

void TextureManager::load_texture(const std::string& name, const std::string& filename) {
    textures[name].loadFromFile(filename);
}

sf::Texture& TextureManager::get_ref(const std::string& texture) {
    return textures[texture];
}