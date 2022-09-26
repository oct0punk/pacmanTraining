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

public:
	bool* drawLines = new bool(false);
	bool* drawPoints = new bool(false);
	bool* drawRightNeighbour = new bool(false);
	bool* drawLeftNeighbour = new bool(false);
	bool* drawDownNeighbour = new bool(false);
	bool* drawUpNeighbour = new bool(false);


	Dijkstra dijkstra;
	int* cellSize = new int(25);

	void Init(sf::RenderWindow* window);

	void LoadMap();

	// Init once, render and update an entity at every frame
	void AddEntity(Entity* e);

	// Remove an entity for the update and the rendering
	void RemoveEntity(Entity* e);

	// Render a wall and collide at pos
	void AddWall(sf::Vector2i pos);

	// Resize cells size
	void ResizeCells(int newSize);

	// Called every frame. Update all entities.
	void Update(double dt);

	// Called at collision between player and ghost
	void GameOver();

	bool isColliding(int cx, int cy);

	sf::Vector2i FindNearestCorrectCell(sf::Vector2i vec);

	// RENDERING

	void DrawMap(sf::RenderWindow* win);
	void DrawWalls(sf::RenderWindow* win);
	void Draw(sf::RenderWindow* win);

	// Singleton
	static World* GetInstance();
};

