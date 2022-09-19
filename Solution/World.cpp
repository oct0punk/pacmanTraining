#pragma once
#include "World.h"

World* World::Instance = nullptr;

sf::VertexArray World::GenerateMapOrigins() {
	sf::VertexArray points;
	for (int x = *cellSize / 2; x < 1000; x += *cellSize)
		for (int y = *cellSize / 2; y < 900; y += *cellSize)
			points.append(sf::Vertex(sf::Vector2f(x, y)));

	return points;
}

sf::VertexArray World::GenerateMapGrid() {
	sf::VertexArray points;
	points.setPrimitiveType(sf::PrimitiveType::Lines);
	for (int x = 0; x < 1000; x += *cellSize) {
		points.append(sf::Vector2f(x, 0));
		points.append(sf::Vector2f(x, 1000));
	}
	for (int y = 0; y < 9000; y += *cellSize) {
		points.append(sf::Vector2f(0, y));
		points.append(sf::Vector2f(1000, y));
	}
	return points;
}

void World::Init(sf::RenderWindow* window) {
	block = new sf::RectangleShape(sf::Vector2f(*cellSize, *cellSize));
	block->setFillColor(sf::Color::Transparent);
	block->setOutlineColor(sf::Color::Cyan);
	block->setOutlineThickness(-*cellSize / 10);

	pole = new sf::RectangleShape(sf::Vector2f(*cellSize, *cellSize));
	pole->setFillColor(sf::Color::Transparent);
	pole->setOutlineColor(sf::Color(130, 130, 130));
	pole->setOutlineThickness(-*cellSize / 10);

	lines = GenerateMapGrid();
	points = GenerateMapOrigins();

	win = window;


	LoadMap();
}

void World::LoadMap()
{
	dijkstra.vertices.clear();
	FILE* f;
	char buffer[42];
	fopen_s(&f, "res/map.txt", "rb");
	int y = 0;
	while (!feof(f)) {
		fread(buffer, sizeof(char), 42, f);
		for (int x = 0; x < 40; x++) {
			bool isWall =
				buffer[x] == (char)*"B";
			if (isWall) {
				AddWall(sf::Vector2i(x, y));
			}
			else
				if (buffer[x] == (char)*"P") {
					poles.push_back(sf::Vector2i(x, y));
					dijkstra.vertices.push_back(Pole(sf::Vector2i(x, y)));
				}
		}
		y++;
	}
	fclose(f);
	dijkstra.Neighbourhood();
}

void World::AddEntity(Entity* e) {
	entities.push_back(e);
	e->cellSize = cellSize;
	e->Init();	
	Character* c = (Character*)e;
	if (c) c->dij = dijkstra;		
}


void World::AddWall(sf::Vector2i pos) {
	if (!isColliding(pos.x, pos.y)) {
		blocks.push_back(sf::Vector2i(pos.x, pos.y));			// Render a block
		points = GenerateMapOrigins();
	}
}

void World::ResizeCells(int newSize) {
	*cellSize = newSize;
	block->setSize(sf::Vector2f(*cellSize, *cellSize));
	block->setOutlineThickness(-*cellSize / 5);
	lines = GenerateMapGrid();
	points = GenerateMapOrigins();
}

void World::Update(double dt) {
	//bombTimer -= dt * 0.0f;
	//if (bombTimer < 0) {
	//	bombTimer = 1;
	//	sf::CircleShape bombShape(20);
	//	bombShape.setFillColor(sf::Color::Yellow);
	//	bombShape.setOutlineColor(sf::Color::Red);
	//	bombShape.setOutlineThickness(5);
	//	Bomb b(new sf::CircleShape(bombShape));
	//	b.rx = b.ry = .5f;
	//	b.cx = rand() % 38 + 1;
	//	b.cy = rand() % 38 + 1;
	//	AddEntity(new Bomb (b));
	//}
	//std::cout << bombTimer;
	//std::cout << "\n";

	for (auto e : entities) {
		e->Update(dt);
	}

	return;
	// IMGUI
	ImGui::Begin("World");
	ImGui::Checkbox("Draw Lines", drawLines);
	ImGui::Checkbox("Draw Points", drawPoints);
	ImGui::End();
}

bool World::isColliding(int cx, int cy) {
	if (cx < 0) return false;
	if (cy < 0) return false;
	if (cx > 1000) return false;
	if (cy > 1000) return false;

	for (auto b : blocks) {
		if (b.x == cx && b.y == cy)
			return true;
	}
	return false;
}

void World::Draw(sf::RenderWindow* win) {
	DrawMap(win);
	DrawWalls(win);

	// Draw Entities
	for (auto e : entities) {
		e->Draw(win);
	}

	sf::VertexArray arr;
	arr.setPrimitiveType(sf::PrimitiveType::LinesStrip);
}

World* World::GetInstance() {
	if (World::Instance)
		return World::Instance;
	else {
		World* w = new World();
		World::Instance = w;
		return w;
	}
}


