#ifndef RNG_H
#define RNG_H
#include <random>

// one global engine, seeded once in main so a given seed replays the same game
inline std::mt19937 &engine() {
    static std::mt19937 e;
    return e;
}
inline void seedRng(unsigned s) { engine().seed(s); }

// random int in [0, n)
inline int randInt(int n) {
    std::uniform_int_distribution<int> d(0, n - 1);
    return d(engine());
}
// random int in [lo, hi]
inline int randRange(int lo, int hi) {
    std::uniform_int_distribution<int> d(lo, hi);
    return d(engine());
}
#endif
