#include "entity.hpp"
#include <cmath>

void go(EntityCoords& coords, EntityDirection direction, float move_factor) {
    switch (direction) {
    case EntityDirection::UP:
        coords.first -= move_factor;
        break;
    case EntityDirection::DOWN:
        coords.first += move_factor;
        break;
    case EntityDirection::RIGHT:
        coords.second += move_factor;
        break;
    case EntityDirection::LEFT:
        coords.second -= move_factor;
        break;
    }
}

/* Naive, because of the assumption, that both c1 and c2 are coordinates
 *  of top left corner of a square with a side of length 1
 * 
 * @return true if the squares intersect
 */ 
bool naive_bbox_intersect(const EntityCoords& c1, const EntityCoords& c2, float intersect_tolerance) {
    bool rows_intersect = ((c1.first >= c2.first) && (c1.first - 1.f + intersect_tolerance <= c2.first)) ||
                          ((c2.first >= c1.first) && (c2.first - 1.f + intersect_tolerance <= c1.first));
    bool cols_intersect = ((c1.second >= c2.second) && (c1.second - 1.f + intersect_tolerance <= c2.second)) ||
                          ((c2.second >= c1.second) && (c2.second - 1.f + intersect_tolerance <= c1.second));
    return rows_intersect && cols_intersect;
}

std::pair<int, int> to_integral(const EntityCoords& coords) {
    std::pair<int, int> result;
    float ceil_row = ceilf(coords.first);
    float ceil_col = ceilf(coords.second);
    if (ceil_row - coords.first <= 0.5f) {
        result.first = ceil_row;
    } else {
        result.first = ceil_row - 1;
    }
    if (ceil_col - coords.second <= 0.5f) {
        result.second = ceil_col;
    } else {
        result.second = ceil_col - 1;
    }
    return result;
}