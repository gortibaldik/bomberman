#ifndef GAME_MAP_HPP
#define GAME_MAP_HPP

#include <string>

class GameMap {
public:
    void load_from_config(const std::string&);
    virtual void process_loaded(const std::string& token, const std::string& animation,
                                const std::string& type, int row, int column) = 0;
    virtual void initialize() = 0;

    static void generate_config(const std::string&, int, int);
protected:
    int rows, columns;
};

#endif