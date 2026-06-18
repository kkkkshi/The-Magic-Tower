#ifndef GAME_H
#define GAME_H
#include <vector>
#include <string>
#include <memory>
#include "floor.h"
#include "player.h"

// ties it all together: the player, the current floor and the command loop
class Game {
    std::vector<std::string> fileLines; // the 5-floor layout file, if one was given
    bool useFile = false;
    int linesPerFloor = 0;

    std::shared_ptr<Player> player;
    Floor floor;
    int currentFloor = 1; // 1..5
    std::string action;
    bool merchantsHostile = false;
    int barrierFloor = 0; // the floor the barrier suit shows up on
    bool won = false;

    std::shared_ptr<Player> makePlayer(char race);
    void newGame(char race);
    void buildFloor();
    void nextFloor();

    static bool dirDelta(const std::string &d, int &dr, int &dc);
    static int damage(int atk, int def);

    void movePlayer(const std::string &dir);
    void usePotion(const std::string &dir);
    void attack(const std::string &dir);
    void enemyTurn();
    void render() const;

public:
    void setFile(const std::vector<std::string> &lines);
    void run();
};
#endif
