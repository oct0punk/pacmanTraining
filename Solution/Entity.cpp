#pragma once
#include "Entity.h"
#include "World.h"
#include <iostream>

float clamp(float val, float a, float b) {
	if (val < a)
		val = a;
	if (val > b)
		val = b;
	return val;
}

sf::Vector2f Normalize(sf::Vector2f v) {
	float len = sqrt(v.x * v.x + v.y * v.y);
	sf::Vector2f vec;
	vec.x = v.x / len;
	vec.y = v.y / len;
	return vec;
}

Entity::Entity(sf::Shape* shape) {
	this->shape = shape;
}

void Entity::Init() {
	SyncSprite();
}

void Entity::Update(double dt) {
	SyncSprite();
}

void Entity::Draw(sf::RenderWindow* win) {
	win->draw(*shape);
}

void Entity::SyncSprite() {
	while (rx < 0) {
		cx--;
		rx++;
	}
	while (rx > 1) {
		rx--;
		cx++;
	}
	while (ry < 0) {
		cy--;
		ry++;
	}
	while (ry > 1) {
		ry--;
		cy++;
	}

	cx = clamp(cx, 1, 39);
	cy = clamp(cy, 1, 39);


	px = (cx + rx) * *cellSize;
	py = (cy + ry) * *cellSize;
	shape->setPosition(px, py);
}

Character::Character(sf::Shape* shape) : Entity(shape) {

}

void Character::Update(double dt) {
// Move
rx += dx * speed * dt;
ry += dy * speed * dt;

SyncSprite();
}

void Character::UpdatePathfinding(double dt)
{	
	dist -= speed * dt;
	// go to next target ?
	if (dist <= 0) {
		rx = ry = .5f;
		if (nextPos.size()) {
			cx = nextPos.front().x;
			cy = nextPos.front().y;
			nextPos.pop_front();
			NextTarget();
		}
	}
}

void Character::GoTo(sf::Vector2i end) {
	SetPath(dij.FindPath(sf::Vector2i(cx, cy), end));
}

void Character::SetPath(std::vector<sf::Vector2i> pathfinding) {
	nextPos.clear();
	dist = 0;
	for (auto s : pathfinding)
		nextPos.push_back(s);
	if (nextPos.size())
		nextPos.pop_front();
	NextTarget();
}

void Character::NextTarget() {
	if (!nextPos.size()) {
		dx = 0;
		dy = 0;
		return;
		// Stop movement
	}

	px = (cx + rx) * *cellSize;
	py = (cy + ry) * *cellSize;

	// Set next target position
	target.x = (nextPos.front().x + .5f) * *cellSize;
	target.y = (nextPos.front().y + .5f) * *cellSize;

	// set move's direction
	sf::Vector2f vMove(target - sf::Vector2f(px, py));
	sf::Vector2f direction(vMove);
	direction = Normalize(direction);
	dx = direction.x;
	dy = direction.y;

	dist = sqrt(vMove.x * vMove.x + vMove.y * vMove.y) / *cellSize;
}

Player::Player(sf::Shape* shape) : Character(shape) {
	dx = 1;
	dy = 0;
}

