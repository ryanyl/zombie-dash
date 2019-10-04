#include "Actor.h"
#include "StudentWorld.h"

Actor::Actor(int imageID, double x, double y, int startDirection, int depth, bool tang, bool canBeFlamed, bool canbeinfec, bool fireproof, StudentWorld* world) : GraphObject(imageID, x, y, startDirection, depth)
{
	s = world;
	m_solid = tang;
	m_canBeBurned = canBeFlamed;
	m_canBeInfected = canbeinfec;
	m_alive = true;
	m_fireproof = fireproof;
}

StudentWorld* Actor::getWorld()
{
	return s;
}

void Actor::setDead()
{
	m_alive = false;
}

Human::Human(int imageID, double x, double y, StudentWorld* s) : Actor(imageID, x, y, right, 0, true, true, true, false, s)
{
	infecStat = 0;
	infected = false;
}

bool Human::criticalInfection() const
{
	return infecStat == 500;
}

void Human::cure()
{
	setInfect(false);
	infecStat = 0;
}

void Human::increaseInfection()
{
	infecStat++;
}

Penelope::Penelope(double x, double y, StudentWorld* s) : Human(IID_PLAYER, x, y, s)
{
	m_numVac = m_numFireCharges = m_numMines = 0;
}

void Penelope::doSomething()
{
	if (!isAlive())
		return;
	if (isInfected())
		increaseInfection();
	if (criticalInfection())
	{
		setDead();
		return;
	}
	int ch;
	if (getWorld()->getKey(ch))
	{
		switch (ch)
		{
		case KEY_PRESS_LEFT:
			setDirection(left);
			getWorld()->moveActor(left, 4, this);
			break;
		case KEY_PRESS_RIGHT:
			setDirection(right);
			getWorld()->moveActor(right, 4, this);
			break;
		case KEY_PRESS_UP:
			setDirection(up);
			getWorld()->moveActor(up, 4, this);
			break;
		case KEY_PRESS_DOWN:
			setDirection(down);
			getWorld()->moveActor(down, 4, this);
			break;
		case KEY_PRESS_ENTER:
			if (m_numVac >= 1) {
				cure();
				m_numVac--;
			}
			break;
		case KEY_PRESS_SPACE:
			if (m_numFireCharges >= 1) {
				int dir = getDirection();
				for (int i = 1; i <= 3; i++) {
					if (dir == right) {
						if (!getWorld()->addFlame(getX() + i * SPRITE_WIDTH, getY(), dir))
							break;
					}
					else if (dir == left) {
						if (!getWorld()->addFlame(getX() - i * SPRITE_WIDTH, getY(), dir))
							break;
					}
					else if (dir == up) {
						if (!getWorld()->addFlame(getX(), getY() + i * SPRITE_HEIGHT, dir))
							break;
					}
					else if (dir == down) {
						if (!getWorld()->addFlame(getX(), getY() - i * SPRITE_HEIGHT, dir))
							break;
					}
				}
				getWorld()->playSound(SOUND_PLAYER_FIRE);
				m_numFireCharges--;
			}
			break;
		case KEY_PRESS_TAB:
			if (m_numMines >= 1)
			{
				getWorld()->addMine(getX(), getY());
				m_numMines--;
			}
		default:
			break;
		}
	}
}

void Penelope::setDead()
{
	Actor::setDead();
	getWorld()->playSound(SOUND_PLAYER_DIE);
}

void Penelope::exitSuccess()
{
	if (getWorld()->playerCanExit())
	{
		getWorld()->playerFinished();
		getWorld()->playSound(SOUND_LEVEL_FINISHED);
	}
}

Citizen::Citizen(double x, double y, StudentWorld* s) : Human(IID_CITIZEN, x, y, s)
{
	paralyzed = 1;
}

