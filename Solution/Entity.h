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
	sf::Vector2f target;
	float dist = 0;
	int speed = 1000;
	
public:
	float dx = 0;
	float dy = 0;
	Dijkstra dij;
	std::list<sf::Vector2i> nextPos;


	Character(sf::Shape* shape);

	virtual void Update(double dt);

	virtual void UpdatePathfinding(double dt);

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
	float time = 7.0f;
	float initialSpeed;
	GhostState state;
	void (Ghost::* updatePtr)(double);

	bool scattering = false;
	std::vector<sf::Vector2i> scatterPath;


public:
	Ghost(sf::Shape* shape, Player* p, std::vector<sf::Vector2i> scatPath) : Character(shape) {
		initialSpeed = speed;
		this->p = p;
		scatterPath = scatPath;
		ChangeFSMState(GhostState::Chase);
	}

	void ChangeFSMState(GhostState newState) {
		nextPos.clear();
		dx = dy = 0;
		rx = ry = .5f;
		switch (newState)
		{
		case Chase:
			time = 7.0f;
			updatePtr = &Ghost::ChaseUpdate;
			break;
		case Scatter:
			time = 5.0f;
			scattering = false;
			updatePtr = &Ghost::ScatterUpdate;
			break;
		case Frightened:
			break;
		default:
			break;
		}
	}

	void UpdatePathfinding(double dt) {
		if (!nextPos.size())
			ChasePacman();
		Character::UpdatePathfinding(dt);
	}
	void Update(double dt) {
		time -= dt * 200.0f;
		std::cout << time;
		std::cout << "\n";
		(this->*updatePtr)(dt);
	}
	void ChaseUpdate(double dt) {
		UpdatePathfinding(dt);
		Character::Update(dt);
		if (time < 0)
			ChangeFSMState(GhostState::Scatter);
	}
	void ScatterUpdate(double dt) {
		if (!scattering) {
			scattering = true;
			GoTo(scatterPath.front());
		}
		if (nextPos.empty()) {
			for (auto s : scatterPath)
				nextPos.push_back(s);
			NextTarget();
		}
		
		UpdatePathfinding(dt);
		Character::Update(dt);
		if (time < 0)
			ChangeFSMState(GhostState::Chase);
	}

	virtual void ChasePacman() {
		GoTo(sf::Vector2i(p->cx, p->cy));
	}

};
