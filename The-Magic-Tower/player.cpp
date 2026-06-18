#include "player.h"

std::string Player::usePotion(PotionType t) {
    switch (t) {
        case PotionType::RH: changeHP(10);  return "RH";
        case PotionType::BA: atkMod += 5;   return "BA";
        case PotionType::BD: defMod += 5;   return "BD";
        case PotionType::PH: changeHP(-10); return "PH";
        case PotionType::WA: atkMod -= 5;   return "WA";
        case PotionType::WD: defMod -= 5;   return "WD";
    }
    return "?";
}

// elves read the three negative potions as their positive versions
std::string Elf::usePotion(PotionType t) {
    switch (t) {
        case PotionType::PH: changeHP(10); return "PH";
        case PotionType::WA: atkMod += 5;  return "WA";
        case PotionType::WD: defMod += 5;  return "WD";
        default: return Player::usePotion(t);
    }
}