void Player::Update(double dt) {

	// Update dir
	if (!World::GetInstance()->isColliding(cx + desiredDir.x, cy + desiredDir.y))
	{
		// Auto turn
		if (fabs(rx - .5f) < speed * dt)
			if (fabs(ry - .5f) < speed * dt) {
				rx = ry = .5f;
				dx = desiredDir.x;
				dy = desiredDir.y;
			}

	}

	// Anticipate pacman's direction
	int x = cx;
	int y = cy;
	while (!World::GetInstance()->isColliding(x, y)) {
		x += dx;
		y += dy;
		auto iter = std::find(dij.vertices.begin(), dij.vertices.end(), sf::Vector2i(x, y));
		if (iter != dij.vertices.end()) {
			int index = iter - dij.vertices.begin();
			Pole pole = dij.vertices.at(index);
			nextCheckpoint = pole.pos;

			if (desiredDir.x != dx || desiredDir.y != dy) {
				if (desiredDir.x < 0) {
					if (pole.nLeft.first)
						nextCheckpoint = pole.nLeft.second;
				}
				else if (desiredDir.x > 0) {
					if (pole.nRight.first)
						nextCheckpoint = pole.nRight.second;
				}
				else if (desiredDir.y < 0) {
					if (pole.nUp.first)
						nextCheckpoint = pole.nUp.second;
				}
				else if (desiredDir.y > 0) {
					if (pole.nDown.first)
						nextCheckpoint = pole.nDown.second;
				}
			}
			break;
		}
	}
	
	// Move
	rx += dx * speed * dt;
	ry += dy * speed * dt;
	dist -= speed * dt;

	// Stop
	if (World::GetInstance()->isColliding(cx + dx, cy + dy)) {
		if (dx < 0)
			rx = clamp(rx, .5f, 1);
		if (dx > 0)
			rx = clamp(rx, 0, .5f);
		if (dy < 0)
			ry = clamp(ry, .5f, 1);
		if (dy > 0)
			ry = clamp(ry, 0, .5f);
	}

	SyncSprite();
}

void Player::Draw(sf::RenderWindow* win) {
	Entity::Draw(win);

	// Debug
	if (*drawPrediction) {
		sf::VertexArray dijkstraArr;
		dijkstraArr.setPrimitiveType(sf::PrimitiveType::Lines);
		sf::Vertex pPos;
		pPos.position = sf::Vector2f(px, py);
		pPos.color = sf::Color::Green;
		sf::Vertex pNextPos;
		pNextPos.position = sf::Vector2f((nextCheckpoint.x + .5f) * *cellSize, (nextCheckpoint.y + .5f) * *cellSize);
		pNextPos.color = sf::Color::Blue;
		dijkstraArr.append(pPos);
		dijkstraArr.append(pNextPos);
		win->draw(dijkstraArr);
	}
}

void Player::Reset() {
	alive = true;
	dx = dy = 0.0f;
	cx = 19;
	cy = 25;
	desiredDir = sf::Vector2i();
}

Inky::Inky(sf::Shape* spr, Player* pacman, Ghost* blinky, std::vector<sf::Vector2i> scatPath) : Ghost(spr, pacman, scatPath) {
	this->blinky = blinky;
}

void Inky::ChasePacman() {
	sf::Vector2i blinkyToTarget = p->nextCheckpoint - sf::Vector2i(blinky->cx, blinky->cy);
	blinkyToTarget *= 2;
	blinkyToTarget += sf::Vector2i(blinky->cx, blinky->cy);
	originalCell = blinkyToTarget;
	blinkyToTarget = World::GetInstance()->FindNearestCorrectCell(blinkyToTarget);
	correctedCell = blinkyToTarget;
	GoTo(blinkyToTarget);

}

void Inky::Draw(sf::RenderWindow* win) {
	if (*drawCellCorrection) {
		sf::RectangleShape debugCell(sf::Vector2f(*cellSize - 13, *cellSize - 13));
		debugCell.setOrigin((*cellSize - 13) / 2, (*cellSize - 13) / 2);
		debugCell.setFillColor(sf::Color::Red);

		sf::Vector2f cellPos;
		cellPos.x = (originalCell.x + .5f) * *cellSize;
		cellPos.y = (originalCell.y + .5f) * *cellSize;
		debugCell.setPosition(cellPos);
		win->draw(debugCell);

		debugCell.setFillColor(sf::Color::Green);
		cellPos.x = (correctedCell.x + .5f) * *cellSize;
		cellPos.y = (correctedCell.y + .5f) * *cellSize;
		debugCell.setPosition(cellPos);
		win->draw(debugCell);
	}
	Entity::Draw(win);
}

