void Character::setX(int x)
{
    this.X = x;
}

void Character::setY(int y)
{
    this.Y = y;
}

int Character::getX()
{
    return this.X;
}

int Character::getY()
{
    return this.Y;
}

void Character::setStrength(int strength)
{
    this.ATK = strength;
}

void Character::setHP(int hp)
{
    this.HP = hp;
}

void Character::setDEF(int def)
{
    this.DEF = def;
}

void Character::setMoney(int money)
{
    if(money > 0)
        this.Money += money;
}

int Character::getMoney()
{
    return this.Money;
}

int Character::getStrength()
{
    return this.ATK;
}
int Character::getHP()
{
    return this.HP;
}

int Character::getDEF()
{
    return this.DEF;
}

void Charactor::setCompass(std::shared_ptr<Compass> compass)
{
    this.compass = compass;
}
std::shared_ptr<Compass> getCompass()
{
    return this.compass;
}
void Charactor::move(int direction)
{
    switch (direction)
    {
        case 0://UP
            if(this.Y < Floor.getMaxY())
                this.Y++;
            break;
        case 1://DOWN
            if(this.Y > 0)
                this.Y--;
            break;
        case 2://LEFT
            if(this.X < 0)
                this.X--;
            break;
        case 3://RIGTH
            if(this.X < Floor.getMAXX())
                this.X ++;
                break;
        default:
            break;
    }
}
