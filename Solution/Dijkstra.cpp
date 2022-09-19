#include "Dijkstra.h"
#include "World.h"

void Dijkstra::Neighbourhood() {
	for (int i = 0; i < vertices.size(); i++) {
		if (!vertices[i].nDown.first) {	// Find down's neighbour
			int x = vertices[i].pos.x;
			int y = vertices[i].pos.y;
			while (!World::GetInstance()->isColliding(x, y)) {
				y++;
				auto neighbour = std::find(vertices.begin(), vertices.end(), Pole(sf::Vector2i(x, y)));
				if (neighbour != vertices.end())
				{
					vertices[i].nDown.first = true;
					vertices[i].nDown.second = sf::Vector2i(x, y);
					// And assign reciprocal
					neighbour->nUp.first = true;
					neighbour->nUp.second = vertices[i].pos;
					break;
				}
			}
		}
		if (!vertices[i].nUp.first) {	// Find up's neighbour
			int x = vertices[i].pos.x;
			int y = vertices[i].pos.y;
			while (!World::GetInstance()->isColliding(x, y)) {
				y--;
				auto neighbour = std::find(vertices.begin(), vertices.end(), Pole(sf::Vector2i(x, y)));
				if (neighbour != vertices.end())
				{
					vertices[i].nUp.first = true;
					vertices[i].nUp.second = sf::Vector2i(x, y);
					// And assign reciprocal
					neighbour->nDown.first = true;
					neighbour->nDown.second = vertices[i].pos;
					break;
				}
			}
		}
		if (!vertices[i].nLeft.first) {	// Find left's neighbour
			int x = vertices[i].pos.x;
			int y = vertices[i].pos.y;
			while (!World::GetInstance()->isColliding(x, y)) {
				x--;
				auto neighbour = std::find(vertices.begin(), vertices.end(), Pole(sf::Vector2i(x, y)));
				if (neighbour != vertices.end())
				{
					vertices[i].nLeft.first = true;
					vertices[i].nLeft.second = sf::Vector2i(x, y);
					// And assign reciprocal
					neighbour->nRight.first = true;
					neighbour->nRight.second = vertices[i].pos;
					break;
				}
			}
		}
		if (!vertices[i].nRight.first) {// Find right's neighbour
			int x = vertices[i].pos.x;
			int y = vertices[i].pos.y;
			while (!World::GetInstance()->isColliding(x, y)) {
				x++;
				auto neighbour = std::find(vertices.begin(), vertices.end(), Pole(sf::Vector2i(x, y)));
				if (neighbour != vertices.end())
				{
					vertices[i].nRight.first = true;
					vertices[i].nRight.second = sf::Vector2i(x, y);
					// And assign reciprocal
					neighbour->nLeft.first = true;
					neighbour->nLeft.second = vertices[i].pos;
					break;
				}
			}
		}
	}
}

void Dijkstra::Init(sf::Vector2i start) {
	for (auto v : vertices) {
		queue.push_back(v);				// Fill the queue
		distances[v.pos] = INT32_MAX;	// All distances to inifinite
	}
	distances[start] = 0;			// Except start
}

std::pair<bool, Pole> Dijkstra::FindMin() {
	std::pair<bool, Pole> res(false, sf::Vector2i());
	float minDist = INT32_MAX;
	for (auto v : queue) {
		if (minDist > distances[v.pos]) {
			res.first = true;
			res.second = v;
			minDist = distances[v.pos];
		}
	}
	return res;
}

void Dijkstra::relax(sf::Vector2i& s1, sf::Vector2i& s2) {
	float dist_s1s2 = len(s2 - s1);
	if (distances[s2] > distances[s1] + dist_s1s2) {
		distances[s2] = distances[s1] + dist_s1s2;
		pred[s2] = s1;
	}
}

void Dijkstra::compute(sf::Vector2i start) {
	if (computing) return;

	computing = true;
	queue.clear();
	pred.clear();
	distances.clear();
	Init(start);
	while (!queue.empty()) {

		std::pair<bool, Pole> s1 = FindMin();
		if (s1.first == false)
			break;

		// Queue privé de s1
		auto pos = std::find(queue.begin(), queue.end(), s1.second);
		if (pos != queue.end())
			queue.erase(pos);

		// Relax
		relax(s1.second.pos, s1.second.nDown.second);
		relax(s1.second.pos, s1.second.nUp.second);
		relax(s1.second.pos, s1.second.nRight.second);
		relax(s1.second.pos, s1.second.nLeft.second);
	}
	computing = false;

}

