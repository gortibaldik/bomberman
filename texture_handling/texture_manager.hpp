#ifndef TEXTURE_MANAGER_HPP
#define TEXTURE_MANAGER_HPP

#include <SFML/Graphics.hpp>
#include <string>
#include <map>

class TextureManager {
public:
    void load_texture(const std::string& name, const std::string& filename);
    sf::Texture& get_ref(const std::string& texture);
private:
    std::map<std::string, sf::Texture> textures;
};
#endif