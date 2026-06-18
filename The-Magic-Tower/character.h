#ifndef CHARACTER_H
#define CHARACTER_H

// base class shared by the player and every enemy
class Character {
protected:
    int maxHP, HP, ATK, DEF;
    int r = 0, c = 0; // position: row, column
public:
    Character(int hp, int atk, int def)
        : maxHP(hp), HP(hp), ATK(atk), DEF(def) {}
    virtual ~Character() {}

    int getHP() const { return HP; }
    int getMaxHP() const { return maxHP; }
    void setHP(int h) {
        HP = h;
        if (HP > maxHP) HP = maxHP;
        if (HP < 0) HP = 0;
    }
    void changeHP(int d) { setHP(HP + d); }
    bool alive() const { return HP > 0; }

    virtual int getAtk() const { return ATK; }
    virtual int getDef() const { return DEF; }
    void setAtk(int a) { ATK = a; }
    void setDef(int d) { DEF = d; }

    int getR() const { return r; }
    int getC() const { return c; }
    void setPos(int rr, int cc) { r = rr; c = cc; }

    virtual char symbol() const = 0;
};
#endif
