#include "StudentWorld.h"
#include "GameConstants.h"
#include "Level.h"
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>  // defines the manipulator setw
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
	: GameWorld(assetPath)
{}

StudentWorld::~StudentWorld()
{
	cleanUp();
}

bool StudentWorld::moveActor(Direction dir, int amt, Actor *a)
{
	switch (dir)
	{
	case 90:
		if (canMove(a->getX(), a->getY() + amt, a)) {
			a->moveTo(a->getX(), a->getY() + amt);
			return true;
		}
		break;
	case 270:
		if (canMove(a->getX(), a->getY() - amt, a)) {
			a->moveTo(a->getX(), a->getY() - amt);
			return true;
		}
		break;
	case 180:
		if (canMove(a->getX() - amt, a->getY(), a)) {
			a->moveTo(a->getX() - amt, a->getY());
			return true;
		}
		break;
	case 0:
		if (canMove(a->getX() + amt, a->getY(), a)) {
			a->moveTo(a->getX() + amt, a->getY());
			return true;
		}
		break;
	}
	return false;
}

bool StudentWorld::canMove(double x1, double y1, Actor* a)
{
	double x11 = x1 + SPRITE_WIDTH - 1, y11 = y1 + SPRITE_HEIGHT - 1;
	double x2 = player->getX(), y2 = player->getY();
	double x22 = x2 + SPRITE_WIDTH - 1, y22 = y2 + SPRITE_HEIGHT - 1;
	//check if object will intersects with player after movement
	if (a != player)
	{
		if ((x1 <= x22 && x11 >= x2 && y1 < y2 && y11 >= y2) || (x1 <= x22 && x11 >= x2 && y1 > y2 && y1 <= y22)
			|| (y1 <= y22 && y11 >= y2 && x1 < x2 && x11 >= x2) || (y1 <= y22 && y11 >= y2 && x1 > x2 && x1 <= x22))
			return false;
	}
	//check if object will intersect with any other solid object after movement
	list<Actor*>::iterator it = li.begin();
	while (it != li.end())
	{
		if ((*it)->isSolid() && (*it)->isAlive() && *it != a)
		{
			double x2 = (*it)->getX(), y2 = (*it)->getY();
			double x22 = x2 + SPRITE_WIDTH - 1, y22 = y2 + SPRITE_HEIGHT - 1;
			if ((x1 <= x22 && x11 >= x2 && y1 < y2 && y11 >= y2) || (x1 <= x22 && x11 >= x2 && y1 > y2 && y1 <= y22)
				|| (y1 <= y22 && y11 >= y2 && x1 < x2 && x11 >= x2) || (y1 <= y22 && y11 >= y2 && x1 > x2 && x1 <= x22))
				return false;
		}
		it++;
	}
	return true;
}

void StudentWorld::ZombieBorn(double x, double y)
{
	if (randInt(1, 10) <= 3)
		li.push_back(new SmartZombie(x, y, this));
	else
		li.push_back(new DumbZombie(x, y, this));
}

bool StudentWorld::addFlame(double x, double y, int dir)
{
	list<Actor*>::iterator it = li.begin();
	while (it != li.end())
	{
		if ((*it)->isFireproof())
		{
			if (overlap(x, y, (*it)->getX(), (*it)->getY()))
				return false;
		}
		it++;
	}
	li.push_back(new Flame(x, y, dir, this));
	return true;
}

bool StudentWorld::tryAddVomit(double x, double y)
{
	bool doIt = (1 == randInt(1, 3)); //one in three chance zombie will do it
	list<Actor*>::iterator it = li.begin();
	if (overlap(x, y, player->getX(), player->getY()) && doIt)
	{
		li.push_back(new Vomit(x, y, this));
		playSound(SOUND_ZOMBIE_VOMIT);
		return true;
	}
	while (it != li.end())
	{
		if ((*it)->isAlive() && (*it)->canBeInfected() && overlap(x, y, (*it)->getX(), (*it)->getY()))
		{
			if (doIt)
			{
				li.push_back(new Vomit(x, y, this));
				playSound(SOUND_ZOMBIE_VOMIT);
				return true;
			}
		}
		it++;
	}
	return false;

}

void StudentWorld::addMine(double x, double y)
{
	li.push_back(new Mine(x, y, this));
}

void StudentWorld::addPit(double x, double y)
{
	li.push_back(new Pit(x, y, this));
}

