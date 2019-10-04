#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Actor.h"
#include <string>
#include <list>
using namespace std;

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetPath);
	~StudentWorld();

	bool moveActor(Direction dir, int amt, Actor *a);
	bool playerCanExit() { return m_numCit == 0; };
	void decNumCit() { m_numCit--; };
	void ZombieBorn(double x, double y);
	void playerFinished() { m_playerFinished = true; };
	void incPlayerVac() { player->incNumVac(1); };
	void incPlayerMines() { player->incNumMines(2); };
	void incPlayerFire() { player->incNumFireCharge(5); };

	double distanceOfClosestCitizen(double x, double y);
	double distanceOfClosestZombie(double x, double y);
	double distanceOfPlayer(double x, double y);
	Direction chooseDirectionPlayer(double x, double y, int amt, bool isCit, Actor *a);
	Direction chooseDirectionCitizen(double x, double y, int amt);
	Direction chooseDirectionAwayZombie(double x, double y, Actor *a);

	bool addFlame(double x, double y, int dir);
	bool tryAddVomit(double x, double y);
	void addMine(double x, double y);
	void addPit(double x, double y);
	void addVaccine(double x, double y);

	void processFlame(double x, double y);
	void processVomit(double x, double y);
	void processPit(double x, double y);
	bool processMine(double x, double y); //returns true if mine explodes
	void processExit(double x, double y);

	virtual int init();
	virtual int move();
	virtual void cleanUp();

private:
	double distance(double x, double y, double x1, double y1);
	bool overlap(double x1, double y1, double x2, double y2);
	bool canMoveDir(double x, double y, Direction dir, int amt, Actor *a);
	bool canMove(double x, double y, Actor *a);
	bool m_playerFinished;
	int m_numCit;
	Penelope *player;
	list<Actor*> li;
};

#endif // STUDENTWORLD_H_
