#include "ParticleSystem.h"
#include "Physics.h"
#include <iostream>

ParticleSystem::ParticleSystem(const Vec2& ipos, const float s, const size_t count, const Vec2& b_dry, const float spSize)
	:m_iPosition(ipos), m_size(s), m_count(count), m_boundary(b_dry)
{
	gen = std::mt19937(rd());
	m_spartialGrid = SpartialGrid(m_boundary.x, m_boundary.y, spSize);
}

ParticleSystem::ParticleSystem(const ParticleSystem& other)
	: initializationClock(), initializationDuration(other.initializationDuration),
	m_boidParam(other.m_boidParam), m_particleDirection(other.m_particleDirection),
	m_particles(other.m_particles), m_vertices(other.m_vertices),
	m_radius(other.m_radius), m_startSpeed(other.m_startSpeed),
	m_turnSpeed(other.m_turnSpeed), m_dragSpeed(other.m_dragSpeed),
	m_size(other.m_size), m_defaultSize(other.m_defaultSize),
	m_defaultCount(other.m_defaultCount), m_count(other.m_count),
	m_iPosition(other.m_iPosition), m_shapeType(other.m_shapeType),
	m_spartialGrid(other.m_spartialGrid), m_speedRange(other.m_speedRange),
	m_lifeRange(other.m_lifeRange), m_boundary(other.m_boundary),
	m_spanningborder(other.m_spanningborder), m_color(other.m_color),
	m_firstColor(other.m_firstColor), m_secondColor(other.m_secondColor),
	m_texture(other.m_texture), m_adjustmentAngle(other.m_adjustmentAngle),
	m_useTexture(other.m_useTexture), lifespan(other.lifespan),
	targetTracking(other.targetTracking), circularMovement(other.circularMovement),
	randomSpeed(other.randomSpeed), boid(other.boid),
	randomColor(other.randomColor), fadeOverLifespan(other.fadeOverLifespan),
	targetPair(other.targetPair)
{}

ParticleSystem& ParticleSystem::operator = (const ParticleSystem & other)
{
	initializationClock = sf::Clock(), initializationDuration = other.initializationDuration,
	m_boidParam = other.m_boidParam, m_particleDirection = other.m_particleDirection,
	m_particles = other.m_particles, m_vertices = other.m_vertices,
	m_radius = other.m_radius, m_startSpeed = other.m_startSpeed,
	m_turnSpeed = other.m_turnSpeed, m_dragSpeed = other.m_dragSpeed,
	m_size = other.m_size, m_defaultSize = other.m_defaultSize,
	m_defaultCount = other.m_defaultCount, m_count = other.m_count,
	m_iPosition = other.m_iPosition, m_shapeType = other.m_shapeType,
	m_spartialGrid = other.m_spartialGrid, m_speedRange = other.m_speedRange,
	m_lifeRange = other.m_lifeRange, m_boundary = other.m_boundary,
	m_spanningborder = other.m_spanningborder, m_color = other.m_color,
	m_firstColor = other.m_firstColor, m_secondColor = other.m_secondColor,
	m_texture = other.m_texture, m_adjustmentAngle = other.m_adjustmentAngle,
	m_useTexture = other.m_useTexture, lifespan = other.lifespan,
	targetTracking = other.targetTracking, circularMovement = other.circularMovement,
	randomSpeed = other.randomSpeed, boid = other.boid,
	randomColor = other.randomColor, fadeOverLifespan = other.fadeOverLifespan,
	targetPair = other.targetPair;
	return *this;
}

void ParticleSystem::resetParticles(size_t count, float size)
{
	if (count != m_defaultCount) m_count = count;
	if (size != m_defaultSize) m_size = size;
	m_particles = std::vector<Particle>(m_count);
	if (m_shapeType == "rectangle" || m_useTexture) m_vertices = sf::VertexArray(sf::Triangles, 6 * m_count);
	else m_vertices = sf::VertexArray(sf::Triangles, 3 * m_count);

	for (size_t pIndex = 0; pIndex < m_particles.size(); pIndex++)
	{
		resetParticle(pIndex, true);
		if(boid) m_spartialGrid.add(pIndex, m_particles[pIndex].position);
	}
}