void StudentWorld::addVaccine(double x, double y)
{
	int dir = 90 * randInt(0, 3);
	if (dir == 0)
		x += SPRITE_WIDTH;
	if (dir == 180)
		x -= SPRITE_WIDTH;
	if (dir == 90)
		y += SPRITE_HEIGHT;
	if (dir == 270)
		y -= SPRITE_HEIGHT;
	list<Actor*>::iterator it = li.begin();
	while (it != li.end()) {
		if (overlap(x, y, (*it)->getX(), (*it)->getY()))
			return;
		it++;
	}
	li.push_back(new VaccinePack(x, y, this));
}

//========PROCESSING=OVERLAPS=============//

bool StudentWorld::overlap(double x1, double y1, double x2, double y2)
{
	double diffx = SPRITE_WIDTH / 2;
	double diffy = SPRITE_HEIGHT / 2;
	x1 += diffx;
	x2 += diffx;
	y1 += diffy;
	y2 += diffy;
	return (x1 - x2) * (x1 - x2) + (y1 - y2)*(y1 - y2) <= 100.;
}

void StudentWorld::processFlame(double x, double y)
{

	if (overlap(x, y, player->getX(), player->getY())) {
		player->setDead();
		return; //immmediately return if player dies
	}
	list<Actor*>::iterator it = li.begin();
	while (it != li.end())
	{
		if ((*it)->canBeBurned() && (*it)->isAlive() && overlap(x, y, (*it)->getX(), (*it)->getY()))
			(*it)->setDead();
		it++;
	}
}

void StudentWorld::processVomit(double x, double y)
{
	if (overlap(x, y, player->getX(), player->getY())) {
		player->setInfect(true);
	}
	list<Actor*>::iterator it = li.begin();
	while (it != li.end())
	{
		if ((*it)->canBeInfected() && (*it)->isAlive() && overlap(x, y, (*it)->getX(), (*it)->getY()))
			(*it)->setInfect(true);
		it++;
	}
}

void StudentWorld::processPit(double x, double y)
{
	if (overlap(x, y, player->getX(), player->getY())) {
		player->setDead();
		return;
	}
	list<Actor*>::iterator it = li.begin();
	while (it != li.end())
	{
		if ((*it)->canBeBurned() && (*it)->isSolid() && (*it)->isAlive() && overlap(x, y, (*it)->getX(), (*it)->getY()))
		{
			(*it)->setDead();
		}
		it++;
	}
}

bool StudentWorld::processMine(double x, double y)
{
	list<Actor*>::iterator it = li.begin();
	while (it != li.end())
	{
		if ((*it)->isSolid() && (*it)->canBeBurned() && (*it)->isAlive() && overlap(x, y, (*it)->getX(), (*it)->getY()))
			return true;
		else if (overlap(x, y, player->getX(), player->getY()))
			return true;
		it++;
	}
	return false;
}

void StudentWorld::processExit(double x, double y)
{
	list<Actor*>::iterator it = li.begin();
	if (overlap(x, y, player->getX(), player->getY()))
		player->exitSuccess();
	while (it != li.end())
	{
		if ((*it)->canBeInfected() && (*it)->isAlive() && overlap(x, y, (*it)->getX(), (*it)->getY()))
		{
			(*it)->exitSuccess();
		}
		it++;
	}
}

//=======FOR=AI=MOVE==========//

double StudentWorld::distance(double x, double y, double x1, double y1)
{
	return sqrt((x - x1)*(x - x1) + (y - y1)*(y - y1));
}

double StudentWorld::distanceOfClosestCitizen(double x, double y)
{
	list<Actor*>::iterator it = li.begin();
	double result = 81;
	double dist;
	//check distance of all citizens
	while (it != li.end())
	{
		if ((*it)->canBeInfected() && (*it)->isAlive()) {
			dist = distance(x, y, (*it)->getX(), (*it)->getY());
			if (dist < result)
				result = dist;
		}
		it++;
	}
	return result;
}

double StudentWorld::distanceOfClosestZombie(double x, double y)
{
	list<Actor*>::iterator it = li.begin();
	double result = 1000;
	double dist;
	while (it != li.end())
	{
		if ((*it)->isSolid() && (*it)->canBeBurned() && !(*it)->canBeInfected() && (*it)->isAlive()) {
			dist = distance(x, y, (*it)->getX(), (*it)->getY());
			if (dist < result)
				result = dist;
		}
		it++;
	}
	return result;
}

double StudentWorld::distanceOfPlayer(double x, double y)
{
	return distance(x, y, player->getX(), player->getY());
}

