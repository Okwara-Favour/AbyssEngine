#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "lineSegment.h"
class LightSource
{
	Vec2 lightSource;
	Vec2 windowSize;
	Vec2 canvasSize;
	float extent;
	sf::Texture texture;
	std::vector<lineSegment> linesegments;
	sf::Color color = sf::Color(255, 255, 255);
	sf::VertexArray vertices;
	sf::CircleShape circle;
	int lines;
	float startAngle;
	float endAngle;
	float defaultStartAngle;
	float defaultEndAngle;
	bool useTexture = true;
	void makeCircle();
	void makeLinesegments();
	void makePolygons();
	void cutLineOnIntersect(lineSegment& pointer, Intersect& intersect, Vec2& minIntersect);
public:
	LightSource(const Vec2& lpos = Vec2(0, 0), const Vec2& wSize = Vec2(400, 400),
		const float e = 200,
		const sf::Texture& t = sf::Texture(),
		const sf::Color& color = sf::Color(255, 255, 255),
		const int n = 1, const float sAngle = 0.0f, const float eAngle = 360.0f);

	void enableOrDisableTexture(const bool useT) { useTexture = useT; }
	void setTexture(const sf::Texture& t);
	void setLightSource(const Vec2& lpos);
	void setExtent(float e);
	void setLines(int n);
	void setDefaultAngles(float sAngle, float eAngle);
	void setAngles(float sAngle, float eAngle);
	void setAngle(float angle);
	const int						getLines() const { return lines; }
	const float						getExtent() const { return extent; }
	const std::pair<float, float>	getDefaultAngle() const { return { defaultStartAngle, defaultEndAngle }; }
	sf::Color&						getColor() { return color; }
	const Vec2&						getLightSource() const { return lightSource; }
	void reMake();
	void drawPolygons(sf::RenderWindow& window, const bool withTexture = true) const;
	void drawCircle(sf::RenderWindow& window, const bool withTexture = true) const;
	void rectIntersect(const Vec2& pos, const Vec2& size);
};