void Citizen::doSomething()
{
	if (!isAlive())
		return;
	//process infection
	if (isInfected())
		increaseInfection();
	if (criticalInfection())
	{
		Actor::setDead();
		getWorld()->increaseScore(-1000);
		getWorld()->playSound(SOUND_ZOMBIE_BORN);
		getWorld()->decNumCit();
		getWorld()->ZombieBorn(getX(), getY());
		return;
	}
	//citizezn moves every other tick
	bool randomBool = (randInt(1, 0) == 0);
	paralyzed *= -1;
	if (paralyzed == 1)
	{
		double dist_p = getWorld()->distanceOfPlayer(getX(), getY());
		double dist_z = getWorld()->distanceOfClosestZombie(getX(), getY());
		if (dist_p < dist_z && dist_p <= 80) {
			Direction dir = getWorld()->chooseDirectionPlayer(getX(), getY(), 2, true, this);
			if (getWorld()->moveActor(dir, 2, this)) {
				setDirection(dir);
				return;
			}
		}
		if (dist_z <= 80)
		{
			Direction direct = getWorld()->chooseDirectionAwayZombie(getX(), getY(), this);
			if (direct == 666)
				return;
			else
			{
				setDirection(direct);;
				getWorld()->moveActor(direct, 2, this);
			}
		}
	}
}

void Citizen::setInfect(bool x)
{
	if (!isInfected())
		getWorld()->playSound(SOUND_CITIZEN_INFECTED);
	Human::setInfect(x);
}

void Citizen::setDead()
{
	Actor::setDead();
	getWorld()->playSound(SOUND_CITIZEN_DIE);
	getWorld()->increaseScore(-1000);
	getWorld()->decNumCit();
}

void Citizen::exitSuccess()
{
	Actor::setDead();
	getWorld()->decNumCit();
	getWorld()->increaseScore(500);
	getWorld()->playSound(SOUND_CITIZEN_SAVED);
}

//====================ZOMBIES======================//

Zombie::Zombie(double x, double y, StudentWorld* s) : Actor(IID_ZOMBIE, x, y, right, 0, true, true, false, false, s)
{
	paralyzed = -1;
}

void Zombie::doSomething()
{
	if (!isAlive())
		return;
	paralyzed *= -1;
	if (paralyzed == 1)
		return;
	//if there is person in front of it vomit and return
	if (getDirection() == up)
	{
		if (getWorld()->tryAddVomit(getX(), getY() + SPRITE_HEIGHT))
			return;
	}
	else if (getDirection() == down)
	{
		if (getWorld()->tryAddVomit(getX(), getY() - SPRITE_HEIGHT))
			return;
	}
	else if (getDirection() == right)
	{
		if (getWorld()->tryAddVomit(getX() + SPRITE_WIDTH, getY()))
			return;
	}
	else if (getDirection() == left)
	{
		if (getWorld()->tryAddVomit(getX() - SPRITE_WIDTH, getY()))
			return;
	}
	makeMove(); //calls are made only to the derived class's version of makeMove()
}

void Zombie::setDead()
{
	Actor::setDead();
	getWorld()->playSound(SOUND_ZOMBIE_DIE);
}

DumbZombie::DumbZombie(double x, double y, StudentWorld *s) : Zombie(x, y, s) { planDistance = 0; }

void DumbZombie::setDead()
{
	Zombie::setDead();
	getWorld()->increaseScore(1000);
	if (randInt(1, 10) == 1)
		getWorld()->addVaccine(getX(), getY());
}

void DumbZombie::makeMove()
{
	if (planDistance == 0)
	{
		planDistance = randInt(3, 10);
		setDirection(randInt(0, 3) * 90);
		return;
	}
	if (getWorld()->moveActor(getDirection(), 1, this))
		planDistance--;
	else
		planDistance = 0;
}

SmartZombie::SmartZombie(double x, double y, StudentWorld *s) : Zombie(x, y, s)
{
	planDistance = 0;
}

void SmartZombie::setDead()
{
	Zombie::setDead();
	getWorld()->increaseScore(2000);
}

void SmartZombie::makeMove()
{
	if (planDistance == 0)
	{
		planDistance = randInt(3, 10);
		double distCitizen = getWorld()->distanceOfClosestCitizen(getX(), getY());
		double distPlayer = getWorld()->distanceOfPlayer(getX(), getY());
		if (distCitizen <= 80 && distCitizen < distPlayer)
			setDirection(getWorld()->chooseDirectionCitizen(getX(), getY(), 1));
		else if (distPlayer <= 80 && distPlayer < distCitizen)
			setDirection(getWorld()->chooseDirectionPlayer(getX(), getY(), 1, false, this));
		else
		{
			setDirection(randInt(0, 3) * 90);
		}
		return;
	}
	if (getWorld()->moveActor(getDirection(), 1, this))
		planDistance--;
	else
	{
		planDistance = 0;
	}
}


