#include "floor.h"
#include "rng.h"
#include <queue>

// --- map template ---

void Floor::drawRoom(int top, int left, int bottom, int right) {
    for (int c = left; c <= right; ++c) {
        grid[top][c].base = '-';
        grid[bottom][c].base = '-';
    }
    for (int r = top; r <= bottom; ++r) {
        grid[r][left].base = '|';
        grid[r][right].base = '|';
    }
    for (int r = top + 1; r < bottom; ++r)
        for (int c = left + 1; c < right; ++c)
            grid[r][c].base = '.';
}

// the fixed 5-room layout: two rooms across the top, two across the bottom,
// one in the middle, all joined by a vertical passage that runs between a top
// and a bottom corridor. same map every floor, only the contents change.
void Floor::buildTemplate() {
    rows = 25;
    cols = 79;
    grid.assign(rows, std::vector<Cell>(cols));
    for (int r = 0; r < rows; ++r)
        for (int c = 0; c < cols; ++c)
            grid[r][c] = Cell(r, c, ' ');

    drawRoom(1, 1, 6, 24);    // chamber: top-left
    drawRoom(1, 54, 6, 77);   // top-right
    drawRoom(9, 28, 15, 50);  // centre
    drawRoom(17, 1, 23, 24);  // bottom-left
    drawRoom(17, 54, 23, 77); // bottom-right

    // passages (#)
    for (int c = 25; c <= 53; ++c) grid[3][c].base = '#';   // top corridor
    for (int c = 25; c <= 53; ++c) grid[20][c].base = '#';  // bottom corridor
    for (int r = 4; r <= 8; ++r)  grid[r][39].base = '#';   // spine upper
    for (int r = 16; r <= 19; ++r) grid[r][39].base = '#';  // spine lower

    // doorways (+)
    grid[3][24].base = '+';  grid[3][54].base = '+';
    grid[9][39].base = '+';  grid[15][39].base = '+';
    grid[20][24].base = '+'; grid[20][54].base = '+';
}

void Floor::detectChambers() {
    chambers.clear();
    std::vector<std::vector<bool>> seen(rows, std::vector<bool>(cols, false));
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            if (grid[r][c].base != '.' || seen[r][c]) continue;
            int id = static_cast<int>(chambers.size());
            std::vector<std::pair<int, int>> tiles;
            std::queue<std::pair<int, int>> q;
            q.push({r, c});
            seen[r][c] = true;
            const int dr[] = {-1, 1, 0, 0};
            const int dc[] = {0, 0, -1, 1};
            while (!q.empty()) {
                auto [cr, cc] = q.front();
                q.pop();
                grid[cr][cc].chamber = id;
                tiles.push_back({cr, cc});
                for (int k = 0; k < 4; ++k) {
                    int nr = cr + dr[k], nc = cc + dc[k];
                    if (inBounds(nr, nc) && !seen[nr][nc] &&
                        grid[nr][nc].base == '.') {
                        seen[nr][nc] = true;
                        q.push({nr, nc});
                    }
                }
            }
            chambers.push_back(tiles);
        }
    }
}

// --- spawn helpers ---

bool Floor::tileFree(int r, int c) const {
    if (!inBounds(r, c)) return false;
    const Cell &cell = grid[r][c];
    return cell.base == '.' && !cell.stairs && !cell.item && !cell.enemy &&
           !cell.player;
}

std::pair<int, int> Floor::randFreeTile(int chamber) {
    std::vector<std::pair<int, int>> cand;
    for (int id = 0; id < numChambers(); ++id) {
        if (chamber >= 0 && id != chamber) continue;
        for (auto &t : chambers[id])
            if (tileFree(t.first, t.second)) cand.push_back(t);
    }
    if (cand.empty()) return {-1, -1};
    return cand[randInt(static_cast<int>(cand.size()))];
}