void ParticleSystem::boxSpanning(const size_t index, const bool first)
{
	Vec2 boxMinRange = m_iPosition - m_spanningborder;
	Vec2 boxMaxRange = m_iPosition + m_spanningborder;
	std::uniform_real_distribution<> disX(boxMinRange.x, boxMaxRange.x);
	std::uniform_real_distribution<> disY(boxMinRange.y, boxMaxRange.y);

	float posX = disX(gen);
	float posY = disY(gen);

	//std::cout << posX << " " << posY << std::endl;
	int no_of_sides = 3;
	if (m_shapeType == "rectangle" || m_useTexture) {
		createBoxShape(index, Vec2(posX, posY));
		no_of_sides = 6;
	}
	else createTriangleShape(index, Vec2(posX, posY));

	sf::Color color = m_color;
	if (randomColor)
	{
		std::uniform_real_distribution<> disR(m_firstColor.r, m_secondColor.r);
		std::uniform_real_distribution<> disG(m_firstColor.g, m_secondColor.g);
		std::uniform_real_distribution<> disB(m_firstColor.b, m_secondColor.b);
		color = sf::Color(disR(gen), disG(gen), disB(gen));
	}
	if (first && lifespan) color.a = 0;
	if (!m_useTexture) colorParticle(index, no_of_sides, color.r, color.g, color.b);
	else textureParticle(index);
}

void ParticleSystem::updateTriangleShape(const size_t index)
{
	auto& pos = m_particles[index].position;
	m_vertices[3 * index + 0].position = sf::Vector2f(pos.x, pos.y - m_size / 2);
	m_vertices[3 * index + 1].position = sf::Vector2f(pos.x + m_size / 2, pos.y + m_size / 2);
	m_vertices[3 * index + 2].position = sf::Vector2f(pos.x - m_size / 2, pos.y + m_size / 2);
}

void ParticleSystem::createTriangleShape(const size_t index, const Vec2& pos)
{
	m_vertices[3 * index + 0].position = sf::Vector2f(pos.x + m_size / 2, pos.y);
	m_vertices[3 * index + 1].position = sf::Vector2f(pos.x + m_size, pos.y + m_size);
	m_vertices[3 * index + 2].position = sf::Vector2f(pos.x, pos.y + m_size);
	m_particles[index].position = pos + m_size / 2;
}

void ParticleSystem::updateBoxShape(const size_t index)
{
	auto& pos = m_particles[index].position;
	m_vertices[6 * index + 0].position = sf::Vector2f(pos.x - m_size / 2, pos.y - m_size / 2);
	m_vertices[6 * index + 1].position = sf::Vector2f(pos.x + m_size / 2, pos.y + m_size / 2);
	m_vertices[6 * index + 2].position = sf::Vector2f(pos.x - m_size / 2, pos.y + m_size / 2);
	m_vertices[6 * index + 3].position = sf::Vector2f(pos.x - m_size / 2, pos.y - m_size / 2);
	m_vertices[6 * index + 4].position = sf::Vector2f(pos.x + m_size / 2, pos.y - m_size / 2);
	m_vertices[6 * index + 5].position = sf::Vector2f(pos.x + m_size / 2, pos.y + m_size / 2);
}

void ParticleSystem::createBoxShape(const size_t index, const Vec2& pos)
{
	m_vertices[6 * index + 0].position = sf::Vector2f(pos.x, pos.y);
	m_vertices[6 * index + 1].position = sf::Vector2f(pos.x + m_size, pos.y + m_size);
	m_vertices[6 * index + 2].position = sf::Vector2f(pos.x, pos.y + m_size);
	m_vertices[6 * index + 3].position = sf::Vector2f(pos.x, pos.y);
	m_vertices[6 * index + 4].position = sf::Vector2f(pos.x + m_size, pos.y);
	m_vertices[6 * index + 5].position = sf::Vector2f(pos.x + m_size, pos.y + m_size);

	m_particles[index].position = pos + m_size / 2;
}
void ParticleSystem::colorParticle(const size_t index, const int noOfSides, const int r, const int g, const int b, const int a)
{
	sf::Color color(r, g, b, a);
	for (int i = 0; i < noOfSides; i++)
	{
		m_vertices[noOfSides * index + i].color = color;
	}
}

