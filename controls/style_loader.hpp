#ifndef GAME_STATE_STYLE_LOADER
#define GAME_STATE_STYLE_LOADER

#include <fstream>
#include <string>
#include <unordered_map>
#include "controls/control_field.hpp"
#include "texture_handling/texture_manager.hpp"

using Styles = std::unordered_map<std::string, GStyle>;

class StylesHolder {
public:
    void add_style(const std::string& name, GStyle&& gstyle);
    const GStyle& get_style(const std::string& name) const;
private:
    Styles styles;
};

class StyleLoader {
public:
    StyleLoader( const std::string& file_name, const TextureManager&);
    void load(StylesHolder&);
private:
    std::ifstream ifs;
    const TextureManager& tm;
};

#endif
