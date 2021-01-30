#ifndef CONTROL_FIELD_BOMBERMAN_HPP
#define CONTROL_FIELD_BOMBERMAN_HPP
#include <SFML/Graphics.hpp>
#include <memory>


struct GStyle {
    sf::Color cbackground;
    sf::Color cbackground_highlight;
    sf::Color cborder;
    sf::Color cborder_highlight;
    sf::Color ctext;
    sf::Color ctext_highlight;
    const sf::Font* font;
    float border_size;

    GStyle(const sf::Color& cbackground,
            const sf::Color& cback_high,
            const sf::Color& cbord,
            const sf::Color& cbord_high,
            const sf::Color& ctext,
            const sf::Color& ctext_high,
            const sf::Font* font,
            float bsize):   cbackground(cbackground),
                            cbackground_highlight(cback_high),
                            cborder(cbord),
                            cborder_highlight(cbord_high),
                            ctext(ctext),
                            ctext_highlight(ctext_high),
                            font(font),
                            border_size(bsize) {}
};

enum CONTROL_STATE {
    IDLE,
    HOVER,
    PRESSED
};

class ControlGrid;

class ControlField {
public:
    ControlField( float x,
            float y,
            float letter_width,
            const std::string& text,
            GStyle* style,
            ControlGrid* grid);
    void render(sf::RenderTarget* target);
    void handle_input(const sf::Vector2f& mouse_position, const sf::Event& e);
    void update();
    void move_pos(float, unsigned int, unsigned int);

    float get_height() const;
    std::string get_text() const;
private:
    sf::RectangleShape shape;
    GStyle* style;
    ControlGrid* grid;
    CONTROL_STATE state;
    float letter_width;
    sf::Text text;
};
using CFPtr = std::unique_ptr<ControlField>;
#endif