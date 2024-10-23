#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include "Vec2.h";
#include "SpartialGrid.h"

struct BoidParam {
	float visualRange = 20, protectRange = 4, matchFac = 20, centerFac = 1600, avoidFac = 160, turnFac = 400;
	Vec2 startTurnRange = Vec2(3, 3);
	bool turnWhenOutOfBounds = true, mirrorToCenter = false, collideOnEdges = false;
};

struct ParticleDirection {
	bool allDirections = true;
	bool north = false;
	bool south = false;
	bool east = false;
	bool west = false;

	bool trueNorth = false;
	bool trueSouth = false;
	bool trueEast = false;
	bool trueWest = false;
};

class ParticleSystem
{
	struct Particle
	{
		Vec2 position;
		Vec2 velocity;
		int totalLifetime;
		int lifetime;
		float speed;
		float angle;
	};
	std::random_device		rd;
	std::mt19937			gen;
	sf::Clock				initializationClock;
	float					initializationDuration = 1.0f;
	BoidParam				m_boidParam;
	ParticleDirection		m_particleDirection;
	std::vector<Particle>	m_particles;
	sf::VertexArray			m_vertices;
	float					m_radius = 2;
	float					m_startSpeed = 2;
	float					m_turnSpeed = 0.05f;
	float					m_dragSpeed = 0.05f;
	float					m_size = 8;
	float					m_defaultSize = 8;
	size_t					m_defaultCount = 1024;
	size_t					m_count = 1024;
	Vec2					m_iPosition;
	std::string				m_shapeType = "rectangle";
	SpartialGrid			m_spartialGrid;
	Vec2					m_speedRange = Vec2(-2, 2);
	Vec2					m_lifeRange = Vec2(30, 100);
	Vec2					m_boundary = Vec2(800, 600);
	Vec2					m_spanningborder = Vec2(100, 50);
	sf::Color				m_color = sf::Color::Red;
	sf::Color				m_firstColor = sf::Color::Red;
	sf::Color				m_secondColor = sf::Color::White;
	sf::Texture				m_texture;
	float					m_adjustmentAngle = 102.12f;
	bool					m_useTexture = false;
	void					move(const size_t index);
	void					boxSpanning(const size_t index, const bool first = false);
	void					createBoxShape(const size_t index, const Vec2& pos);
	void					updateBoxShape(const size_t index);
	void					createTriangleShape(const size_t index, const Vec2& pos);
	void					updateTriangleShape(const size_t index);
	void					rotate(const size_t index, const float theta, const int noOfSides = 6);
	void					colorParticle(const size_t index, const int noOfSides = 6, const int r = 255, const int g = 255, const int b = 255, const int a = 255);
	void					updateOpacityByLifespan(const size_t index, const int noOfSides = 6);
	void					textureParticle(const size_t index);
	void					resetParticle(const size_t index, const bool first = false);
	void boidAlg(const size_t index, const float visualRange, const float protectedRange, const float matchingFactor = 1.0f, const float centeringFactor = 1.0f, const float avoidFactor = 1.0f);
public:
	ParticleSystem(const Vec2& ipos = Vec2(), const float s = 8, const size_t count = 1024, const Vec2& b_dry = Vec2(800,600), const float spSize = 50.0f);
	ParticleSystem(const ParticleSystem& other);
	ParticleSystem& operator = (const ParticleSystem& other);
	bool lifespan = true;
	bool targetTracking = false;
	bool circularMovement = false;
	bool randomSpeed = true;
	bool boid = false;
	bool randomColor = false;
	bool fadeOverLifespan = false;
	std::pair<Vec2, std::string> targetPair = {Vec2(), "seek"};
	void resetParticles(size_t count = 1024, float size = 8);
	void update();
	void draw(sf::RenderWindow& window) const;
	void changeShape(const std::string& shape);
	void setSpeed(const float speed);
	void setRadius(const float radius);
	void setTurnSpeed(const float turnSpeed);
	bool isOutOfBounds(const Vec2& pos) const;
	void maxBounds(const size_t index);
	void outOfBoundsSteerTurnAway(const size_t index, const float turnSpeed);
	void mirrorBoundary(const size_t index);
	void setTexture(const sf::Texture& texture);
	void setColors(const sf::Color& firstcolor, const sf::Color& secondcolor);
	void setColor(const sf::Color& color);
	void setTarget(const Vec2& target, const std::string& type);
	void setLifeRange(const Vec2& lRange);
	void setSpeedRange(const Vec2& sRange);
	float getAdjustmentAngle();
	void setAdjustmentAngle(const float angle);
	void setSpanningBorder(const Vec2& size);
	ParticleDirection& getParticleDirection();
	BoidParam& getBoidParam();
};
