#include "Collision.h"

std::vector<CollideInfo> Collision::collideAll(const std::shared_ptr<Entity>& entity, EntityManager& EM, float offset)
{
	if (!entity->getComponent<CBoxCollider>().has || !entity->getComponent<CCircleCollider>().has)
	{
		return {};
	}

	std::vector<CollideInfo> allCollidedEntities;
	Physics physics;
	for (auto& e : EM.getEntities())
	{
		if (!e->getComponent<CBoxCollider>().has || !e->getComponent<CCircleCollider>().has)
		{
			continue;
		}
		auto overlap = physics.getOverlap(entity, e);
		if (overlap.first)
		{
			allCollidedEntities.push_back(CollideInfo(e, overlap.second));
		}
	}
	return allCollidedEntities;
}
std::vector<CollideInfo> Collision::collideWithTag(const std::shared_ptr<Entity>& entity, EntityManager& EM, const std::string& tag, float offset)
{
	if (!entity->getComponent<CBoxCollider>().has || !entity->getComponent<CCircleCollider>().has)
	{
		return {};
	}

	std::vector<CollideInfo> allCollidedEntities;
	Physics physics;
	for (auto& e : EM.getEntities(tag))
	{
		if (!e->getComponent<CBoxCollider>().has || !e->getComponent<CCircleCollider>().has)
		{
			continue;
		}
		auto overlap = physics.getOverlap(entity, e);
		if (overlap.first)
		{
			allCollidedEntities.push_back(CollideInfo(e, overlap.second));
		}
	}
	return allCollidedEntities;
}

bool Collision::checkCollison(const std::shared_ptr<Entity>& a, const std::shared_ptr<Entity>& b, float offset)
{
	Physics physics;
	return physics.getOverlap(a, b).first;
}