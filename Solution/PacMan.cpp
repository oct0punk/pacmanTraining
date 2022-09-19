#pragma once
#include <SFML/Graphics.hpp>
#include "World.h"
#include <iostream>
#include <chrono>
#include <algorithm>
#include <math.h>


int main()
{
	sf::CircleShape circle(10);
	circle.setFillColor(sf::Color::Yellow);
	circle.setOrigin(10, 10);
	Player* pacman = new Player(&circle);
	pacman->cx = 19;
	pacman->cy = 25;
	pacman->rx = pacman->ry = .5f;
	sf::CircleShape cGhost(10);
	cGhost.setOrigin(10, 10);
	cGhost.setFillColor(sf::Color::Red);
	Ghost* g1 = new Ghost(new sf::CircleShape(cGhost), pacman);
	g1->cx = 1;
	g1->cy = 1;
	g1->rx = g1->ry = .5f;

	sf::RenderWindow window(sf::VideoMode(1000,1000), "PacMan");
	World* world = World::GetInstance();

	world->Init(&window);
	world->AddEntity(pacman);
	world->AddEntity(g1);

	ImGui::SFML::Init(window);
	int* cellSize = world->cellSize;

	sf::Clock deltaClock;
	while (window.isOpen()) {

		sf::Event event;
		while (window.pollEvent(event)) {
			ImGui::SFML::ProcessEvent(event);
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == event.KeyPressed && sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
				pacman->desiredDir.x = -1;
				pacman->desiredDir.y = 0;
			}
			if (event.type == event.KeyPressed && sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
				pacman->desiredDir.x = 1;
				pacman->desiredDir.y = 0;
			}
			if (event.type == event.KeyPressed && sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
				pacman->desiredDir.x = 0;
				pacman->desiredDir.y = 1;
			}
			if (event.type == event.KeyPressed && sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
				pacman->desiredDir.x = 0;
				pacman->desiredDir.y = -1;
			}
		}

#pragma region UPDATE

		ImGui::SFML::Update(window, deltaClock.restart());
		float dt = deltaClock.getElapsedTime().asSeconds();
		world->Update(dt);

#pragma endregion

#pragma region RENDER

		window.clear();
		world->Draw(&window);

		// Pathfinding
		// sf::VertexArray dijkstraArr;
		// dijkstraArr.setPrimitiveType(sf::PrimitiveType::Lines);
		// for (auto s : g1->nextPos) {
		// 	sf::Vector2f first((s.x + .5f) * *cellSize, (s.y + .5f) * *cellSize);
		// 	sf::Vertex v1(first);
		// 	v1.color = sf::Color(255, 255, 0);
		// 	dijkstraArr.append(v1);
		// }
		// window.draw(dijkstraArr);
		
		ImGui::SFML::Render(window);
		window.display();

#pragma endregion
	}
	ImGui::SFML::Shutdown();
	return 0;
}