void ParticleSystem::textureParticle(const size_t index)
{
	m_vertices[6 * index + 0].texCoords = sf::Vector2f(0.0f, 0.0f);
	m_vertices[6 * index + 1].texCoords = sf::Vector2f(m_texture.getSize().x, m_texture.getSize().y);
	m_vertices[6 * index + 2].texCoords = sf::Vector2f(0.0f, m_texture.getSize().y);
	m_vertices[6 * index + 3].texCoords = sf::Vector2f(0.0f, 0.0f);
	m_vertices[6 * index + 4].texCoords = sf::Vector2f(m_texture.getSize().x, 0.0f);
	m_vertices[6 * index + 5].texCoords = sf::Vector2f(m_texture.getSize().x, m_texture.getSize().y);

	m_vertices[6 * index + 0].color.a = 255;
	m_vertices[6 * index + 1].color.a = 255;
	m_vertices[6 * index + 2].color.a = 255;
	m_vertices[6 * index + 3].color.a = 255;
	m_vertices[6 * index + 4].color.a = 255;
	m_vertices[6 * index + 5].color.a = 255;
}

void ParticleSystem::updateOpacityByLifespan(const size_t index, const int noOfSides)
{
	float div = (m_particles[index].totalLifetime != 0) ? 255.0f / m_particles[index].totalLifetime : 0.0f;
	for (int i = 0; i < noOfSides; i++)
	{
		float alpha = m_vertices[noOfSides * index + i].color.a - div;
		if (alpha < 0.0f) alpha = 0.0f;
		m_vertices[noOfSides * index + i].color.a = alpha;
	}
}

void ParticleSystem::resetParticle(const size_t index, const bool first)
{
	boxSpanning(index, first);

	std::uniform_real_distribution<> dis(m_speedRange.x, m_speedRange.y);
	float rx = rand() % 2 == 0 ? m_speedRange.x : m_speedRange.y;
	float ry = rand() % 2 == 0 ? m_speedRange.x : m_speedRange.y;
	if (randomSpeed)
	{
		rx = dis(gen);
		ry = dis(gen);
	}
	if (!m_particleDirection.allDirections)
	{
		if (m_particleDirection.north) { ry = -(abs(ry)); }
		if (m_particleDirection.south) { ry = abs(ry); }
		if (m_particleDirection.east) { rx = abs(rx); }
		if (m_particleDirection.west) { rx = -abs(rx); }

		if (m_particleDirection.trueNorth || m_particleDirection.trueSouth) {
			rx = 0;
			ry = m_particleDirection.trueNorth ? -abs(ry) : abs(ry);
		}
		if (m_particleDirection.trueEast || m_particleDirection.trueWest) {
			rx = m_particleDirection.trueEast ? abs(rx) : -abs(rx);
			ry = 0;
		}
	}
	m_particles[index].velocity = Vec2(rx, ry);
	m_particles[index].speed = m_particles[index].velocity.length();
	Vec2 nextPoint = m_particles[index].position + m_particles[index].velocity;
	float angle = m_particles[index].position.angleFromPoint(nextPoint);
	m_particles[index].angle = angle;

	float elapsedTime = initializationClock.getElapsedTime().asSeconds();
	float lifeFactor = (elapsedTime < initializationDuration && lifespan) ? (elapsedTime / initializationDuration) : 1.0f;

	dis = std::uniform_real_distribution<>(m_lifeRange.x, m_lifeRange.y);
	int lifetime = dis(gen);
	lifetime = lifetime * lifeFactor;
	m_particles[index].lifetime = lifetime;
	m_particles[index].totalLifetime = lifetime;
}

void ParticleSystem::move(const size_t index)
{
	auto& pos = m_particles[index].position;
	Physics physics;
	float& angle = m_particles[index].angle;
	// Update the angle
	if (circularMovement)
	{
		angle += m_turnSpeed;
		m_particles[index].velocity = Vec2(m_radius * cos(angle), m_radius * sin(angle));
	}
	if (targetTracking)
	{
		physics.steeringSeek(pos, targetPair.first, m_particles[index].velocity, m_dragSpeed, abs(m_particles[index].speed));
	}
	if (boid) boidAlg(index, m_boidParam.visualRange + m_size, m_boidParam.protectRange + m_size,
		m_particles[index].speed / m_boidParam.matchFac, m_particles[index].speed / m_boidParam.centerFac, m_particles[index].speed / m_boidParam.avoidFac);
	else pos += m_particles[index].velocity;

	int no_of_sides = 3;
	if (m_shapeType == "rectangle" || m_useTexture) {
		updateBoxShape(index);
		no_of_sides = 6;
	}
	else updateTriangleShape(index);
	float theta = std::atan2(m_particles[index].velocity.y, m_particles[index].velocity.x);
	rotate(index, theta + m_adjustmentAngle, no_of_sides);

	if (fadeOverLifespan && lifespan) updateOpacityByLifespan(index, no_of_sides);
}


