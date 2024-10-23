#pragma once
#include <functional>
#include "Physics.h"
#include "EntityManager.h"

struct CollideInfo
{
	std::shared_ptr<Entity> c_entity;
	Vec2 c_overlap;
	CollideInfo(const std::shared_ptr<Entity>& e, Vec2 overlap)
		:c_entity(e)
		,c_overlap(overlap)
	{}
};

class Collision
{
public: 
	Collision() {};
	std::vector<CollideInfo> collideAll(const std::shared_ptr<Entity>& entity, EntityManager& EM, float offset = 0);
	std::vector<CollideInfo> collideWithTag(const std::shared_ptr<Entity>& entity, EntityManager& EM, const std::string& tag, float offset = 0);
	bool checkCollison(const std::shared_ptr<Entity>& a, const std::shared_ptr<Entity>& b, float offset = 0);
};