void Floor::spawnDragonFor(int gr, int gc) {
    int ch = grid[gr][gc].chamber;
    // prefer a tile adjacent to the hoard, else any free tile in the chamber
    std::vector<std::pair<int, int>> adj;
    for (int dr = -1; dr <= 1; ++dr)
        for (int dc = -1; dc <= 1; ++dc) {
            if (!dr && !dc) continue;
            int nr = gr + dr, nc = gc + dc;
            if (tileFree(nr, nc) && grid[nr][nc].chamber == ch)
                adj.push_back({nr, nc});
        }
    std::pair<int, int> pos =
        adj.empty() ? randFreeTile(ch) : adj[randInt((int)adj.size())];
    if (pos.first < 0) return;
    auto d = std::make_shared<Dragon>();
    d->setPos(pos.first, pos.second);
    d->setChamber(ch);
    d->setGuard(gr, gc);
    grid[pos.first][pos.second].enemy = d;
    enemies.push_back(d);
}

std::shared_ptr<Enemy> Floor::makeEnemy(int roll) {
    // distribution out of 18: W 4, V 3, N 5, T 2, X 2, M 2
    if (roll < 4)  return std::make_shared<Werewolf>();
    if (roll < 7)  return std::make_shared<Vampire>();
    if (roll < 12) return std::make_shared<Goblin>();
    if (roll < 14) return std::make_shared<Troll>();
    if (roll < 16) return std::make_shared<Phoenix>();
    return std::make_shared<Merchant>();
}

// --- random floors ---

void Floor::generate(std::shared_ptr<Player> p, bool needBarrier) {
    enemies.clear();
    stairR = stairC = -1;
    buildTemplate();
    detectChambers();
    player = p;

    // 1. player
    int pcCh = randInt(numChambers());
    auto pcPos = randFreeTile(pcCh);
    player->setPos(pcPos.first, pcPos.second);
    grid[pcPos.first][pcPos.second].player = player;

    // 2. stairs (in a different chamber)
    int stCh;
    do { stCh = randInt(numChambers()); } while (stCh == pcCh);
    auto stPos = randFreeTile(stCh);
    stairR = stPos.first;
    stairC = stPos.second;
    grid[stairR][stairC].stairs = true;

    // 3. potions (10)
    for (int i = 0; i < 10; ++i) {
        auto pos = randFreeTile(-1);
        if (pos.first < 0) break;
        PotionType t = static_cast<PotionType>(randInt(6));
        grid[pos.first][pos.second].item =
            std::make_shared<Potion>(pos.first, pos.second, t);
    }

    // 4. gold (10): 5/8 normal(1), 2/8 small hoard(2), 1/8 dragon hoard(6)
    for (int i = 0; i < 10; ++i) {
        auto pos = randFreeTile(-1);
        if (pos.first < 0) break;
        int roll = randInt(8);
        if (roll < 5) {
            grid[pos.first][pos.second].item =
                std::make_shared<Gold>(pos.first, pos.second, 1);
        } else if (roll < 7) {
            grid[pos.first][pos.second].item =
                std::make_shared<Gold>(pos.first, pos.second, 2);
        } else {
            grid[pos.first][pos.second].item =
                std::make_shared<Gold>(pos.first, pos.second, 6, true);
            spawnDragonFor(pos.first, pos.second);
        }
    }

    // 5. barrier suit (once per game), guarded by a dragon
    if (needBarrier) {
        auto pos = randFreeTile(-1);
        if (pos.first >= 0) {
            grid[pos.first][pos.second].item =
                std::make_shared<BarrierSuit>(pos.first, pos.second, true);
            spawnDragonFor(pos.first, pos.second);
        }
    }

    // 6. enemies (20, dragons excluded from this roll)
    for (int i = 0; i < 20; ++i) {
        auto pos = randFreeTile(-1);
        if (pos.first < 0) break;
        auto e = makeEnemy(randInt(18));
        e->setPos(pos.first, pos.second);
        e->setChamber(grid[pos.first][pos.second].chamber);
        grid[pos.first][pos.second].enemy = e;
        enemies.push_back(e);
    }

    // 7. compass: one (non-dragon) enemy holds it
    std::vector<std::shared_ptr<Enemy>> carriers;
    for (auto &e : enemies)
        if (!std::dynamic_pointer_cast<Dragon>(e)) carriers.push_back(e);
    if (!carriers.empty())
        carriers[randInt((int)carriers.size())]->giveCompass();
}

// --- floors read from a file ---

