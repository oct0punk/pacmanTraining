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

	// Called when added to world's update
	virtual void Init();

	// Called after GameOver
	virtual void Reset() {	}

	// Called every frame
	virtual void Update(double dt);

	// Render shape
	virtual void Draw(sf::RenderWindow* win);

	// Update px and py using cx, cy, rx, ry. Then update shape's position.
	void SyncSprite();
};

class Character : public Entity {
protected:
	sf::Vector2f target;
	float dist = 0;
	float speed = 2000.0f;
	
public:
	float dx = 0;
	float dy = 0;
	Dijkstra dij;
	std::list<sf::Vector2i> nextPos;


	Character(sf::Shape* shape);

	// Called every frame
	void Update(double dt);

	// Called in Update
	virtual void UpdatePathfinding(double dt);

	// Ask the character to reach destination
	virtual void GoTo(sf::Vector2i destination);

	// Reach each elements in the order
	virtual void SetPath(std::vector<sf::Vector2i> pathfinding);

	// Auto change direction for pathfinding
	void NextTarget();
};

class Player : public Character {
public:
	sf::Vector2i lastCheckpoint;
	sf::Vector2i nextCheckpoint;
	sf::Vector2i desiredDir;

	bool* drawPrediction = new bool(false);
	bool alive = true;

	Player(sf::Shape* shape);
	
	void Update(double dt);
	void Draw(sf::RenderWindow* win);
	void Reset();
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
	float updatePathTime = 1.0f;
	float updatePathTimeRate = 1.0f;
	float initialSpeed;
	GhostState state;
	void (Ghost::* updatePtr)(double);

	bool scattering = false;
	std::vector<sf::Vector2i> scatterPath;


public:
	float time = 7.0f;
	bool* drawDestination = new bool(false);

	Ghost(sf::Shape* shape, Player* p, std::vector<sf::Vector2i> scatPath);

	// Change IAstate from current GhostState to newState
	void ChangeFSMState(GhostState newState);

	void Reset();

	void UpdatePathfinding(double dt);
	void Update(double dt);
	void ChaseUpdate(double dt);
	void ScatterUpdate(double dt);

	virtual void ChasePacman();
	bool CollideWithPacman();

	void Draw(sf::RenderWindow* win);
};


class Pinky : public Ghost {

public:
	Pinky(sf::Shape* spr, Player* pacman, std::vector<sf::Vector2i> scatPath);

	void ChasePacman();
};

class Inky : public Ghost {
	Ghost* blinky;
	sf::Vector2i originalCell;
	sf::Vector2i correctedCell;
public:
	bool* drawCellCorrection = new bool(false);

	Inky(sf::Shape* spr, Player* pacman, Ghost* blinky, std::vector<sf::Vector2i> scatPath);

	void ChasePacman();
	void Draw(sf::RenderWindow* win);
};

class Clyde : public Ghost {
public:
	Clyde(sf::Shape* spr, Player* pacman, std::vector<sf::Vector2i> scatPath);

	void ChasePacman();
};