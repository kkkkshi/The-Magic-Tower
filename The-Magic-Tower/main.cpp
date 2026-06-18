#include "game.h"
#include "rng.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>

// Usage: ./cc3k [floorfile] [seed]
//   floorfile - optional layout of all 5 floors (see project spec)
//   seed      - optional unsigned int for reproducible random generation
int main(int argc, char **argv) {
    std::vector<std::string> lines;
    bool useFile = false;
    unsigned seed;
    bool haveSeed = false;

    for (int i = 1; i < argc; ++i) {
        std::ifstream f(argv[i]);
        if (f) {
            std::string line;
            while (std::getline(f, line)) lines.push_back(line);
            useFile = true;
        } else {
            // not a readable file: treat it as a seed
            try {
                seed = static_cast<unsigned>(std::stoul(argv[i]));
                haveSeed = true;
            } catch (...) {
            }
        }
    }

    if (!haveSeed) {
        std::random_device rd;
        seed = rd();
    }
    seedRng(seed);

    Game game;
    if (useFile) game.setFile(lines);
    game.run();
    return 0;
}
