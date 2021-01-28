#ifndef BUTTON_BOMBERMAN_HPP
#define BUTTON_BOMBERMAN_HPP
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

enum BTN_STATE {
    IDLE,
    HOVER,
    PRESSED
};

class Button {
public:
    Button( float x,
            float y,
            float height,
            const std::string& text,
            GStyle* style);
    void render(sf::RenderTarget* target);
    void handle_input(const sf::Vector2f& mouse_position);
    void update();
    void move_pos(float, float);
private:
    sf::RectangleShape shape;
    GStyle* style;
    BTN_STATE state;
    sf::Text text;
};
using ButtonPtr = std::unique_ptr<Button>;
#endif