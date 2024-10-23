#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include "Vec2.h"
struct Color
{
	int m_r = 0, m_g = 0, m_b = 0;
	Color(int r, int g, int b) :m_r(r), m_g(g), m_b(b) {}
	Color() {}
	sf::Color getColor() { return sf::Color(m_r, m_g, m_b); }
	const bool equals(const Color& other) const
	{
		return m_r == other.m_r && m_g == other.m_g && m_b == other.m_b;
	}
};

class Cell
{
	Vec2 size;
	Vec2 pos;
public:
	Color color;
	Cell(const Vec2& p, const Vec2& s);
	void draw(sf::RenderWindow& window);
	const Vec2 getSize() const;
	const Vec2 getPosition() const;
};

class Grid
{
	Vec2 m_size;
	Vec2 m_cellSize;
	bool insideViewBounds(const Cell& cell, const sf::View& view);
	bool outOfViewBounds(const sf::View& view, const Vec2& targetPos);
	void init();
public:
	std::vector<std::vector<Cell>> m_grid;
	Grid();
	Grid(const Vec2& size, const Vec2& cellsize);
	Cell& get(int col, int row);
	const Vec2& size();
	const Vec2& cellSize();
	const bool notInBounds(int col, int row) const;
	void render(sf::RenderWindow& window, bool fullScreen = true);
};

