#ifndef CELL_H
#define CELL_H
#include <memory>

class Player;
class Enemy;
class Item;

// one square of the map. base is the static glyph; the pointers are whatever
// happens to be standing on it.
struct Cell {
    int r = 0, c = 0;
    char base = ' ';   // | - + # . or space
    int chamber = -1;  // which chamber this tile belongs to, -1 if not a floor tile
    bool stairs = false;
    std::shared_ptr<Item> item;
    std::shared_ptr<Enemy> enemy;
    std::shared_ptr<Player> player;

    Cell() {}
    Cell(int rr, int cc, char b) : r(rr), c(cc), base(b) {}

    bool walkable() const { return base == '.' || base == '+' || base == '#'; }
    bool floorTile() const { return base == '.'; }
};
#endif
