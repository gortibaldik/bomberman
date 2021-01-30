#include "text_field.hpp"
#include "control_grid.hpp"

// shouldn't be here, should be just inherited from control_field
void TextField::move_pos(float factor, unsigned int new_x, unsigned int new_y) {
    letter_width *= factor;

    text.setCharacterSize(letter_width);
    int text_size = text.getString().getSize();
    text.setPosition(new_x, new_y);

    auto&& fr = text.getGlobalBounds();
    shape.setSize(sf::Vector2f(fr.width, fr.height));
    shape.setPosition(fr.left, fr.top);
}

TextField::TextField(   float x,
                        float y,
                        float letter_width,
                        float default_width,
                        unsigned int max_length,
                        GStyle* gstyle,
                        ControlGrid* grid):  style(gstyle),
                                            text("", *style->font),
                                            state(IDLE),
                                            letter_width(letter_width),
                                            grid(grid) {
    
    this->text.setFillColor(style->ctext);
    this->text.setPosition(x, y);
    this->text.setCharacterSize(this->letter_width);
    
    auto&& fr = this->text.getGlobalBounds();

    shape.setFillColor(style->cbackground);
    shape.setPosition(fr.left, fr.top);
    shape.setSize(sf::Vector2f(fr.width, fr.height));
    shape.setOutlineThickness(style->border_size);
    shape.setOutlineColor(style->cborder);
}

float TextField::get_height() const {
    return shape.getGlobalBounds().height;
}

std::string TextField::get_text() const {
    return text.getString().toAnsiString();
}

void TextField::render(sf::RenderTarget* target) {
    target->draw(shape);
    target->draw(text);
}

void TextField::handle_input(const sf::Vector2f& mouse_position, const sf::Event& e) {
    state = IDLE;
    if (shape.getGlobalBounds().contains(mouse_position)) {
        state = HOVER;
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            state = PRESSED;
            //grid->which_pressed = this;
        }
    }
}

void TextField::update() {
    switch(state) {
    case IDLE:
        text.setFillColor(style->ctext);
        shape.setFillColor(style->cbackground);
        shape.setOutlineColor(style->cborder);
        break;
    case HOVER:
        text.setFillColor(style->ctext_highlight);
        shape.setFillColor(style->cbackground_highlight);
        shape.setOutlineColor(style->cborder_highlight);
        break;
    }
}