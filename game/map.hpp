#ifndef GAME_MAP_HPP
#define GAME_MAP_HPP

#include <string>
#include <utility>

class GameMap {
public:
    void load_from_config(const std::string&);
    virtual void process_loaded(const std::string& token, const std::string& animation,
                                const std::string& type, int row, int column) = 0;
    virtual void initialize() = 0;
    int transform_to_int(const std::pair<int, int>&) const;
    std::pair<int, int> transform_to_coords(int) const;
    const std::string& get_name() const { return name; }
    int get_rows() const { return rows; }
    int get_columns() const { return columns; }

    static void generate_config(const std::string&, int, int);
protected:
    int rows = 0, columns = 0;
    std::string name;
};

#endif