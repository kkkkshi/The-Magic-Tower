#include "game.h"
#include "rng.h"
#include <iostream>
#include <sstream>
#include <cmath>

void Game::setFile(const std::vector<std::string> &lines) {
    fileLines = lines;
    useFile = !lines.empty();
    if (useFile) linesPerFloor = static_cast<int>(lines.size()) / 5;
}

std::shared_ptr<Player> Game::makePlayer(char race) {
    switch (race) {
        case 'h': return std::make_shared<Human>();
        case 'e': return std::make_shared<Elf>();
        case 'd': return std::make_shared<Dwarf>();
        case 'o': return std::make_shared<Orc>();
    }
    return nullptr;
}

void Game::newGame(char race) {
    player = makePlayer(race);
    currentFloor = 1;
    merchantsHostile = false;
    won = false;
    barrierFloor = randRange(1, 5); // pick the one floor that gets the barrier suit
    buildFloor();
    action = "Player character has spawned.";
}

void Game::buildFloor() {
    player->newFloorReset();
    if (useFile) {
        int start = (currentFloor - 1) * linesPerFloor;
        std::vector<std::string> block(fileLines.begin() + start,
                                       fileLines.begin() + start + linesPerFloor);
        floor.loadFromLines(block, player);
    } else {
        floor.generate(player, currentFloor == barrierFloor);
    }
}

void Game::nextFloor() {
    ++currentFloor;
    buildFloor();
    action = "PC descends to floor " + std::to_string(currentFloor) + ".";
}

bool Game::dirDelta(const std::string &d, int &dr, int &dc) {
    if (d == "no")      { dr = -1; dc = 0; }
    else if (d == "so") { dr = 1;  dc = 0; }
    else if (d == "ea") { dr = 0;  dc = 1; }
    else if (d == "we") { dr = 0;  dc = -1; }
    else if (d == "ne") { dr = -1; dc = 1; }
    else if (d == "nw") { dr = -1; dc = -1; }
    else if (d == "se") { dr = 1;  dc = 1; }
    else if (d == "sw") { dr = 1;  dc = -1; }
    else return false;
    return true;
}

// damage = ceil( 100/(100+Def) * Atk ), done with integer math
int Game::damage(int atk, int def) {
    return (100 * atk + (100 + def) - 1) / (100 + def);
}

void Game::movePlayer(const std::string &dir) {
    int dr, dc;
    if (!dirDelta(dir, dr, dc)) { action = "Unknown direction."; return; }
    int pr = player->getR(), pc = player->getC();
    int nr = pr + dr, nc = pc + dc;
    if (!floor.inBounds(nr, nc) || !floor.at(nr, nc).walkable()) {
        action = "PC bumps into a wall."; return;
    }

    // walking onto the stairs drops you a floor, or wins on floor 5
    if (floor.at(nr, nc).stairs) {
        if (currentFloor >= 5) { won = true; return; }
        nextFloor();
        return;
    }

    Cell &dest = floor.at(nr, nc);

    // can't walk into an enemy: you have to attack it
    if (dest.enemy) {
        action = "An enemy is in the way (a " + dir + " to attack).";
        return;
    }

    // a potion or a still-guarded hoard can't be stepped on
    if (dest.item) {
        if (auto g = std::dynamic_pointer_cast<Gold>(dest.item)) {
            if (g->isGuarded()) { action = "A dragon still guards that hoard."; return; }
            player->addGold(g->getValue());
            action = "PC picks up " + std::to_string(g->getValue()) + " gold.";
            dest.item = nullptr;
        } else if (std::dynamic_pointer_cast<Compass>(dest.item)) {
            player->giveCompass();
            action = "PC picks up the compass; the stairs are revealed.";
            dest.item = nullptr;
        } else if (auto b = std::dynamic_pointer_cast<BarrierSuit>(dest.item)) {
            if (b->isGuarded()) { action = "A dragon still guards the barrier suit."; return; }
            player->giveBarrier();
            action = "PC picks up the barrier suit.";
            dest.item = nullptr;
        } else { // a potion: stays put, you stand next to it and use it
            action = "PC sees an unknown potion.";
            return;
        }
    } else {
        action = "PC moves " + dir + ".";
    }

    floor.at(pr, pc).player = nullptr;
    player->setPos(nr, nc);
    dest.player = player;
    enemyTurn();
}

