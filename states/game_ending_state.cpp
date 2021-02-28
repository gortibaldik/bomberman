#include "game_ending_state.hpp"

enum class BTN: int {
    RESTART, MM_RETURN, QUIT, LEADERBOARD, BLANK
};

static const std::unordered_map<std::string, BTN> str_to_btn = { 
    {"Quit", BTN::QUIT}, {"Restart game", BTN::RESTART},
    {"Return to main menu", BTN::MM_RETURN}, {"Leaderboard:", BTN::LEADERBOARD},
    {"", BTN::BLANK}
};
static std::unordered_map<BTN, std::string> btn_to_str;
static void initialize_maps() {
    for (auto&& p : str_to_btn) {
        btn_to_str.emplace(p.second, p.first);
    }
}

void GameEndingState::update(float) {
    if (client == nullptr) {
        window_manager.pop_states(2);
        return;
    }
    if (leaderboard_initialized == false) {
        auto&& ps = client->get_players_scores();
        if (ps.size() != 0) {
            initialize_leaderboard(ps);
        }
    }
    menu.update();
}

void GameEndingState::handle_btn_pressed() {
    if (pressed && (str_to_btn.find(pressed->get_content()) != str_to_btn.end())) {
        switch (str_to_btn.at(pressed->get_content())) {
        case BTN::MM_RETURN:
            window_manager.pop_states(2);
            break;
        case BTN::QUIT:
            window_manager.window.close();
            break;
        }
    }
}

void GameEndingState::initialize_leaderboard(const std::vector<std::string>& scores) {
    leaderboard_initialized = true;
    menu.add_non_clickable(btn_to_str.at(BTN::BLANK));
    menu.add_non_clickable(btn_to_str.at(BTN::LEADERBOARD));
    for (auto&& score : scores) {
        menu.add_non_clickable("\t" + score);
    }
}

GameEndingState::GameEndingState( WindowManager& mngr
                                , const sf::View& view
                                , GameClient* client)
                                : MenuState(mngr, view, "start_menu")
                                , client(client)
                                , leaderboard_initialized(false) {
    initialize_maps();
    menu.add_button(btn_to_str.at(BTN::RESTART));
    menu.add_button(btn_to_str.at(BTN::MM_RETURN));
    menu.add_button(btn_to_str.at(BTN::QUIT));
    if (client == nullptr) { return; }
    auto&& ps = client->get_players_scores();
    if (ps.size() != 0) {
        initialize_leaderboard(ps);
    }
}