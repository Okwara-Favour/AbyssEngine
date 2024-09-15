#pragma once
#include <functional>
#include "Scene.h"
#include "Physics.h"

struct Collide
{
	std::shared_ptr<Entity> entity;
	Vec2 overlap;
	Collide(std::shared_ptr<Entity>& e, Vec2 ov)
		:entity(e)
		,overlap(ov)
	{}
};

class Collision
{
	EntityManager entityManager;
public:
	Collision(EntityManager& eM);
	std::vector<Collide> collideAll(const std::shared_ptr<Entity>& entity, float offset = 0);
	std::vector<Collide> collideWith(const std::shared_ptr<Entity>& entity, const std::string& name, float offset = 0);
	void resolveCollisionWith(std::shared_ptr<Entity>& entity, const std::string& name, float offset, const std::function<void(std::shared_ptr<Entity>&, std::shared_ptr<Entity>&, const Vec2&, const Vec2&)>& func);
	bool checkCollison(const std::shared_ptr<Entity>& a, const std::shared_ptr<Entity>& b, float offset = 0);
};

