#pragma once
#include <unordered_map>
#include <vector>
#include "SFML/Window.hpp"

namespace std {
	template <>	struct hash<sf::Vector2i> {
		std::size_t operator()(const sf::Vector2i& k) const {
			using std::size_t;
			using std::hash;
			using std::string;
			return k.x | (k.y << 12);
		}
	};
}

float len(sf::Vector2i s);

struct Pole {
	sf::Vector2i pos;
	std::pair<bool, sf::Vector2i> nDown;
	std::pair<bool, sf::Vector2i> nUp;
	std::pair<bool, sf::Vector2i> nLeft;
	std::pair<bool, sf::Vector2i> nRight;

	Pole(sf::Vector2i position) {
		pos = position;
	}

	bool operator ==(Pole other) {
		return pos == other.pos;
	}
};

class Dijkstra {
	bool computing = false;
	std::unordered_map<sf::Vector2i, float> distances;
	std::unordered_map <sf::Vector2i, sf::Vector2i> pred;
	std::vector<Pole> queue;
	std::vector<sf::Vector2i> path;

	void compute(sf::Vector2i start);
	void Init(sf::Vector2i start);
	// Find the nearest vertices in queue
	std::pair<bool, Pole> FindMin();
	void relax(sf::Vector2i& s1, sf::Vector2i& s2);

public:
	std::vector<Pole> vertices;

	// Procedurally set all poles neighbours
	void Neighbourhood();
	
	// Find the nearest pole at up, down, left or right
	Pole FindNearestNeighbourPole(sf::Vector2i cell);
	
	// Build path from start's nearest pole to end's nearest pole 
	std::vector<sf::Vector2i> FindPath(sf::Vector2i start, sf::Vector2i end);

	// Set dijkstra's path from endCell to startCell
	void BuildPath(sf::Vector2i& endCell, sf::Vector2i& startCell);
};
