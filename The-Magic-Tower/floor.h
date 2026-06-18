#ifndef FLOOR_H
#define FLOOR_H
#include <vector>
#include <memory>
#include <string>
#include <ostream>
#include "cell.h"
#include "player.h"
#include "enemy.h"
#include "item.h"

// a single level: the cell grid, its chambers, the player, the enemies, stairs
class Floor {
    int rows = 0, cols = 0;
    std::vector<std::vector<Cell>> grid;
    std::vector<std::vector<std::pair<int, int>>> chambers;
    std::shared_ptr<Player> player;
    std::vector<std::shared_ptr<Enemy>> enemies;
    int stairR = -1, stairC = -1;

    void buildTemplate();
    void drawRoom(int top, int left, int bottom, int right);
    void detectChambers();
    bool tileFree(int r, int c) const;            // empty floor tile, not the stairs
    std::pair<int, int> randFreeTile(int chamber); // chamber < 0 = anywhere
    void spawnDragonFor(int gr, int gc);          // drop a dragon next to a hoard/suit
    std::shared_ptr<Enemy> makeEnemy(int roll);

public:
    Floor() {}
    int numRows() const { return rows; }
    int numCols() const { return cols; }
    int numChambers() const { return static_cast<int>(chambers.size()); }

    Cell &at(int r, int c) { return grid[r][c]; }
    const Cell &at(int r, int c) const { return grid[r][c]; }
    bool inBounds(int r, int c) const {
        return r >= 0 && r < rows && c >= 0 && c < cols;
    }

    std::shared_ptr<Player> getPlayer() const { return player; }
    std::vector<std::shared_ptr<Enemy>> &getEnemies() { return enemies; }

    int stairRow() const { return stairR; }
    int stairCol() const { return stairC; }

    // randomly fill the template. needBarrier puts the barrier suit on this floor.
    void generate(std::shared_ptr<Player> p, bool needBarrier);
    // build a floor straight from a layout block (the optional floor file)
    void loadFromLines(const std::vector<std::string> &lines,
                       std::shared_ptr<Player> p);

    char displayChar(int r, int c) const;
    void print(std::ostream &out) const;
};
#endif
