#include "Collision.h"

Collision::Collision(EntityManager& eM)
	:entityManager(eM)
{}
std::vector<Collide> Collision::collideAll(const std::shared_ptr<Entity>& entity, float offset)
{
	if (!entity->getComponent<CBoundingbox>().has)
	{
		return {};
	}

	std::vector<Collide> allCollidedEntities;
	Physics physics;
	for (auto& e : entityManager.getEntities())
	{
		if (!e->getComponent<CBoundingbox>().has)
		{
			continue;
		}
		Vec2 overlap = physics.getOverLap(entity, e);
		if (overlap.x > offset && overlap.y > offset)
		{
			allCollidedEntities.push_back(Collide(e, overlap));
		}
	}
	return allCollidedEntities;
}
std::vector<Collide> Collision::collideWith(const std::shared_ptr<Entity>& entity, const std::string& name, float offset)
{
	if (!entity->getComponent<CBoundingbox>().has)
	{
		return {};
	}

	std::vector<Collide> allCollidedEntities;
	Physics physics;
	for (auto& e : entityManager.getEntities(name))
	{
		if (!e->getComponent<CBoundingbox>().has)
		{
			continue;
		}
		Vec2 overlap = physics.getOverLap(entity, e);
		if (overlap.x > offset && overlap.y > offset)
		{
			allCollidedEntities.push_back(Collide(e, overlap));
		}
	}
	return allCollidedEntities;
}
void Collision::resolveCollisionWith(std::shared_ptr<Entity>& entity, const std::string& name, float offset, const std::function<void(std::shared_ptr<Entity>&, std::shared_ptr<Entity>&, const Vec2&, const Vec2&)>& func)
{
	if (!entity->getComponent<CBoundingbox>().has)
	{
		return;
	}
	Physics physics;
	for (auto& e : entityManager.getEntities(name))
	{
		if (!e->getComponent<CBoundingbox>().has)
		{
			continue;
		}
		Vec2 overlap = physics.getOverLap(entity, e);
		Vec2 prevOverlap = physics.getPreviousOverLap(entity, e);
		if (overlap.x > offset && overlap.y > offset)
		{
			func(entity, e, overlap, prevOverlap);
		}
	}
}
bool Collision::checkCollison(const std::shared_ptr<Entity>& a, const std::shared_ptr<Entity>& b, float offset)
{
	Physics physics;
	Vec2 overlap = physics.getOverLap(a, b);
	return overlap.x > offset && overlap.y > offset;
}