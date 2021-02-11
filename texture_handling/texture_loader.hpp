#ifndef TEXTURE_LOADER_HPP
#define TEXTURE_LOADER_HPP
#include <string>
#include <fstream>
#include "texture_manager.hpp"

class TextureLoader {
public:
    TextureLoader(const std::string& name_of_config_file);
    void load(TextureManager&);
private:
    std::ifstream ifs;
};

#endif
