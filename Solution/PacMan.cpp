#pragma once
#include <SFML/Graphics.hpp>
#include "World.h"
#include <iostream>


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
	Ghost* blinky = new Ghost(new sf::CircleShape(cGhost), pacman,	{
			sf::Vector2i(37, 1),
			sf::Vector2i(20, 1),
			sf::Vector2i(20, 6),
			sf::Vector2i(37, 6)									});
	blinky->cx = 37;
	blinky->cy = 1;
	blinky->rx = blinky->ry = .5f;

	cGhost.setFillColor(sf::Color::Magenta);
	Pinky* pinky = new Pinky(new sf::CircleShape(cGhost), pacman, { 
		sf::Vector2i(1, 1), 
		sf::Vector2i(1, 6), 
		sf::Vector2i(9, 6), 
		sf::Vector2i(9, 1),										});
	pinky->cx = 1;
	pinky->cy = 1;
	pinky->rx = pinky->ry = .5f;

	cGhost.setFillColor(sf::Color(255, 200, 0));
	Clyde* clyde = new Clyde(new sf::CircleShape(cGhost), pacman, {
		sf::Vector2i(19, 15),
		sf::Vector2i(13, 15),
		sf::Vector2i(13, 23),
		sf::Vector2i(19, 23),
		});
	clyde->cx = 1;
	clyde->cy = 38;


	cGhost.setFillColor(sf::Color::Blue);
	cGhost.setOutlineThickness(2);
	Inky* inky = new Inky(new sf::CircleShape(cGhost), pacman, blinky,	{
		sf::Vector2i(37, 38),
		sf::Vector2i(37, 31),
		sf::Vector2i(32, 31),
		sf::Vector2i(32, 35),
		sf::Vector2i(21, 35),
		sf::Vector2i(21, 38),
		});
	inky->cx = 37;
	inky->cy = 38;

	sf::RenderWindow window(sf::VideoMode(1000,1000), "PacMan");
	window.setFramerateLimit(60);
	window.setVerticalSyncEnabled(true);
	World* world = World::GetInstance();

	world->Init(&window);
	world->AddEntity(pacman);
	world->AddEntity(blinky);
	world->AddEntity(pinky);
	world->AddEntity(inky);
	world->AddEntity(clyde);

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
			if (event.type == event.KeyPressed && sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
				std::cout << pacman->cx;
				std::cout << " - ";
				std::cout << pacman->cy;
				std::cout << "\n";
			}
		}

#pragma region UPDATE

		ImGui::SFML::Update(window, deltaClock.restart());
		float dt = deltaClock.getElapsedTime().asSeconds();

		ImGui::Begin("Debug");
		ImGui::Checkbox("Player's prediction", pacman->drawPrediction);
		ImGui::Checkbox("Inky's correction", inky->drawCellCorrection);
		ImGui::Checkbox("Clyde's destination", clyde->drawDestination);
		ImGui::Checkbox("Right neighbour", world->drawRightNeighbour);
		ImGui::Checkbox("Left neighbour", world->drawLeftNeighbour);
		ImGui::Checkbox("Up neighbour", world->drawUpNeighbour);
		ImGui::Checkbox("Down neighbour", world->drawDownNeighbour);
		ImGui::End();

		world->Update(dt);

#pragma endregion


#pragma region RENDER

		window.clear();
		world->Draw(&window);

		
		ImGui::SFML::Render(window);
		window.display();
#pragma endregion

	}
	ImGui::SFML::Shutdown();
	return 0;
}
