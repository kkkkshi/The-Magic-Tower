
using namespace std;

class Character
{
	protected:
		int HP;
		int ATK;
		int DEF;
		int X;
		int Y;
		int money;
		std::shared_ptr<Compass> compass = NULL;
		Character(int hp,int atk,int def):HP(hp),Atk(atk),DEF(def){}
		virtual ~Charater(){}
	public:
		void setStrength(int ) ;
		void setHP(int) ;
		void setDEF(int) ;
		int getStrength();
		int getHP();
		int getDEF();
		void setX(int x);
		void setY(int y);
		int getX();
		int getY();	
		void setMoney(int money);
		int getMoney();
		char print()=0;
		void setCompass(std::shared_ptr<Compass>);
		std::shared_ptr<Compass> getCompass();
		virtual void move();
}