void Floor::loadFromLines(const std::vector<std::string> &lines,
                          std::shared_ptr<Player> p) {
    enemies.clear();
    stairR = stairC = -1;
    rows = static_cast<int>(lines.size());
    cols = 0;
    for (auto &l : lines) cols = std::max(cols, (int)l.size());
    grid.assign(rows, std::vector<Cell>(cols));

    int pcR = -1, pcC = -1;
    std::vector<std::pair<int, int>> dragonHoards; // need a dragon spawned next to

    for (int r = 0; r < rows; ++r) {
        const std::string &line = lines[r];
        for (int c = 0; c < cols; ++c) {
            char ch = c < (int)line.size() ? line[c] : ' ';
            grid[r][c] = Cell(r, c, ' ');
            switch (ch) {
                case '|': case '-': case '+': case '#': case '.': case ' ':
                    grid[r][c].base = (ch == 0) ? ' ' : ch;
                    break;
                case '@':
                    grid[r][c].base = '.'; pcR = r; pcC = c; break;
                case '\\':
                    grid[r][c].base = '.'; grid[r][c].stairs = true;
                    stairR = r; stairC = c; break;
                case '0': case '1': case '2': case '3': case '4': case '5': {
                    grid[r][c].base = '.';
                    grid[r][c].item = std::make_shared<Potion>(
                        r, c, static_cast<PotionType>(ch - '0'));
                    break;
                }
                case '6': grid[r][c].base = '.';
                    grid[r][c].item = std::make_shared<Gold>(r, c, 1); break;
                case '7': grid[r][c].base = '.';
                    grid[r][c].item = std::make_shared<Gold>(r, c, 2); break;
                case '8': grid[r][c].base = '.';
                    grid[r][c].item = std::make_shared<Gold>(r, c, 4); break;
                case '9': grid[r][c].base = '.';
                    grid[r][c].item = std::make_shared<Gold>(r, c, 6, true);
                    dragonHoards.push_back({r, c}); break;
                case 'C': grid[r][c].base = '.';
                    grid[r][c].item = std::make_shared<Compass>(r, c); break;
                case 'B': grid[r][c].base = '.';
                    grid[r][c].item = std::make_shared<BarrierSuit>(r, c, false);
                    break;
                case 'V': case 'W': case 'T': case 'X':
                case 'N': case 'M': case 'D': {
                    grid[r][c].base = '.';
                    std::shared_ptr<Enemy> e;
                    if (ch == 'V') e = std::make_shared<Vampire>();
                    else if (ch == 'W') e = std::make_shared<Werewolf>();
                    else if (ch == 'T') e = std::make_shared<Troll>();
                    else if (ch == 'X') e = std::make_shared<Phoenix>();
                    else if (ch == 'N') e = std::make_shared<Goblin>();
                    else if (ch == 'M') e = std::make_shared<Merchant>();
                    else e = std::make_shared<Dragon>();
                    e->setPos(r, c);
                    grid[r][c].enemy = e;
                    enemies.push_back(e);
                    break;
                }
                default:
                    grid[r][c].base = ' ';
            }
        }
    }

    detectChambers();
    for (auto &e : enemies)
        e->setChamber(grid[e->getR()][e->getC()].chamber);
    for (auto &h : dragonHoards) spawnDragonFor(h.first, h.second);

    player = p;
    if (pcR >= 0) {
        player->setPos(pcR, pcC);
        grid[pcR][pcC].player = player;
    }

    // give the compass to an enemy if the file did not place one
    bool hasCompass = false;
    for (int r = 0; r < rows; ++r)
        for (int c = 0; c < cols; ++c)
            if (grid[r][c].item &&
                std::dynamic_pointer_cast<Compass>(grid[r][c].item))
                hasCompass = true;
    if (!hasCompass) {
        std::vector<std::shared_ptr<Enemy>> carriers;
        for (auto &e : enemies)
            if (!std::dynamic_pointer_cast<Dragon>(e)) carriers.push_back(e);
        if (!carriers.empty())
            carriers[randInt((int)carriers.size())]->giveCompass();
    }
}

// --- drawing ---

char Floor::displayChar(int r, int c) const {
    const Cell &cell = grid[r][c];
    if (cell.player) return '@';
    if (cell.enemy) return cell.enemy->symbol();
    if (cell.item) return cell.item->symbol();
    if (cell.stairs) return (player && player->hasCompass()) ? '\\' : '.';
    return cell.base;
}

void Floor::print(std::ostream &out) const {
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) out << displayChar(r, c);
        out << '\n';
    }
}
