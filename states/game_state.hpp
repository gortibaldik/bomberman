#ifndef GAME_STATE_HPP
#define GAME_STATE_HPP

#include "abstract.hpp"
#include "controls/control_grid.hpp"
#include <vector>
#include <SFML/Graphics.hpp>
class GameState: public State {
public:
    void draw(float dt) override;
    void update(float dt) override;
    void handle_input() override;
    
    GameState(WindowManager&);
    GameState(WindowManager&, const sf::View&);
protected:
    sf::View view;
};
#endif