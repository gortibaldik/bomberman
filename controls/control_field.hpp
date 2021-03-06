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
    const sf::Font& font;
    float border_size;
    float default_width;
    float spacing_x;
    float spacing_y;

    GStyle( const sf::Color& cbackground
          , const sf::Color& cback_high
          , const sf::Color& cbord
          , const sf::Color& cbord_high
          , const sf::Color& ctext
          , const sf::Color& ctext_high
          , const sf::Font& font
          , float bsize
          , float default_width
          , float spacing_x
          , float spacing_y)
          : cbackground(cbackground)
          , cbackground_highlight(cback_high)
          , cborder(cbord)
          , cborder_highlight(cbord_high)
          , ctext(ctext)
          , ctext_highlight(ctext_high)
          , font(font)
          , border_size(bsize)
          , default_width(default_width)
          , spacing_x(spacing_x)
          , spacing_y(spacing_y) {}
};

enum class CONTROL_STATE {
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

    float get_height() const;
    float get_width() const;
    float get_top() const;
    float get_left() const;
    std::string get_content() const;
    virtual void set_content(const std::string&);
    virtual bool is_valid() { return true; }
protected:
    ControlField( float x
                , float y
                , float letter_size
                , const GStyle* style
                , ControlGrid* grid);
    sf::RectangleShape shape;
    sf::FloatRect bounding_box;
    const GStyle* style;
    ControlGrid* grid;
    CONTROL_STATE state;
    float letter_size;
    sf::Text text;
};
using CFPtr = std::unique_ptr<ControlField>;
#endif