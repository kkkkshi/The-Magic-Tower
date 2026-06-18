#include "enemy.h"
#include "player.h"

void Vampire::afterHittingPlayer(Player &p) {
    if (p.isDwarf())
        changeHP(-5); // dwarf blood backfires on the vampire
    else
        changeHP(5);  // otherwise it drains 5 HP off the hit
}

void Goblin::afterHittingPlayer(Player &p) {
    p.loseGold(5); // takes 5 gold whenever it lands a hit
}
