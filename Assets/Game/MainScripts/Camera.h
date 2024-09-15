#pragma once
#include <SFML/Graphics.hpp>
#include "Vec2.h"
class Camera
{
	Vec2		m_winSize;
	Vec2		m_cellPos;
	Vec2		m_cellSize;
	Vec2		m_cameraPos;
	Vec2		m_gridSize;
public:
	sf::View	m_view;
	Camera(const Vec2& winSize, const Vec2& p, const Vec2& s, const Vec2& gs);
	Camera();
	void createCellView(const Vec2& pos);
	void createFullView();
	void followEntity(const Vec2& pos, const Vec2& size, const Vec2& minBounds, const Vec2& maxBounds);
	const Vec2& cameraCellSize() const;
	const bool outOfCameraBounds(const Vec2& pos) const;
};