Ghost::Ghost(sf::Shape* shape, Player* p, std::vector<sf::Vector2i> scatPath) : Character(shape) {
	initialSpeed = speed;
	this->p = p;
	scatterPath = scatPath;
	ChangeFSMState(GhostState::Chase);
}

// Change IAstate from current GhostState to newState

void Ghost::ChangeFSMState(GhostState newState) {
	nextPos.clear();
	updatePathTime = 1.0f;
	dx = dy = 0;
	switch (newState)
	{
	case Chase:
		time = 0.05f;
		updatePathTimeRate = 1.0f;
		updatePtr = &Ghost::ChaseUpdate;
		break;
	case Scatter:
		time = 0.01f;
		updatePathTimeRate = 0.0f;
		scattering = false;
		updatePtr = &Ghost::ScatterUpdate;
		break;
	case Frightened:
		break;
	default:
		break;
	}
}

void Ghost::Reset() {
	nextPos.clear();
	cx = scatterPath.front().x;
	cy = scatterPath.front().y;
	rx = ry = .5f;
	SyncSprite();
	;		ChangeFSMState(GhostState::Scatter);
}

void Ghost::UpdatePathfinding(double dt) {
	updatePathTime -= dt * updatePathTimeRate;
	if (!nextPos.size() || updatePathTime < 0) {
		nextPos.clear();
		dx = dy = 0;
		updatePathTime = .01f;
		ChasePacman();
	}
	Character::UpdatePathfinding(dt);
}

void Ghost::Update(double dt) {
	if (CollideWithPacman()) {
		World::GetInstance()->GameOver();
		return;
	}
	time -= dt;
	(this->*updatePtr)(dt);
}

void Ghost::ChaseUpdate(double dt) {
	UpdatePathfinding(dt);
	Character::Update(dt);
	if (time < 0)
		ChangeFSMState(GhostState::Scatter);
}

void Ghost::ScatterUpdate(double dt) {
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

void Ghost::ChasePacman() {
	GoTo(sf::Vector2i(p->cx, p->cy));
}

bool Ghost::CollideWithPacman() {
	return sf::Vector2i(cx, cy) == sf::Vector2i(p->cx, p->cy);
}

void Ghost::Draw(sf::RenderWindow* win) {
	Character::Draw(win);
	if (*drawDestination) {
		sf::VertexArray arr;
		arr.setPrimitiveType(sf::PrimitiveType::Lines);
		for (auto p : nextPos) {
			sf::Vertex v;
			v.position.x = px;
			v.position.y = py;
			v.color = shape->getFillColor();
			arr.append(v);
			v.color = sf::Color::Yellow;
			v.position.x = (nextPos.front().x + .5f) * *cellSize;
			v.position.y = (nextPos.front().y + .5f) * *cellSize;
			arr.append(v);
		}
		win->draw(arr);
	}
}

Clyde::Clyde(sf::Shape* spr, Player* pacman, std::vector<sf::Vector2i> scatPath) : Ghost(spr, pacman, scatPath) {
}

void Clyde::ChasePacman() {
	sf::Vector2i c = World::GetInstance()->FindNearestCorrectCell(sf::Vector2i(cx, cy));
	Pole p = dij.FindNearestNeighbourPole(sf::Vector2i(c.x, c.y));
	std::vector<sf::Vector2i> arr;
	if (p.nDown.first)
		arr.push_back(p.nDown.second);
	if (p.nLeft.first)
		arr.push_back(p.nLeft.second);
	if (p.nRight.first)
		arr.push_back(p.nRight.second);
	if (p.nUp.first)
		arr.push_back(p.nUp.second);

	int idx = rand() % arr.size();
	GoTo(arr[idx]);
}

Pinky::Pinky(sf::Shape* spr, Player* pacman, std::vector<sf::Vector2i> scatPath) : Ghost(spr, pacman, scatPath) {

}

void Pinky::ChasePacman() {
	GoTo(p->nextCheckpoint);
	nextPos.push_back(sf::Vector2i(p->cx, p->cy));
}