sf::Vector2i Dijkstra::FindNearestPole(sf::Vector2i cell) {
	sf::Vector2i res;
	float minDistFromStart = INT32_MAX;
	// Left
	for (int x = cell.x; x >  0; x--) {
		if (World::GetInstance()->isColliding(x, cell.y)) break;
		auto pos = std::find(vertices.begin(), vertices.end(), sf::Vector2i(x, cell.y));
		if (pos != vertices.end()) {
			float length = len(cell - sf::Vector2i(x, cell.y));
			if (length < minDistFromStart) {
				minDistFromStart = length;
				res = sf::Vector2i(x, cell.y);
				break;
			}
		}
	}
	// Right
	for (int x = cell.x; x < 40; x++) {
		auto pos = std::find(vertices.begin(), vertices.end(), sf::Vector2i(x, cell.y));
		if (World::GetInstance()->isColliding(x, cell.y)) break;
		if (pos != vertices.end()) {
			float length = len(cell - sf::Vector2i(x, cell.y));
			if (length < minDistFromStart) {
				minDistFromStart = length;
				res = sf::Vector2i(x, cell.y);
				break;
			}
		}
	}
	// Up
	for (int y = cell.y; y >  0; y--) {
		auto pos = std::find(vertices.begin(), vertices.end(), sf::Vector2i(cell.x, y));
		if (World::GetInstance()->isColliding(cell.x, y)) break;
		if (pos != vertices.end()) {
			float length = len(cell - sf::Vector2i(cell.x, y));
			if (length < minDistFromStart) {
				minDistFromStart = length;
				res = sf::Vector2i(cell.x, y);
				break;
			}
		}
	}
	// Down
	for (int y = cell.y; y < 40; y++) {
		auto pos = std::find(vertices.begin(), vertices.end(), sf::Vector2i(cell.x, y));
		if (World::GetInstance()->isColliding(cell.x, y)) break;
		if (pos != vertices.end()) {
			float length = len(cell - sf::Vector2i(cell.x, y));
			if (length < minDistFromStart) {
				minDistFromStart = length;
				res = sf::Vector2i(cell.x, y);
				break;
			}
		}
	}
	return res;

	//float min	DistFromStart = INT32_MAX;
	//sf::Vector2i res
	//for (auto v : vertices) {
	//	float lenFromStart = sqrt((v.pos - cell).x * (v.pos - cell).x + (v.pos - cell).y * (v.pos - cell).y);
	//	if (v == cell) {
	//		return v.pos;
	//		minDistFromStart = 0;
	//		break;
	//	}
	//	else if (v.pos != cell && minDistFromStart > lenFromStart)
	//	{
	//		minDistFromStart = lenFromStart;
	//		res = v.pos;
	//	}
	//}
	// return res;
}

std::vector<sf::Vector2i> Dijkstra::FindPath(sf::Vector2i start, sf::Vector2i end) {	
	sf::Vector2i startCell	= FindNearestPole(start);
	sf::Vector2i endCell	= FindNearestPole(end);
	compute(startCell);				// Build Dijkstra
	BuildPath(startCell, endCell);	// Construct Path from endCell to startCell
	
	if (start != startCell)
		path.insert(path.begin(), start);
	if (end != endCell)
		path.push_back(end);
	return path;
}

void Dijkstra::BuildPath(sf::Vector2i& startCell, sf::Vector2i& endCell)
{
	path.clear();
	sf::Vector2i s = endCell;
	path.push_back(endCell);
	while (s != startCell) {
		auto pos = pred.find(s);
		if (pos != pred.end()) {
			s = pred[s];
			path.push_back(s);
		}
		else {
			path.clear();
			break;
		}
	}

	std::reverse(path.begin(), path.end());
}

float len(sf::Vector2i s) {
	return sqrt(s.x * s.x + s.y * s.y);
}
