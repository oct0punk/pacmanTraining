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
		cx = nextPos.front().x;
		cy = nextPos.front().y;
		if (nextPos.size()) {
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

	auto iter = std::find(dij.vertices.begin(), dij.vertices.end(), sf::Vector2i(cx, cy));
	if (iter != dij.vertices.end())
	{
		int index = iter - dij.vertices.begin();
		Pole pole = dij.vertices.at(index);
		lastCheckpoint = pole.pos;
		if (sf::Vector2i(cx, cy) == sf::Vector2i(1, 1)) {
			sf::Vector2i();
		}

		// Anticipate pacman's direction
		sf::Vector2i direction(dx, dy);
		if (direction == sf::Vector2i(1, 0)) {
			if (pole.nRight.first)
				nextCheckpoint = pole.nRight.second;
			else
				nextCheckpoint = pole.pos;
		}
		else if (direction == sf::Vector2i(-1, 0)) {
			if (pole.nLeft.first)
				nextCheckpoint = pole.nLeft.second;
			else
				nextCheckpoint = pole.pos;
		}
		else if (direction == sf::Vector2i(0, 1)) {
			if (pole.nUp.first)
				nextCheckpoint = pole.nUp.second;
			else
				nextCheckpoint = pole.pos;
		}
		else if (direction == sf::Vector2i(0, -1)) {
			if (pole.nDown.first)
				nextCheckpoint = pole.nDown.second;
			else
				nextCheckpoint = pole.pos;
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

