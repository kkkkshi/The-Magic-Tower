#include <sstream>
#include <ostream>

void Game::RestartGame()
        {
            char type;
            int r = rand(5);
            currentFloor = 0;
            floors.clear();
            for(int i = 0;i < 5;i++)
            {
                floor = make_shared<Floor>();
                std::cout<< "input Player type"<<endl;
                std::cin>>type;
                floor->generateFloor(type);
                if( i == 0)
                {
                    currentFloor = floor;
                }
                if(i == r)
                {
                    floor->setBarrierSuit(); 
                }
                floors.push_back(floor);
            }
        }
    void Game::flush()
    {
        std::vector<std::shared_ptr<Enemy>> enemies = Floor.getEnemies();

        for(auto enemy:enemies)
        {
            int rx = rand(4);
            int ry = rand(4);
            std::shared_ptr<Dragon> d = dynamic_cast<Dragon>(enemy);
            if(d != NULL) continue;
            enemy->move(rx);
            enemy->move(ry);
        }
    }

    void Game::startNewGame()
    {
        RestartGame();
        action = "Player character has swaped";
    }

    void Game::GameOver()
    {
        floors.clear();
        currentFloor = 0;
        action = nullptr;
    }

    void Game::setCurrentFloor(int current)
    {
        this.currentFloor = current;
    }

    std::shared_ptr<Floor> getCurrentFloor()
    {
        return this.currentFloor;
    }

    void Game::MovePlayer(string cmd)
    {
        std::shared_ptr<Player> player = floors[currentFloor]->getPlayer();
        int x,y;
        x = player->getX();
        y = player->getY();
        if (cmd == "no") {
            driection = "North";
            y++;
        } else if (cmd == "so") {
            driection = "South";
            y--;
        } else if (cmd == "ea") {
            driection = "East";
            x++;
        } else if (cmd == "we") {
            driection = "Western";
            x--;
        } else if (cmd == "ne") {
            driection = "Northeast";
            y++;
            x++;
        } else if (cmd == "nw") {
            driection = "Northwest";
            y++;
            x--;
        } else if (cmd == "se") {
            driection = "Southeast";
            y--;
            x++;
        } else if (cmd == "sw") {
            driection = "Southwest";
            y--;
            x--;
        }
        player->setX(x);
        player->setY(y);
        action = "PC move " + driection + " and sees an unknown potion.";
    }

    void Game::PickUp(string cmd)
    {
        ostringstream oss;
        std::shared_ptr<Player> player = floors[currentFloor]->getPlayer();
        int x,y;
        x = player->getX();
        y = player->getY();
        if (cmd == "u<we>") {
            x--;
        } else if (cmd == "u<ne>") {
            y++;
            x++;
        } else if (cmd == "u<nw>") {
            y++;
            x--;
        } else if (cmd == "u<se>") {
            y--;
            x++;
        } else if (cmd == "u<sw>") {
            y--;
            x--;
        }
        std::shared_ptr<Cell> cell = floor[currentFloor].getCell(x,y);
        if(!cell->getIfItem())
        {
            return;
        }
        std::shared_ptr<BarrierSuit> bs = dynamic_cast<BarrierSuit>(cell->getItem());
        std::shared_ptr<Compass> compass = dynamic_cast<Compass>(cell->getItem());
        if(bs != NULL)
        {
            player->setBarrierSuit(cell->getItem());
            
        }else if(compass != NULL){
            player->setCompass(cell->getItem());
        } 
        oss << "PC use "<<cell->getItem()->getType();
        action = oss.str();
        cell->setItem(NULL);
    }

    void Game::Attack(string cmd)
    {
        ostringstream oss;
        std::shared_ptr<Player> player = floors[currentFloor]->getPlayer();
        int x,y;
        x = player->getX();
        y = player->getY();
        if (cmd == "a<no>") {
            y++;
        } else if (cmd == "a<so>") {
            y--;
        } else if (cmd == "a<ea>") {
            x++;
        } else if (cmd == "a<we>") {
            x--;
        } else if (cmd == "a<ne>") {
            x--;
            y++;
        } else if (cmd == "a<nw>") {
            y++;
            x++;
        } else if (cmd == "a<se>") {
            y--;
            x++;
        } else if (cmd == "a<sw>") {
            y--;
            x--;
        } 
        std::shared_ptr<Cell> cell = floor[currentFloor].getCell(x,y);
         if(!cell->getIfEnemy())
        {
            reutrn;
        }
        std::shared_ptr<Enemy> enemy = cell->getEnemy();

        enemy->setHP(player->getStrength());

        if(enemy->getHP() == 0)
            {
                enemies.remove(enemy);
                player.setMoney(enemy->getMoney());

                std::shared_ptr<Dragon> d = dynamic_cast<Dragon>();
            }
        if(d != NULL)
            {
                player.setMoney(d->getTreasure());
                if(d->getBarrierSuit() != NULL)
                    {
                        cell->setItem(d->getBarrierSuit());
                        d->setBarrierSuit(NULL);
                    }
            }
        player->setHP(enemy->getStrength());

        floor->MoveEnemy();

        oss << "PC deals 1 demage " << ememy->getType()<<" ( "<<player->getStrength()<<" HP )." << enemy->getType() << "deals 2 demage to PC.";
        action = oss.str();
        
    }