void ParticleSystem::rotate(const size_t index, const float theta, const int noOfSides)
{
	sf::Vector2f center(m_particles[index].position.x, m_particles[index].position.y);

	// Calculate cosine and sine of the angle
	float cosTheta = std::cos(theta);
	float sinTheta = std::sin(theta);

	// Rotate each vertex around the center
	for (int i = 0; i < noOfSides; ++i)
	{
		auto& pos = m_vertices[noOfSides * index + i].position;
		// Translate point to origin
		float translatedX = pos.x - center.x;
		float translatedY = pos.y - center.y;
		// Apply rotation
		float rotatedX = translatedX * cosTheta - translatedY * sinTheta;
		float rotatedY = translatedX * sinTheta + translatedY * cosTheta;
		// Translate back
		pos = sf::Vector2f(rotatedX + center.x, rotatedY + center.y);
	}
}

void ParticleSystem::update()
{
	if(boid) m_spartialGrid.clear();
	for (size_t pIndex = 0; pIndex < m_particles.size(); pIndex++)
	{
		if (m_particles[pIndex].lifetime <= 0 && lifespan) resetParticle(pIndex);
		move(pIndex);
		if (lifespan) m_particles[pIndex].lifetime--;
		if(boid) m_spartialGrid.add(pIndex, m_particles[pIndex].position);
	}
}

void ParticleSystem::draw(sf::RenderWindow& window) const
{
	if (m_useTexture) window.draw(m_vertices, &m_texture);
	else window.draw(m_vertices);
}

void ParticleSystem::setSpeed(const float speed) { m_speedRange = Vec2(speed, speed);}
void ParticleSystem::setRadius(const float radius) { m_radius = radius; }
void ParticleSystem::setTurnSpeed(const float turnSpeed) { m_turnSpeed = turnSpeed; }

void ParticleSystem::boidAlg(const size_t index, const float visualRange, const float protectedRange, const float matchingFactor, const float centeringFactor, const float avoidFactor)
{
	Vec2 sepAvg;
	Vec2 aliAvg;
	Vec2 cohAvg;
	size_t neighbourBoids = 0;
	float visualRangeSquared = visualRange * visualRange;
	float protectedRangeSquared = protectedRange * protectedRange;

	auto neighbours = m_spartialGrid.getNeighbours(m_particles[index].position, visualRange);

	for (auto& i : neighbours)
	{
		if (i != index)
		{
			Vec2 diff = m_particles[index].position - m_particles[i].position;
			float distanceSquared = diff.x * diff.x + diff.y * diff.y;
			if (distanceSquared < protectedRangeSquared)
			{
				sepAvg += diff;
			}
			else if (distanceSquared < visualRangeSquared)
			{
				aliAvg += m_particles[i].velocity;
				cohAvg += m_particles[i].position;
				neighbourBoids++;
			}
		}
	}

	if (neighbourBoids > 0)
	{
		float invNeighbourBoids = 1.0f / neighbourBoids;
		aliAvg *= invNeighbourBoids;
		cohAvg *= invNeighbourBoids;
		aliAvg = (aliAvg - m_particles[index].velocity) * matchingFactor;
		cohAvg = (cohAvg - m_particles[index].position) * centeringFactor;
	}
	sepAvg *= avoidFactor;
	m_particles[index].velocity += aliAvg + cohAvg + sepAvg;
	if (m_boidParam.turnWhenOutOfBounds) outOfBoundsSteerTurnAway(index, m_particles[index].speed / m_boidParam.turnFac);
	else if (m_boidParam.collideOnEdges) maxBounds(index);
	else if (m_boidParam.mirrorToCenter) mirrorBoundary(index);
	float speed = m_particles[index].velocity.length();
	float minspeed = m_particles[index].speed / 2;
	float maxspeed = m_particles[index].speed;
	if (speed < minspeed)
	{
		m_particles[index].velocity.normalize();
		m_particles[index].velocity *= minspeed;
	}
	if (speed > maxspeed)
	{
		m_particles[index].velocity.normalize();
		m_particles[index].velocity *= maxspeed;
	}
	m_particles[index].position += m_particles[index].velocity;
}

