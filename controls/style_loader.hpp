#ifndef GAME_STATE_STYLE_LOADER
#define GAME_STATE_STYLE_LOADER

#include <fstream>
#include <string>
#include <unordered_map>
#include "control_field.hpp"
#include "control_grid.hpp"
#include "texture_handling/texture_manager.hpp"

template<typename T>
class StylesHolder {
public:
    void add_style(const std::string& name, T&& style) {
        styles.emplace(name, std::move(style));
    }
    const T& get_style(const std::string& name) const {
        auto it = styles.find(name);
        if (it == styles.end()) {
            throw std::runtime_error("Unknown style: " + name);
        }
        return it->second;
    }
private:
    std::unordered_map<std::string, T> styles;
};

class StyleLoader {
public:
    StyleLoader( const std::string& file_name, TextureManager&);
    void load(StylesHolder<GStyle>&);
private:
    std::ifstream ifs;
    TextureManager& tm;
};

class CGStyleLoader {
public:
    CGStyleLoader( const std::string& file_name, const StylesHolder<GStyle>&);
    void load(StylesHolder<CGStyle>&);
private:
    std::ifstream ifs;
    const StylesHolder<GStyle>& sh;
};

#endif