Direction StudentWorld::chooseDirectionPlayer(double x, double y, int amt, bool isCit, Actor *a)
{
	Direction choice1 = distanceOfPlayer(x, y + amt) < distanceOfPlayer(x, y) ? 90 : 270;
	Direction choice2 = distanceOfPlayer(x + amt, y) < distanceOfPlayer(x, y) ? 0 : 180;

	if (player->getX() == x) {
		return choice1;
	}
	else if (player->getY() == y) {
		return choice2;
	}
	//if actor being chosen for is a zombie
	if (!isCit)
	{
		if (randInt(0, 1) == 1) {
			return choice1;
		}
		return choice2;
	}
	//if actor being chosen for is a citizen
	else {
		if (randInt(0, 1) == 0) {
			if (canMoveDir(x, y, choice1, amt, a))
				return choice1;
			else
				return choice2;
		}
		else {
			if (canMoveDir(x, y, choice2, amt, a))
				return choice2;
			else
				return choice1;
		}
	}
}

Direction StudentWorld::chooseDirectionCitizen(double x, double y, int amt)
{
	Direction choice1 = distanceOfClosestCitizen(x, y + amt) < distanceOfClosestCitizen(x, y) ? 90 : 270;
	Direction choice2 = distanceOfClosestCitizen(x + amt, y) < distanceOfClosestCitizen(x, y) ? 0 : 180;

	if (distanceOfClosestCitizen(x, y) - distanceOfClosestCitizen(x, y + amt) == amt || distanceOfClosestCitizen(x, y) - distanceOfClosestCitizen(x, y + amt) == -amt) {
		return choice1;
	}
	else if (distanceOfClosestCitizen(x, y) - distanceOfClosestCitizen(x + amt, y) == amt || distanceOfClosestCitizen(x, y) - distanceOfClosestCitizen(x + amt, y) == -amt) {
		return choice2;
	}
	else if (randInt(0, 1) == 1) {
		return choice1;
	}
	else {
		return choice2;
	}
}

Direction StudentWorld::chooseDirectionAwayZombie(double x, double y, Actor *a)
{
	Direction choice1 = distanceOfClosestZombie(x, y + 2) < distanceOfClosestZombie(x, y) ? 270 : 90;
	Direction choice2 = distanceOfClosestZombie(x + 2, y) < distanceOfClosestZombie(x, y) ? 180 : 0;
	bool rand = (randInt(1, 0) == 0);
	if (distanceOfClosestZombie(x, y) - distanceOfClosestZombie(x, y + 2) == 2 || distanceOfClosestZombie(x, y) - distanceOfClosestZombie(x, y + 2) == -2) {
		if (canMoveDir(x, y, choice1, 2, a))
			return choice1;
		if (rand)
		{
			if (canMoveDir(x, y, 180, 2, a))
				return 180;
			else if (canMoveDir(x, y, 0, 2, a))
				return 0;
			return 666;
		}
		if (canMoveDir(x, y, 0, 2, a))
			return 0;
		else if (canMoveDir(x, y, 180, 2, a))
			return 180;
		return 666;
	}
	else if (distanceOfClosestZombie(x, y) - distanceOfClosestZombie(x + 2, y) == 2 || distanceOfClosestZombie(x, y) - distanceOfClosestZombie(x + 2, y) == -2) {
		if (canMoveDir(x, y, choice2, 2, a))
			return choice2;
		if (rand)
		{
			if (canMoveDir(x, y, 90, 2, a))
				return 90;
			else if (canMoveDir(x, y, 270, 2, a))
				return 270;
			return 666;
		}
		if (canMoveDir(x, y, 270, 2, a))
			return 270;
		else if (canMoveDir(x, y, 90, 2, a))
			return 90;
		return 666;
	}
	double endDist1, endDist2;
	if (choice1 == 270)
		endDist1 = distanceOfClosestZombie(x, y - 2);
	if (choice1 == 90)
		endDist1 = distanceOfClosestZombie(x, y + 2);
	if (choice2 == 0)
		endDist2 = distanceOfClosestZombie(x + 2, y);
	if (choice2 == 180)
		endDist2 = distanceOfClosestZombie(x - 2, y);
	if (endDist1 > endDist2)
	{
		if (canMoveDir(x, y, choice1, 2, a))
			return choice1;
		if (canMoveDir(x, y, choice2, 2, a))
			return choice2;
		return 666;
	}
	if (canMoveDir(x, y, choice2, 2, a))
		return choice2;
	if (canMoveDir(x, y, choice1, 2, a))
		return choice1;
	return 666;
}

bool StudentWorld::canMoveDir(double x, double y, Direction dir, int amt, Actor *a)
{
	switch (dir)
	{
	case 0:
		return canMove(x + amt, y, a);
		break;
	case 180:
		return canMove(x - amt, y, a);
		break;
	case 90:
		return canMove(x, y + amt, a);
		break;
	case 270:
		return canMove(x, y - amt, a);
		break;
	default:
		return false;
	}
}

