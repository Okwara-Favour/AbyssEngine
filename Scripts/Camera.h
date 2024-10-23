#pragma once
#include <SFML/Graphics.hpp>
#include "Vec2.h"

class Display;
class Camera
{
	friend Display;
	sf::RectangleShape m_rectangle;
	sf::View	m_view;
	Vec2		m_scale = Vec2(1, 1);
	sf::Color	m_color = sf::Color(100,100,255);
public:
	Camera(const Vec2& pos = Vec2(), const Vec2& size = Vec2(1280,720));
	Vec2 getSize();
	const Vec2& getScale();
	Vec2 getPosition();
	const sf::Color& getColor();
	float getAngle();

	void setPosition(float x, float y);
	void setScale(float x, float y);
	void setSize(float x, float y);
	void setPosition(const Vec2& p);
	void setScale(const Vec2& s);
	void setSize(const Vec2& s);
	void setAngle(float angle);
	void setColor(int r = 0, int g = 0, int b = 0, int a = 255);

	const bool outOfCameraBounds(const Vec2& pos) const;
	void followEntity(const Vec2& pos);
	void followEntity(const Vec2& pos, const Vec2& minBounds, const Vec2& maxBounds);
	void apply(sf::RenderTexture& texture) const;
	void apply(sf::RenderWindow& window, const Vec2& pos = Vec2(), const Vec2& size = Vec2(1.0,1.0));

	static void Lua(sol::state& lua);
};

