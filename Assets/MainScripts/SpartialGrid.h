#pragma once
#include <vector>
#include "Vec2.h"
class SpartialGrid
{
	struct SpartialCell
	{
		std::vector<size_t> storage;
	};
	std::vector<std::vector<SpartialCell>> spartialArray;
	int sWidth = 0;
	int sHeight = 0;
	float sCellSize = 0;
public:
	SpartialGrid(const int width = 0, const int height = 0, const float cellSize = 0);
	void add(const size_t index, const Vec2& pos);
	std::vector<size_t> getNeighbours(const Vec2& pos, const float range);
	void clear();
};