//===========MAIN=THREE=FUNCTIONS===========//

int StudentWorld::init()
{
	m_playerFinished = false;
	m_numCit = 0;
	if (getLevel() > 99)
		return GWSTATUS_PLAYER_WON;
	Level lev(assetPath());
	ostringstream lvl;
	lvl.fill('0');
	lvl << "level" << setw(2) << getLevel() << ".txt";
	cout << lvl.str();
	Level::LoadResult result = lev.loadLevel(lvl.str());
	if (result == Level::load_fail_file_not_found)
		return GWSTATUS_PLAYER_WON;
	else if (result == Level::load_fail_bad_format)
		return GWSTATUS_LEVEL_ERROR;
	else if (result == Level::load_success)
	{
		for (int i = 0; i < LEVEL_WIDTH; i++) {
			for (int j = 0; j < LEVEL_HEIGHT; j++) {
				Level::MazeEntry ge = lev.getContentsOf(i, j);
				switch (ge)
				{
				case Level::wall:
					li.push_back(new Wall(SPRITE_WIDTH*i, SPRITE_HEIGHT*j, this));
					break;
				case Level::player:
					player = new Penelope(SPRITE_WIDTH*i, SPRITE_HEIGHT*j, this);
					break;
				case Level::gas_can_goodie:
					li.push_back(new Fuel(SPRITE_WIDTH*i, SPRITE_HEIGHT*j, this));
					break;
				case Level::landmine_goodie:
					li.push_back(new MineBox(SPRITE_WIDTH*i, SPRITE_HEIGHT*j, this));
					break;
				case Level::citizen:
					li.push_back(new Citizen(SPRITE_WIDTH*i, SPRITE_HEIGHT*j, this));
					m_numCit++;
					break;
				case Level::exit:
					li.push_back(new Exit(SPRITE_WIDTH*i, SPRITE_HEIGHT*j, this));
					break;
				case Level::vaccine_goodie:
					li.push_back(new VaccinePack(SPRITE_WIDTH*i, SPRITE_HEIGHT*j, this));
					break;
				case Level::dumb_zombie:
					li.push_back(new DumbZombie(SPRITE_WIDTH*i, SPRITE_HEIGHT*j, this));
					break;
				case Level::pit:
					li.push_back(new Pit(SPRITE_WIDTH*i, SPRITE_HEIGHT*j, this));
					break;
				case Level::smart_zombie:
					li.push_back(new SmartZombie(SPRITE_WIDTH*i, SPRITE_HEIGHT*j, this));
					break;
				default:
					break;
				}
			}
		}
	}
	return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
	ostringstream oss;
	oss << "Score: ";
	oss.fill('0');
	if (getScore() < 0)
	{
		oss << "-" << setw(5) << -getScore();
	}
	else {
		oss << setw(6) << getScore();
	}
	oss.fill(' ');
	oss << setw(9) << "Level: " << getLevel();
	oss << setw(9) << "Lives: " << getLives();
	oss << setw(12) << "Vaccines: " << player->NumVac();
	oss << setw(10) << "Flames: " << player->NumFireCharges();
	oss << setw(9) << "Mines: " << player->NumMines();
	oss << setw(12) << "Infected: " << player->infectionStat();
	setGameStatText(oss.str());

	if (player->isAlive())
	{
		player->doSomething();
		if (!player->isAlive()) {
			decLives();
			return GWSTATUS_PLAYER_DIED;
		}
		list<Actor*>::iterator it = li.begin();
		while (it != li.end())
		{
			if ((*it)->isAlive())
			{
				(*it)->doSomething();
				if (!player->isAlive()) {
					decLives();
					return GWSTATUS_PLAYER_DIED;
				}
				if (m_playerFinished) {
					return GWSTATUS_FINISHED_LEVEL;
				}
			}
			it++;
		}
		it = li.begin();
		while (it != li.end())
		{
			if (!(*it)->isAlive())
			{
				//Actor* a = *it;
				delete *it;
				it = li.erase(it);
				//delete a;
			}
			else
				it++;
		}
		return GWSTATUS_CONTINUE_GAME;
	}
	else
	{
		decLives();
		return GWSTATUS_PLAYER_DIED;
	}
}

void StudentWorld::cleanUp()
{
	delete player;
	player = nullptr;
	list<Actor*>::iterator it = li.begin();
	while (it != li.end())
	{
		Actor* a = *it;
		it = li.erase(it);
		delete a;
	}
}