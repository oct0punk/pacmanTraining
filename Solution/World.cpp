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


// Does not init entities (call "AddEntity" after)
void World::Init(sf::RenderWindow* window) {
	// set block's properties
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


// Generate Pacman's map
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
			// Replace by switch on buffer[x]
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

void World::RemoveEntity(Entity* e) {
	auto iterator = std::find(entities.begin(), entities.end(), e);
	if (iterator != entities.end())
		entities.erase(iterator);
}


void World::AddWall(sf::Vector2i pos) {
	if (!isColliding(pos.x, pos.y)) {
		blocks.push_back(sf::Vector2i(pos.x, pos.y));
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

void World::GameOver() {
	for (auto e : entities)
		e->Reset();
}

// Return true if there is a collider at sf::Vector2i(cx, cy); else return false
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

// Find the nearest cell without collider
sf::Vector2i World::FindNearestCorrectCell(sf::Vector2i vec) {
	vec.x = clamp(vec.x, 1, 37);
	vec.y = clamp(vec.y, 1, 37);
	int size = 0;
	while (true) {
		for (int x = vec.x - size; x < vec.x + size; x++) {
			if (x > 37 || x < 1) continue;															
			sf::Vector2i res(x, vec.y + size);														
			if (!isColliding(res.x, res.y)) return res;												//			 1 --> 
		}																							//		    ________
		for (int y = vec.y + size; y > vec.y - size; y--) {											//	   ^  |	  size	 |  2
			if (y > 37 || y < 1) continue;															//	   |  |	 _|__|_  |  
			sf::Vector2i res(vec.x + size, y);														//	   |  |	 _|__|_  |  |
			if (!isColliding(res.x, res.y)) return res;												//		  |	  |  |   |  |
		}																							//		4 |__________|  V
		for (int x = vec.x + size; x > vec.x - size; x--) {											//			  <-- 3
			if (x > 37 || x < 1) continue;															
			sf::Vector2i res(x, vec.y - size);														// Size++ after each loop
			if (!isColliding(res.x, res.y)) return res;												
		}																							
		for (int y = vec.y - size; y > vec.y + size - 1; y--) {										
			if (y > 37 || y < 1) continue;															
			sf::Vector2i res(vec.x - size, y);
			if (!isColliding(res.x, res.y)) return res;
		}
		size++;
	}
}

void World::Draw(sf::RenderWindow* win) {
	DrawMap(win);
	DrawWalls(win);

	// Draw Entities
	for (auto e : entities)
		e->Draw(win);

	sf::VertexArray arr;
	arr.setPrimitiveType(sf::PrimitiveType::LinesStrip);
}

void World::DrawMap(sf::RenderWindow* win) {
	if (*drawLines)
		win->draw(lines);
	if (*drawPoints)
		win->draw(points);
}

void World::DrawWalls(sf::RenderWindow* win) {
	for (auto w : blocks) {
		sf::RectangleShape rect(*block);
		rect.setPosition(sf::Vector2f(w.x * *cellSize, w.y * *cellSize));
		win->draw(rect);
	}
	for (auto w : poles) {
		sf::RectangleShape rect(*pole);
		rect.setPosition(sf::Vector2f(w.x * *cellSize, w.y * *cellSize));
		win->draw(rect);
	}

	if (*drawRightNeighbour) {
		sf::VertexArray arr;
		arr.setPrimitiveType(sf::PrimitiveType::Lines);
		for (auto p : dijkstra.vertices) {
			if (!p.nRight.first) continue;
			sf::Vertex v;
			v.position.x = (p.pos.x + .5f) * *cellSize;
			v.position.y = (p.pos.y + .5f) * *cellSize;
			v.color = sf::Color::Transparent;
			arr.append(v);
			v.position.x = (p.nRight.second.x + .5f) * *cellSize;
			v.position.y = (p.nRight.second.y + .5f) * *cellSize;
			v.color = sf::Color::White;
			arr.append(v);
		}
		win->draw(arr);
	}
	if (*drawLeftNeighbour) {
		sf::VertexArray arr;
		arr.setPrimitiveType(sf::PrimitiveType::Lines);
		for (auto p : dijkstra.vertices) {
			if (!p.nLeft.first) continue;
			sf::Vertex v;
			v.position.x = (p.pos.x + .5f) * *cellSize;
			v.position.y = (p.pos.y + .5f) * *cellSize;
			v.color = sf::Color::Transparent;
			arr.append(v);
			v.position.x = (p.nLeft.second.x + .5f) * *cellSize;
			v.position.y = (p.nLeft.second.y + .5f) * *cellSize;
			v.color = sf::Color::White;
			arr.append(v);
		}
		win->draw(arr);
	}
	if (*drawUpNeighbour) {
		sf::VertexArray arr;
		arr.setPrimitiveType(sf::PrimitiveType::Lines);
		for (auto p : dijkstra.vertices) {
			if (!p.nUp.first) continue;
			sf::Vertex v;
			v.position.x = (p.pos.x + .5f) * *cellSize;
			v.position.y = (p.pos.y + .5f) * *cellSize;
			v.color = sf::Color::Transparent;
			arr.append(v);
			v.position.x = (p.nUp.second.x + .5f) * *cellSize;
			v.position.y = (p.nUp.second.y + .5f) * *cellSize;
			v.color = sf::Color::White;
			arr.append(v);
		}
		win->draw(arr);
	}
	if (*drawDownNeighbour) {
		sf::VertexArray arr;
		arr.setPrimitiveType(sf::PrimitiveType::Lines);
		for (auto p : dijkstra.vertices) {
			if (!p.nDown.first) continue;
			sf::Vertex v;
			v.position.x = (p.pos.x + .5f) * *cellSize;
			v.position.y = (p.pos.y + .5f) * *cellSize;
			v.color = sf::Color::Transparent;
			arr.append(v);
			v.position.x = (p.nDown.second.x + .5f) * *cellSize;
			v.position.y = (p.nDown.second.y + .5f) * *cellSize;
			v.color = sf::Color::White;
			arr.append(v);
		}
		win->draw(arr);
	}

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
