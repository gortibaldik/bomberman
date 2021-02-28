#include "game_starting_state.hpp"
#include "game_state.hpp"
#include "network/network_params.hpp"
#include "window_manager/def.hpp"
#include <SFML/Network.hpp>

enum class BTN: int {
    START_GAME, START_TIME, BLANK
};

static const std::unordered_map<std::string, BTN> str_to_btn = { 
    {"Starting game", BTN::START_GAME}, {"START_TIME", BTN::START_TIME},
    {"", BTN::BLANK}
};
static std::unordered_map<BTN, std::string> btn_to_str;
static void initialize_maps() {
    for (auto&& str_btn : str_to_btn) {
        btn_to_str.emplace(str_btn.second, str_btn.first);
    }
}

void GameStartingState::handle_btn_pressed() {}

void GameStartingState::update(float) {
    if (connection_timer.getElapsedTime().asMilliseconds() >= Network::ConnectionInterval) {
        server.start_game();
    } else {
        int dt = Network::ConnectionInterval - connection_timer.getElapsedTime().asMilliseconds();
            menu.get_named_field(btn_to_str[BTN::START_TIME])
                                    ->set_content("Starting in " + std::to_string(dt / 1000) + " seconds.");
    }
    if (client.is_game_started()) {
        window_manager.push_state(std::make_unique<GameState>(window_manager, view, &client));
        return;
    }
}

GameStartingState::GameStartingState( WindowManager& mngr
                                    , const sf::View& view)
                                    : MenuState(mngr, view, "start_menu")
                                    , client( "Player"
                                            , mngr.get_tm()
                                            , mngr.get_tm().get_font("game_font")
                                            , true)
                                    , server("media/map_basic.cfg") {
    initialize_maps();
    menu.add_non_clickable(btn_to_str.at(BTN::START_GAME));
    menu.add_non_clickable(btn_to_str.at(BTN::START_TIME), btn_to_str.at(BTN::BLANK));
    auto in_port = server.start(0);
    if (in_port == -1) {
        throw std::runtime_error("GAME_STARTING_STATE -- couldn't create server for game logic");
    }
    auto ip_address = sf::IpAddress::LocalHost;
    if (!client.connect(ip_address, in_port)) {
        throw std::runtime_error("GAME_STARTING_STATE -- couldn't connect to game logic");
    }
    server.set_ready_game();
}