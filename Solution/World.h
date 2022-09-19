#pragma once
#include <vector>
#include "Entity.h"
#include <array>
#include <unordered_map>
#include "Dijkstra.h"


class World {	
private:
	static World* Instance;
	sf::RenderWindow* win = nullptr;
	std::vector<Entity*> entities;

	sf::VertexArray lines;
	sf::VertexArray points;
	sf::VertexArray GenerateMapOrigins();
	sf::VertexArray GenerateMapGrid();

	sf::RectangleShape* block = nullptr;
	sf::RectangleShape* pole = nullptr;
	std::vector<sf::Vector2i> blocks;
	std::vector<sf::Vector2i> poles;

	bool* drawLines = new bool(false);
	bool* drawPoints = new bool(false);

	float bombTimer = 1;

public:
	Dijkstra dijkstra;
	int* cellSize = new int(25);

	void Init(sf::RenderWindow* window);

	void LoadMap();

	void AddEntity(Entity* e);
	void RemoveEntity(Entity* e) {
		auto iterator = std::find(entities.begin(), entities.end(), e);
		if (iterator != entities.end())
			entities.erase(iterator);
	}

	void AddWall(sf::Vector2i pos);

	void ResizeCells(int newSize);


	void Update(double dt);

	bool isColliding(int cx, int cy);

#pragma region Rendering

	void DrawMap(sf::RenderWindow* win) {		
		if (*drawLines)
			win->draw(lines);
		if (*drawPoints)
			win->draw(points);
	}

	void DrawWalls(sf::RenderWindow* win) {
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
	}

	

	void Draw(sf::RenderWindow* win);

#pragma endregion

	static World* GetInstance();

};

