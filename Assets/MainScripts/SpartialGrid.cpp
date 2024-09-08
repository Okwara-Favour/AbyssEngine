#include "SpartialGrid.h"
#include <iostream>

SpartialGrid::SpartialGrid(const int width, const int height, const float cellSize)
	:sCellSize(cellSize)
{
	sWidth = width / cellSize;
	sHeight = height / cellSize;
	for (int row = 0; row < sHeight; row++)
	{
		std::vector<SpartialCell> temp;
		for (int col = 0; col < sWidth; col++)
		{
			temp.push_back(SpartialCell());
		}
		spartialArray.push_back(temp);
	}
}

void SpartialGrid::add(const size_t index, const Vec2& pos)
{
	int col = pos.x / sCellSize;
	int row = pos.y / sCellSize;

	if (col < 0 || col >= sWidth || row < 0 || row >= sHeight) return;
	spartialArray[row][col].storage.push_back(index);
}

void SpartialGrid::clear()
{
	for (int row = 0; row < sHeight; row++)
	{
		for (int col = 0; col < sWidth; col++)
		{
			spartialArray[row][col].storage.clear();
		}
	}
}

std::vector<size_t> SpartialGrid::getNeighbours(const Vec2& pos, const float range)
{
	int pcol = pos.x / sCellSize;
	int prow = pos.y / sCellSize;
	
	int cellRange = std::ceil(range / sCellSize);
	std::vector<size_t> neightbours;

	for (int row = std::max(0, prow - cellRange); row <= std::min(sHeight - 1, prow + cellRange); row++)
	{
		for (int col = std::max(0, pcol - cellRange); col <= std::min(sWidth - 1, pcol + cellRange); col++)
		{
			neightbours.insert(neightbours.end(), spartialArray[row][col].storage.begin(), spartialArray[row][col].storage.end());
		}
	}
	return neightbours;
}