void Game::usePotion(const std::string &dir) {
    int dr, dc;
    if (!dirDelta(dir, dr, dc)) { action = "Unknown direction."; return; }
    int nr = player->getR() + dr, nc = player->getC() + dc;
    if (!floor.inBounds(nr, nc)) { action = "There is no potion there."; return; }
    Cell &cell = floor.at(nr, nc);
    auto pot = std::dynamic_pointer_cast<Potion>(cell.item);
    if (!pot) { action = "There is no potion there."; return; }
    std::string code = player->usePotion(pot->getType());
    cell.item = nullptr;
    action = "PC uses " + code + ".";
    enemyTurn();
}

void Game::attack(const std::string &dir) {
    int dr, dc;
    if (!dirDelta(dir, dr, dc)) { action = "Unknown direction."; return; }
    int nr = player->getR() + dr, nc = player->getC() + dc;
    if (!floor.inBounds(nr, nc) || !floor.at(nr, nc).enemy) {
        action = "There is no enemy there."; return;
    }
    Cell &cell = floor.at(nr, nc);
    auto e = cell.enemy;

    int dmg = damage(player->getAtk(), e->getDef());
    e->changeHP(-dmg);
    std::ostringstream oss;
    oss << "PC deals " << dmg << " damage to " << e->symbol()
        << " (" << e->getHP() << " HP).";

    // hit one merchant and they all come after you
    if (std::dynamic_pointer_cast<Merchant>(e)) merchantsHostile = true;

    if (!e->alive()) {
        // killing the dragon frees whatever it was guarding
        if (auto d = std::dynamic_pointer_cast<Dragon>(e)) {
            if (floor.inBounds(d->guardR(), d->guardC())) {
                Cell &g = floor.at(d->guardR(), d->guardC());
                if (auto gold = std::dynamic_pointer_cast<Gold>(g.item)) gold->unlock();
                if (auto bs = std::dynamic_pointer_cast<BarrierSuit>(g.item)) bs->unlock();
            }
        } else {
            if (e->getGoldDrop() > 0) player->addGold(e->getGoldDrop());
        }
        if (e->hasCompass())
            cell.item = std::make_shared<Compass>(nr, nc);
        cell.enemy = nullptr;
        auto &vec = floor.getEnemies();
        for (size_t i = 0; i < vec.size(); ++i)
            if (vec[i] == e) { vec.erase(vec.begin() + i); break; }
        oss << " " << "It is slain.";
    }
    action = oss.str();
    enemyTurn();
}