bool ParticleSystem::isOutOfBounds(const Vec2& pos) const
{
	return pos.x < 0 + m_size / 2.0f || pos.x > m_boundary.x - m_size / 2.0f || pos.y < 0 + m_size / 2.0f || pos.y > m_boundary.y - m_size / 2.0f;
}

void ParticleSystem::maxBounds(const size_t index)
{
	Vec2 range(m_size / 2, m_size / 2);
	if (m_particles[index].position.x < 0 + range.x || m_particles[index].position.x > m_boundary.x - range.x)
	{
		m_particles[index].velocity.x = -m_particles[index].velocity.x;
	}
	if (m_particles[index].position.y < 0 + range.y || m_particles[index].position.y > m_boundary.y - range.x)
	{
		m_particles[index].velocity.y = -m_particles[index].velocity.y;
	}
}

void ParticleSystem::outOfBoundsSteerTurnAway(const size_t index, const float turnSpeed)
{
	Vec2 range(m_size * m_boidParam.startTurnRange.x, m_size * m_boidParam.startTurnRange.y);
	Vec2 direction = m_particles[index].velocity;
	direction.normalize();
	Vec2 endPoint = m_particles[index].position + direction.scale(m_size * 2);
	Vec2 center(m_boundary.x / 2, m_boundary.y / 2);
	if (isOutOfBounds(endPoint))
	{
		Physics physics;
		float distanceToBoundary = abs(std::min({
			m_particles[index].position.x,
			m_boundary.x - range.x - m_particles[index].position.x,
			m_particles[index].position.y,
			m_boundary.y - range.y - m_particles[index].position.y
			}));
		//float seekForce = (distanceToBoundary > 0) ? turnSpeed * distanceToBoundary : turnSpeed;
		//float seekForce = turnSpeed * (1.0f / (distanceToBoundary + 1.0f));
		physics.steeringSeek(m_particles[index].position, center, m_particles[index].velocity, turnSpeed, m_particles[index].speed);
	}
}


void ParticleSystem::mirrorBoundary(const size_t index)
{
	Vec2 range(m_size / 2, m_size / 2);
	if (m_particles[index].position.x < 0 - range.x) m_particles[index].position.x = m_boundary.x + range.x;
	if (m_particles[index].position.x > m_boundary.x + range.x) m_particles[index].position.x = -range.x;
	if (m_particles[index].position.y < 0 - range.y) m_particles[index].position.y = m_boundary.y + range.x;
	if (m_particles[index].position.y > m_boundary.y + range.y) m_particles[index].position.y = -range.y;
}

void ParticleSystem::changeShape(const std::string& shape)
{
	m_shapeType = shape;
	resetParticles();
}

void ParticleSystem::setTexture(const sf::Texture& texture)
{
	m_texture = texture;
	m_useTexture = true;
}

void ParticleSystem::setColors(const sf::Color& firstcolor, const sf::Color& secondcolor)
{
	m_firstColor = firstcolor, m_secondColor = secondcolor;
}
void ParticleSystem::setColor(const sf::Color& color) { m_color = color; }

float ParticleSystem::getAdjustmentAngle() { return m_adjustmentAngle; }
void ParticleSystem::setAdjustmentAngle(const float angle) { m_adjustmentAngle = angle; }
BoidParam& ParticleSystem::getBoidParam() { return m_boidParam; }
ParticleDirection& ParticleSystem::getParticleDirection() { return m_particleDirection; }

void ParticleSystem::setSpanningBorder(const Vec2& size) { m_spanningborder = size; }

void ParticleSystem::setTarget(const Vec2& target, const std::string& type)
{
	targetPair = { target, type };
}

void ParticleSystem::setLifeRange(const Vec2& lRange)
{
	m_lifeRange = lRange;
}

void ParticleSystem::setSpeedRange(const Vec2& sRange)
{
	m_speedRange = sRange;
}