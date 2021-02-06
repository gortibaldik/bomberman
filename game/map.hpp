#include <vector>
#include <SFML/Graphics.hpp>
#include <texture_handling/texture_manager.hpp>

using TilesMap = std::vector<AnimObject>;
using Entities = std::vector<AnimObject>;

class GameMap {
public:
    GameMap(const std::string&, const TextureManager& tm);
    void load_from_config(const std::string&, const TextureManager&);
    AnimObject& get(int row, int column);
    void render(sf::RenderTarget* rt);
    void fit_to_window(float, float);

    static void generate_config(const std::string&, int, int);
private:
    AnimObject& unsafe_get(int row, int column);
    int rows, columns;
    float tile_width, tile_height;
    float tile_scale_x, tile_scale_y;
    TilesMap tiles;
};