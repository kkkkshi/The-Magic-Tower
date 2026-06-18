#include "item.h"

std::string Potion::name() const {
    switch (type) {
        case PotionType::RH: return "RH";
        case PotionType::BA: return "BA";
        case PotionType::BD: return "BD";
        case PotionType::PH: return "PH";
        case PotionType::WA: return "WA";
        case PotionType::WD: return "WD";
    }
    return "?";
}