//=============NOTFLAMMABLE============//

Wall::Wall(double x, double y, StudentWorld* s) : Actor(IID_WALL, x, y, right, 0, true, false, false, true, s)
{}

Exit::Exit(double x, double y, StudentWorld* s) : Actor(IID_EXIT, x, y, right, 1, false, false, false, true, s) {}

void Exit::doSomething()
{
	getWorld()->processExit(getX(), getY());
}

//============projectile==============//

Projectile::Projectile(int IID, double x, double y, int dir, StudentWorld *s) : Actor(IID, x, y, dir, 0, false, false, false, false, s)
{
	tick = 0;
}

void Projectile::incTick()
{
	if (tick == 2) {
		setDead();
		return;
	}
	tick++;
}

Flame::Flame(double x, double y, int dir, StudentWorld* s) : Projectile(IID_FLAME, x, y, dir, s)
{
}

void Flame::doSomething()
{
	incTick();
	if (!isAlive())
		return;
	getWorld()->processFlame(getX(), getY());
}

Vomit::Vomit(double x, double y, StudentWorld *s) : Projectile(IID_VOMIT, x, y, right, s) {}

void Vomit::doSomething()
{
	incTick();
	if (!isAlive())
		return;
	getWorld()->processVomit(getX(), getY());
}

//===============MINEZZ================//

Mine::Mine(double x, double y, StudentWorld *s) : Actor(IID_LANDMINE, x, y, right, 1, false, true, false, false, s)
{
	tick = 30;
}

void Mine::setDead()
{
	Actor::setDead();
	getWorld()->playSound(SOUND_LANDMINE_EXPLODE);
	//introduce flames resulting from explosion
	getWorld()->addFlame(getX(), getY(), up);
	getWorld()->addFlame(getX() + SPRITE_WIDTH, getY() + SPRITE_HEIGHT, up);
	getWorld()->addFlame(getX() + SPRITE_WIDTH, getY() - SPRITE_HEIGHT, up);
	getWorld()->addFlame(getX() + SPRITE_WIDTH, getY(), up);
	getWorld()->addFlame(getX() - SPRITE_WIDTH, getY() + SPRITE_HEIGHT, up);
	getWorld()->addFlame(getX() - SPRITE_WIDTH, getY() - SPRITE_HEIGHT, up);
	getWorld()->addFlame(getX() - SPRITE_WIDTH, getY(), up);
	getWorld()->addFlame(getX(), getY() - SPRITE_HEIGHT, up);
	getWorld()->addFlame(getX(), getY() + SPRITE_HEIGHT, up);
	//introduce pit
	getWorld()->addPit(getX(), getY());
}

void Mine::doSomething()
{
	if (!isAlive())
		return;
	if (tick > 0)
	{
		tick--;
		return;
	}
	if (getWorld()->processMine(getX(), getY()))
		setDead();
}

//==============PIT====================//

Pit::Pit(double x, double y, StudentWorld *s) : Actor(IID_PIT, x, y, right, 0, false, false, false, false, s) {}

void Pit::doSomething()
{
	getWorld()->processPit(getX(), getY());
}

//================GOODIES===============//

Goodie::Goodie(int imageID, double x, double y, StudentWorld *s) : Actor(imageID, x, y, right, 1, false, true, false, false, s) {}

void Goodie::setCollected()
{
	setDead();
	getWorld()->increaseScore(50);
	getWorld()->playSound(SOUND_GOT_GOODIE);
}

void Goodie::doSomething()
{
	if (getWorld()->distanceOfPlayer(getX(), getY()) <= 10.0)
		setCollected();
}

VaccinePack::VaccinePack(double x, double y, StudentWorld *s) : Goodie(IID_VACCINE_GOODIE, x, y, s) {}

void VaccinePack::setCollected()
{
	Goodie::setCollected();
	getWorld()->incPlayerVac();
}

MineBox::MineBox(double x, double y, StudentWorld *s) :Goodie(IID_LANDMINE_GOODIE, x, y, s) {}

void MineBox::setCollected()
{
	Goodie::setCollected();
	getWorld()->incPlayerMines();
}

Fuel::Fuel(double x, double y, StudentWorld *s) : Goodie(IID_GAS_CAN_GOODIE, x, y, s) {}

void Fuel::setCollected()
{
	Goodie::setCollected();
	getWorld()->incPlayerFire();
}