void Game::enemyTurn() {
    auto &enemies = floor.getEnemies();
    for (auto &e : enemies) e->setActed(false);

    int pr = player->getR(), pc = player->getC();
    std::ostringstream extra;

    // spec wants enemies handled top-to-bottom, left-to-right
    for (int r = 0; r < floor.numRows(); ++r) {
        for (int c = 0; c < floor.numCols(); ++c) {
            auto e = floor.at(r, c).enemy;
            if (!e || e->didAct()) continue;
            e->setActed(true);
            e->onTurn(); // e.g. troll heals up

            pr = player->getR();
            pc = player->getC();
            bool adjacent = std::abs(e->getR() - pr) <= 1 &&
                            std::abs(e->getC() - pc) <= 1 &&
                            !(e->getR() == pr && e->getC() == pc);

            // work out whether this one wants to fight this turn
            bool hostile = e->isHostile();
            if (std::dynamic_pointer_cast<Merchant>(e))
                hostile = merchantsHostile;
            if (auto d = std::dynamic_pointer_cast<Dragon>(e)) {
                bool nearHoard = std::abs(d->guardR() - pr) <= 1 &&
                                 std::abs(d->guardC() - pc) <= 1;
                hostile = adjacent || nearHoard;
            }

            if (adjacent && hostile) {
                if (randInt(2) == 0) {
                    extra << " " << e->symbol() << " misses PC.";
                } else {
                    int dmg = damage(e->getAtk(), player->getDef());
                    if (player->hasBarrier()) dmg = (dmg + 1) / 2;
                    player->changeHP(-dmg);
                    e->afterHittingPlayer(*player);
                    extra << " " << e->symbol() << " deals " << dmg
                          << " damage to PC.";
                    if (!player->alive()) { action += extra.str(); return; }
                }
            } else if (e->canMove()) {
                // otherwise wander one tile, staying inside the chamber
                std::vector<std::pair<int, int>> opts;
                for (int ddr = -1; ddr <= 1; ++ddr)
                    for (int ddc = -1; ddc <= 1; ++ddc) {
                        if (!ddr && !ddc) continue;
                        int tr = e->getR() + ddr, tc = e->getC() + ddc;
                        if (!floor.inBounds(tr, tc)) continue;
                        Cell &t = floor.at(tr, tc);
                        if (t.base == '.' && !t.stairs && !t.item && !t.enemy &&
                            !t.player && t.chamber == e->getChamber())
                            opts.push_back({tr, tc});
                    }
                if (!opts.empty()) {
                    auto np = opts[randInt((int)opts.size())];
                    floor.at(e->getR(), e->getC()).enemy = nullptr;
                    e->setPos(np.first, np.second);
                    floor.at(np.first, np.second).enemy = e;
                }
            }
        }
    }
    action += extra.str();
}

void Game::render() const {
    floor.print(std::cout);
    std::ostringstream top;
    top << "Race: " << player->getRace() << " Gold: " << player->getGold();
    std::string left = top.str();
    int width = floor.numCols();
    std::string rightStr = "Floor " + std::to_string(currentFloor);
    int pad = width - (int)left.size() - (int)rightStr.size();
    if (pad < 1) pad = 1;
    std::cout << left << std::string(pad, ' ') << rightStr << '\n';
    std::cout << "HP: " << player->getHP() << '\n';
    std::cout << "Atk: " << player->getAtk() << '\n';
    std::cout << "Def: " << player->getDef() << '\n';
    std::cout << "Action: " << action << '\n';
}

void Game::run() {
    std::cout << "ChamberCrawler3000+\n";
    std::cout << "Choose your race - (h)uman (e)lf (d)warf (o)rc, or (q)uit: ";
    bool askRace = true;
    std::string cmd;
    while (std::cin >> cmd) {
        if (askRace) {
            if (cmd == "q") { std::cout << "Goodbye.\n"; return; }
            if (cmd == "h" || cmd == "e" || cmd == "d" || cmd == "o") {
                newGame(cmd[0]);
                askRace = false;
                render();
            } else {
                std::cout << "Please enter h, e, d, o or q: ";
            }
            continue;
        }

        if (cmd == "q") { std::cout << "You abandon the dungeon. Goodbye.\n"; return; }
        if (cmd == "r") {
            askRace = true;
            std::cout << "Restarting. Choose your race "
                         "(h/e/d/o) or q: ";
            continue;
        }
        if (cmd == "a" || cmd == "u") {
            std::string dir;
            if (!(std::cin >> dir)) break;
            if (cmd == "a") attack(dir);
            else usePotion(dir);
        } else {
            movePlayer(cmd);
        }

        // end-of-game checks
        if (!player->alive()) {
            render();
            std::cout << "\nYou have died. Score: " << player->score() << "\n";
            std::cout << "Play again? Choose a race (h/e/d/o) or q: ";
            askRace = true;
            continue;
        }
        if (won) {
            render();
            std::cout << "\nYou cleared the dungeon! Score: " << player->score()
                      << "\n";
            std::cout << "Play again? Choose a race (h/e/d/o) or q: ";
            askRace = true;
            continue;
        }
        render();
    }
}
