#ifndef PLAYER_H
#define PLAYER_H
#include "character.h"
#include "item.h"
#include <string>

// the player character. each race is a subclass below that only changes the
// starting stats and a couple of hooks (gold value, potion sign, score).
class Player : public Character {
protected:
    int gold = 0;
    bool compass = false;  // only valid for the current floor; reveals the stairs
    bool barrier = false;  // barrier suit, halves damage taken, kept all game
    int atkMod = 0;        // temp atk/def from potions used on this floor
    int defMod = 0;
    std::string race;
public:
    Player(int hp, int atk, int def, const std::string &race)
        : Character(hp, atk, def), race(race) {}

    char symbol() const override { return '@'; }
    int getAtk() const override { int a = ATK + atkMod; return a < 0 ? 0 : a; }
    int getDef() const override { int d = DEF + defMod; return d < 0 ? 0 : d; }

    // call on every new floor: temp potion effects don't carry over
    void newFloorReset() { atkMod = 0; defMod = 0; compass = false; }

    int getGold() const { return gold; }
    virtual void addGold(int v) { gold += v; }
    void loseGold(int v) { gold -= v; if (gold < 0) gold = 0; }

    bool hasCompass() const { return compass; }
    void giveCompass() { compass = true; }
    bool hasBarrier() const { return barrier; }
    void giveBarrier() { barrier = true; }

    const std::string &getRace() const { return race; }
    virtual bool isDwarf() const { return false; }
    virtual double scoreMult() const { return 1.0; }
    int score() const { return static_cast<int>(gold * scoreMult()); }

    // apply a potion, return its code ("BA", "WD"...) for the action line
    virtual std::string usePotion(PotionType t);
};

class Human : public Player {
public:
    Human() : Player(140, 20, 20, "Human") {}
    double scoreMult() const override { return 1.5; }
};

class Dwarf : public Player {
public:
    Dwarf() : Player(100, 20, 30, "Dwarf") {}
    void addGold(int v) override { gold += v * 2; } // worth double for dwarves
    bool isDwarf() const override { return true; }
};

class Elf : public Player {
public:
    Elf() : Player(140, 30, 10, "Elf") {}
    std::string usePotion(PotionType t) override; // bad potions help elves
};

class Orc : public Player {
public:
    Orc() : Player(180, 30, 25, "Orc") {}
    void addGold(int v) override { gold += v / 2; } // worth half for orcs
};
#endif
