#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld;

class Actor : public GraphObject
{
public:
	Actor(int imageID, double x, double y, int startDirection, int depth, bool tang, bool canBeFlamed, bool canbeinfec, bool fireproof, StudentWorld* world);
	StudentWorld* getWorld();

	bool canBeBurned() const { return m_canBeBurned; };
	bool canBeInfected() const { return m_canBeInfected; };
	virtual void setInfect(bool x) {};
	bool isAlive() const { return m_alive; };
	bool isSolid() const { return m_solid; };
	bool isFireproof() const { return m_fireproof; };
	virtual void exitSuccess() {};
	virtual void setDead();
	virtual void doSomething() = 0;
	virtual ~Actor() {};
private:
	StudentWorld* s;
	
	bool m_canBeBurned;
	bool m_canBeInfected;
	bool m_alive;
	bool m_solid;
	bool m_fireproof;
};

//===========HUMANS============//

class Human : public Actor
{
public:
	Human(int imageID, double x, double y, StudentWorld* s);
	virtual void setInfect(bool x) { infected = x; };
	bool isInfected() const { return infected; };
	bool criticalInfection() const;
	int infectionStat() const { return infecStat; };
	void cure();
	void increaseInfection();
	virtual ~Human() {};
private:
	bool infected;
	int infecStat;
};

class Penelope : public Human
{
public:
	Penelope(double x, double y, StudentWorld* s);
	virtual void doSomething();
	virtual void setDead();
	void exitSuccess();
	void incNumVac(int amt) { m_numVac+=amt; };
	void incNumFireCharge(int amt) { m_numFireCharges += amt; };
	void incNumMines(int amt) { m_numMines += amt; };
	int NumVac() const { return m_numVac; };
	int NumMines() const { return m_numMines; };
	int NumFireCharges() const { return m_numFireCharges; };
private:
	int m_numVac;
	int m_numMines;
	int m_numFireCharges;
};

class Citizen : public Human
{
public:
	Citizen(double x, double y, StudentWorld* s);
	virtual void setInfect(bool x);
	virtual void doSomething();
	virtual void setDead();
	void exitSuccess();
private:
	int paralyzed;
};

class Zombie : public Actor
{
public:
	Zombie(double x, double y, StudentWorld* s);
	virtual void setDead();
	virtual void doSomething();
	virtual void makeMove() = 0;
	virtual ~Zombie() {};
private:
	int paralyzed;
};

class DumbZombie :public Zombie
{
public:
	DumbZombie(double x, double y, StudentWorld*s);
	virtual void makeMove();
	virtual void setDead();
private:
	int planDistance;
};

class SmartZombie : public Zombie
{
public:
	SmartZombie(double x, double y, StudentWorld* s);
	virtual void makeMove();
	virtual void setDead();
private:
	int planDistance;
};

class Wall : public Actor
{
public:
	Wall(double x, double y, StudentWorld *s);
	virtual void doSomething() {};
};

class Exit : public Actor
{
public:
	Exit(double x, double y, StudentWorld *s);
	virtual void doSomething();
};

//===========projectiles==========//

class Projectile : public Actor
{
public:
	Projectile(int IID, double x, double y, int dir, StudentWorld *s);
	void incTick();
	virtual ~Projectile() {};
private:
	int tick;
};

class Flame : public Projectile
{
public:
	Flame(double x, double y, int dir, StudentWorld *s);
	virtual void doSomething();
};

class Vomit : public Projectile
{
public:
	Vomit(double x, double y, StudentWorld *s);
	virtual void doSomething();
};

//===============MINETURUTE============//

class Mine : public Actor
{
public:
	Mine(double x, double y, StudentWorld *s);
	virtual void setDead();
	virtual void doSomething();
private:
	int tick;
};

//=============PITS=================//

class Pit : public Actor
{
public:
	Pit(double x, double y, StudentWorld *s);
	virtual void doSomething();
};

//==============GOODIESSZZZ============//

class Goodie : public Actor
{
public:
	Goodie(int imageID, double x, double y, StudentWorld *s);
	virtual void doSomething();
	virtual void setCollected();
	virtual ~Goodie() {};
};

class VaccinePack : public Goodie
{
public:
	VaccinePack(double x, double y, StudentWorld *s);
	virtual void setCollected();
};

class MineBox : public Goodie
{
public:
	MineBox(double x, double y, StudentWorld *s);
	virtual void setCollected();
};

class Fuel : public Goodie
{
public:
	Fuel(double x, double y, StudentWorld *s);
	virtual void setCollected();
};

#endif // ACTOR_H_
