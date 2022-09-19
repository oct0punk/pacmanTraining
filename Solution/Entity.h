#pragma once
#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"
#include "imgui.h"
#include "imgui-SFML.h"
#include <list>
#include "Dijkstra.h"
#include <iostream>


float clamp(float val, float a, float b);
sf::Vector2f Normalize(sf::Vector2f v);

class Entity {

public:
	sf::Shape* shape = nullptr;
	int* cellSize = nullptr;

	int cx = 0;
	int cy = 0;

	float rx = 0;
	float ry = 0;

	float px = 0;
	float py = 0;

	

	Entity(sf::Shape* shape);

	virtual void Init();

	virtual void Update(double dt);
	void		Draw(sf::RenderWindow* win);

	void SyncSprite();

};


class Character : public Entity {
protected:
	std::list<sf::Vector2i> nextPos;
	sf::Vector2f target;
	float dist = 0;
	int speed = 3000;
	float dx = 0;
	float dy = 0;
	
public:
	Dijkstra dij;


	Character(sf::Shape* shape);

	virtual void Update(double dt);

	void UpdatePathfinding();

	// PATHFINDING
	void GoTo(sf::Vector2i destination);
	virtual void SetPath(std::vector<sf::Vector2i> pathfinding);
	void NextTarget();
};

class Player : public Character {
public:
	sf::Vector2i lastCheckpoint;
	sf::Vector2i nextCheckpoint;
	sf::Vector2i desiredDir;

	Player(sf::Shape* shape) : Character(shape) {
		dx = 1;
		dy = 0;
	}
	
	void Update(double dt);
};

enum GhostState
{
	Chase,
	Scatter,
	Frightened
};

class Ghost : public Character {
protected:
	Player* p = nullptr;
	float time = 1.0f;
	float initialSpeed;
	GhostState state;
	void (Ghost::* updatePtr)(double);


public:
	Ghost(sf::Shape* shape, Player* p) : Character(shape) {
		initialSpeed = speed;
		this->p = p;
		updatePtr = &Ghost::ChaseUpdate;
	}

	void ChangeFSMState(GhostState newState) {
		nextPos.clear();
		switch (newState)
		{
		case Chase:
			updatePtr = &ChaseUpdate;
			break;
		case Scatter:
			break;
		case Frightened:
			break;
		default:
			break;
		}
	}

	void Update(double dt) {
		(this->*updatePtr)(dt);
	}
	void ChaseUpdate(double dt) {
		time -= dt;
		if (!nextPos.size()) {
			rx = ry = .5f;
			dx = dy = 0;
			nextPos.clear();
			time = 1;
			ChasePacman();
		}
		Character::Update(dt);
	}

	virtual void ChasePacman() {
		GoTo(sf::Vector2i(p->lastCheckpoint.x, p->lastCheckpoint.y));
		nextPos.push_back(sf::Vector2i(p->cx, p->cy));
	}

};
