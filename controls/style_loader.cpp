#include "style_loader.hpp"
#include <stdexcept>
#include <sstream>
#include <unordered_map>
#include <SFML/Graphics.hpp>

static void must_getline(std::istream& ifs, std::string& line) {
    if (!std::getline(ifs, line)) {
        throw std::runtime_error("Expecting lines, got eof!");
    }
}

static void must_get_tokens(const std::string& line,const std::string& token) {
    std::string dummy;
    std::stringstream ss(line);
    if (!(ss >> dummy)) {
        throw std::runtime_error("Reading failure!");
    } else if (dummy.compare(token) != 0) {
        throw std::runtime_error("Expecting " + token + " got " + dummy);
    }
}

static void must_get_tokens( const std::string& line
                           , const std::string& token
                           , std::string& result ) {
    std::string dummy;
    std::stringstream ss(line);
    if (!(ss >> dummy >> result)) {
        throw std::runtime_error("Reading failure!");
    } else if (dummy.compare(token) != 0) {
        throw std::runtime_error("Expecting " + token + " got " + dummy);
    }
}

static void load_color_def(std::istream& is, const std::string& name, TextureManager& tm) {
    int r = 0, g = 0, b = 0, alpha = 0;
    if (!(is >> r >> g >> b >> alpha)) {
        throw std::runtime_error("WRONG DEFINITION OF COLOR!");
    }
    tm.load_color(name, sf::Color(r, g, b, alpha));
}

static GStyle load_style(std::istream& is, const TextureManager& tm) {
    std::string line, result;
    must_getline(is, line);
    must_get_tokens(line, "BACKGROUND", result);
    auto&& bck = tm.get_color(result);

    must_getline(is, line);
    must_get_tokens(line, "BACKGROUND_HIGH", result);
    auto&& bck_h = tm.get_color(result);

    must_getline(is, line);
    must_get_tokens(line, "BORDER", result);
    auto&& brdr = tm.get_color(result);

    must_getline(is, line);
    must_get_tokens(line, "BORDER_HIGH", result);
    auto&& brdr_h = tm.get_color(result);

    must_getline(is, line);
    must_get_tokens(line, "TEXT", result);
    auto&& txt = tm.get_color(result);

    must_getline(is, line);
    must_get_tokens(line, "TEXT_HIGH", result);
    auto&& txt_h = tm.get_color(result);

    must_getline(is, line);
    must_get_tokens(line, "FONT", result);
    auto&& font = tm.get_font(result);

    must_getline(is, line);
    must_get_tokens(line, "BORDER_SIZE", result);
    float size = std::stof(result);

    must_getline(is, line);
    must_get_tokens(line, "DEFAULT_WIDTH", result);
    float default_width = std::stof(result);

    must_getline(is, line);
    must_get_tokens(line, "SPACING_X", result);
    float spacing_x = std::stof(result);

    must_getline(is, line);
    must_get_tokens(line, "SPACING_Y", result);
    float spacing_y = std::stof(result);

    return GStyle(bck, bck_h, brdr, brdr_h, txt, txt_h,
                 font, size, default_width, spacing_x, spacing_y);
}

StyleLoader::StyleLoader(const std::string& file_name
                        , TextureManager& tm)
                        : ifs(file_name)
                        , tm(tm) {
    if (!ifs) {
        throw std::runtime_error("Could not load state style config file: " + file_name);
    }
}

void StyleLoader::load(StylesHolder<GStyle>& sh) {
    std::string line, token, name;
    while (std::getline(ifs, line)) {
        std::stringstream ss(line);
        if (!(ss >> token)) {
            continue; // ignore empty lines
        }
        if ((token.compare("STYLE") == 0) && (ss >> name )) {
            auto&& style = load_style(ifs, tm);
            sh.add_style(name, std::move(style));
        } else if ((token.compare("DEFINE") == 0) && (ss >> name)) {
            load_color_def(ifs, name, tm);
        } else {
            throw std::runtime_error("INVALID LINE: "+line);
        }
    }
}

static CGType must_get_type(const std::string& type_str) {
    if (type_str.compare("HORIZONTAL") == 0) {
        return CGType::HORIZONTAL;
    } else if (type_str.compare("VERTICAL") == 0) {
        return CGType::VERTICAL;
    } else {
        throw std::runtime_error("Invalid type! (" + type_str + ")");
    }
}

static CGStyle load_cgstyle(std::istream& is, const StylesHolder<GStyle>& sh) {
    std::string line, result;
    must_getline(is, line);
    must_get_tokens(line, "BUTTON_STYLE", result);
    const GStyle* btn_style = nullptr;
    if (result.compare("NULL") != 0) {
        btn_style = &sh.get_style(result);
    }

    must_getline(is, line);
    must_get_tokens(line, "TEXT_STYLE", result);
    const GStyle* txt_style = nullptr;
    if (result.compare("NULL") != 0) {
        txt_style = &sh.get_style(result);
    }

    must_getline(is, line);
    must_get_tokens(line, "DEFAULT_WIDTH", result);
    auto def_width = std::stof(result);

    must_getline(is, line);
    must_get_tokens(line, "X", result);
    auto x = std::stof(result);

    must_getline(is, line);
    must_get_tokens(line, "Y", result);
    auto y = std::stof(result);

    must_getline(is, line);
    must_get_tokens(line, "LETTER_SIZE", result);
    auto letter_size = std::stoi(result);

    must_getline(is, line);
    must_get_tokens(line, "FACTOR", result);
    auto factor = std::stof(result);

    must_getline(is, line);
    must_get_tokens(line, "TYPE", result);
    auto type = must_get_type(result);

    return CGStyle(btn_style, txt_style, def_width, x, y, letter_size, factor, type);
}

CGStyleLoader::CGStyleLoader(const std::string& file_name
                        , const StylesHolder<GStyle>& sh)
                        : ifs(file_name)
                        , sh(sh) {
    if (!ifs) {
        throw std::runtime_error("Could not load state style config file: " + file_name);
    }
}

void CGStyleLoader::load(StylesHolder<CGStyle>& cgsh) {
    std::string line, token, name;
    while (std::getline(ifs, line)) {
        std::stringstream ss(line);
        if (!(ss >> token)) {
            continue; // ignore empty lines
        }
        if ((token.compare("MENU") == 0) && (ss >> name )) {
            auto&& style = load_cgstyle(ifs, sh);
            cgsh.add_style(name, std::move(style));
        } else {
            throw std::runtime_error("INVALID LINE: "+line);
        }
    }
}