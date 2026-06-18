#ifndef ENEMY_H
#define ENEMY_H
#include "character.h"

class Player;

// enemy base class. the per-type quirks (regen, stealing, stationary dragons)
// live in the subclasses further down.
class Enemy : public Character {
protected:
    char sym;
    bool hostile;
    int goldDrop;     // gold dropped to the PC on death
    bool movable;
    bool holdsCompass = false;
    int chamber = -1; // enemies never leave their chamber
    bool acted = false; // already took its turn this round
public:
    Enemy(int hp, int atk, int def, char s, int gold = 1,
          bool host = true, bool mov = true)
        : Character(hp, atk, def), sym(s), hostile(host),
          goldDrop(gold), movable(mov) {}
    char symbol() const override { return sym; }

    bool isHostile() const { return hostile; }
    void setHostile(bool h) { hostile = h; }
    bool canMove() const { return movable; }
    int getGoldDrop() const { return goldDrop; }

    bool hasCompass() const { return holdsCompass; }
    void giveCompass() { holdsCompass = true; }

    int getChamber() const { return chamber; }
    void setChamber(int x) { chamber = x; }
    bool didAct() const { return acted; }
    void setActed(bool b) { acted = b; }

    virtual void onTurn() {}                     // troll uses this to regen
    virtual void afterHittingPlayer(Player &) {} // vampire/goblin steal here
    virtual ~Enemy() {}
};

class Vampire : public Enemy {
public:
    Vampire() : Enemy(50, 25, 25, 'V') {}
    void afterHittingPlayer(Player &p) override; // drains 5 HP, dwarves hurt it
};

class Werewolf : public Enemy {
public:
    Werewolf() : Enemy(120, 30, 5, 'W') {}
};

class Troll : public Enemy {
public:
    Troll() : Enemy(120, 25, 15, 'T') {}
    void onTurn() override { changeHP(5); } // heals 5 each round
};

class Goblin : public Enemy {
public:
    Goblin() : Enemy(70, 5, 10, 'N') {}
    void afterHittingPlayer(Player &p) override; // swipes 5 gold
};

class Merchant : public Enemy {
public:
    // peaceful until attacked, then all merchants turn on you; drops a hoard of 4
    Merchant() : Enemy(30, 70, 5, 'M', 4, false) {}
};

class Dragon : public Enemy {
    int gR = -1, gC = -1; // the hoard / barrier suit cell it sits next to
public:
    // never moves, drops no gold, only fights when you get near its hoard
    Dragon() : Enemy(150, 20, 20, 'D', 0, false, false) {}
    void setGuard(int r, int c) { gR = r; gC = c; }
    int guardR() const { return gR; }
    int guardC() const { return gC; }
};

class Phoenix : public Enemy {
public:
    Phoenix() : Enemy(50, 35, 20, 'X') {}
};
#endif
