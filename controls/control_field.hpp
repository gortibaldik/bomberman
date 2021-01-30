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
    ACTIVE
};

class ControlGrid;

class ControlField {
public:
    virtual void render(sf::RenderTarget* target);
    virtual void handle_input(const sf::Vector2f& mouse_position, const sf::Event& e);
    virtual void update();
    virtual void move_pos(float, unsigned int, unsigned int);

    float get_height() const;
    float get_width() const;
    std::string get_text() const;
protected:
    ControlField( float x,
            float y,
            float letter_width,
            GStyle* style,
            ControlGrid* grid);
    sf::FloatRect move_position(float,
                                unsigned int,
                                unsigned int,
                                float default_width = 0.f,
                                float default_height = 0.f);
    sf::RectangleShape shape;
    GStyle* style;
    ControlGrid* grid;
    CONTROL_STATE state;
    float letter_width;
    sf::Text text;
};
using CFPtr = std::unique_ptr<ControlField>;
#endif