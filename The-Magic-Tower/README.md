# ChamberCrawler3000+

A terminal rogue-like for CS246. You move an `@` down through five floors of
chambers, fight whatever is in the way, grab gold and potions, and try to reach
the stairs on floor 5 without dying.

## Build

```
make
```

This produces an executable called `cc3k`. `make clean` removes the build files.

## Run

```
./cc3k                 # normal game, floors generated randomly
./cc3k floor.txt       # use a fixed layout for all five floors
./cc3k floor.txt 1234  # fixed layout + fixed seed
./cc3k 1234            # random floors, but reproducible with this seed
```

Arguments are figured out by type: an argument that names a readable file is
treated as a layout file, anything else is treated as the random seed.

## How to play

When the game starts you pick a race:

- `h` human (140 HP, 20 Atk, 20 Def, score +50%)
- `e` elf (140 HP, 30 Atk, 10 Def, the harmful potions help you instead)
- `d` dwarf (100 HP, 20 Atk, 30 Def, gold counts double)
- `o` orc (180 HP, 30 Atk, 25 Def, gold counts half)

Then, each turn, type one command:

- `no so ea we ne nw se sw`: move one square in that direction
- `u <dir>`: drink the potion one square away (e.g. `u ea`)
- `a <dir>`: attack the enemy one square away (e.g. `a ne`)
- `r`: restart (pick a race again)
- `q`: quit

The board is redrawn after every command. Gold is collected just by stepping on
it. Potions can't be walked over; you stand next to one and use it. You don't
find out what a potion does until you drink it.

The stairs only show up as `\` once you've grabbed that floor's compass, which
drops from one of the enemies when it dies. Stepping on the stairs takes you
down (and on floor 5, wins the game).

## Map symbols

```
@  player          V W T N X  enemies (vampire, werewolf, troll, goblin, phoenix)
.  floor tile      M          merchant (leaves you alone until you hit one)
# + walls/doors    D          dragon (guards a hoard, won't move)
P  potion          G          gold
C  compass         B          barrier suit (halves the damage you take)
\  stairs down
```

## Floor file format

A layout file holds all five floors, one after another, each the same height.
Walls/tiles are written literally (`| - + # .` and spaces). Potions and gold
are written as digits so the file doesn't give away what a potion is:

```
0 RH   1 BA   2 BD   3 PH   4 WA   5 WD      (potions)
6 gold(1)  7 small hoard(2)  8 merchant(4)  9 dragon hoard(6)
```

Enemies use their map letter, `@` marks the player start and `\` the stairs.

## Source layout

- `character.h`: base class for the player and enemies
- `player.*`: the player and the four races
- `enemy.*`: the enemy base class and the seven enemy types
- `item.*`: potions, gold, compass, barrier suit
- `cell.h`, `floor.*`: the map: cells, chambers, generation, enemy movement, drawing
- `game.*`: combat, the turn loop, win/lose handling
- `main.cpp`: argument parsing and start-up
- `rng.h`: the shared random number generator

## Notes

- The damage formula is `ceil(100 / (100 + Def) * Atk)`; the barrier suit halves
  what the player takes. The player always attacks first and never misses;
  enemies miss half the time.
- Temporary potions (BA/BD/WA/WD) only last the floor you used them on. RH and PH
  change HP permanently.
- The built-in map is our own five-chamber layout. Chambers are found by a flood
  fill over the floor tiles, so a hand-written layout file works the same way.
