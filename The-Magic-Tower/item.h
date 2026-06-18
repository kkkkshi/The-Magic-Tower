#ifndef ITEM_H
#define ITEM_H
#include <string>

// anything lying on a floor tile: potions, gold, the compass, the barrier suit
class Item {
protected:
    int r, c;
public:
    Item(int rr, int cc) : r(rr), c(cc) {}
    virtual ~Item() {}
    int getR() const { return r; }
    int getC() const { return c; }
    virtual char symbol() const = 0;
    virtual std::string name() const = 0;
};

enum class PotionType { RH, BA, BD, PH, WA, WD };

class Potion : public Item {
    PotionType type;
public:
    Potion(int r, int c, PotionType t) : Item(r, c), type(t) {}
    char symbol() const override { return 'P'; }
    std::string name() const override;
    PotionType getType() const { return type; }
};

class Gold : public Item {
    int value;
    bool guarded; // dragon hoard: locked until the dragon is dead
public:
    Gold(int r, int c, int v, bool g = false) : Item(r, c), value(v), guarded(g) {}
    char symbol() const override { return 'G'; }
    std::string name() const override { return "gold"; }
    int getValue() const { return value; }
    bool isGuarded() const { return guarded; }
    void unlock() { guarded = false; }
};

class Compass : public Item {
public:
    Compass(int r, int c) : Item(r, c) {}
    char symbol() const override { return 'C'; }
    std::string name() const override { return "compass"; }
};

class BarrierSuit : public Item {
    bool guarded;
public:
    BarrierSuit(int r, int c, bool g = true) : Item(r, c), guarded(g) {}
    char symbol() const override { return 'B'; }
    std::string name() const override { return "barrier suit"; }
    bool isGuarded() const { return guarded; }
    void unlock() { guarded = false; }
};
#endif
