#include "Physics.h"
#include "Components.h"

Vec2 Physics::getOverLap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
	auto& aTransform = a->getComponent<CTransform>();
	auto& aBoundary = a->getComponent<CBoundingbox>().size;
	auto& aSize = a->getComponent<CSize>().size;
	Vec2 aPos = { aTransform.pos.x, aTransform.pos.y + (aSize.y - aBoundary.y) / 2.0f };

	auto& bTransform = b->getComponent<CTransform>();
	auto& bBoundary = b->getComponent<CBoundingbox>().size;
	auto& bSize = b->getComponent<CSize>().size;
	Vec2 bPos = { bTransform.pos.x, bTransform.pos.y + (bSize.y - bBoundary.y) / 2.0f };

	Vec2 delta = { std::abs(aPos.x - bPos.x), std::abs(aPos.y - bPos.y) };
	float ox = (aBoundary.x + bBoundary.x) / 2.0f - delta.x;
	float oy = (aBoundary.y + bBoundary.y) / 2.0f - delta.y;

	return Vec2(ox, oy);
}
Vec2 Physics::getPreviousOverLap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
	auto& aTransform = a->getComponent<CTransform>();
	auto& aBoundary = a->getComponent<CBoundingbox>().size;
	auto& aSize = a->getComponent<CSize>().size;
	Vec2 aPos = { aTransform.prevPos.x, aTransform.prevPos.y + (aSize.y - aBoundary.y) / 2.0f };

	auto& bTransform = b->getComponent<CTransform>();
	auto& bBoundary = b->getComponent<CBoundingbox>().size;
	auto& bSize = b->getComponent<CSize>().size;
	Vec2 bPos = { bTransform.prevPos.x, bTransform.prevPos.y + (bSize.y - bBoundary.y) / 2.0f };

	Vec2 delta = { std::abs(aPos.x - bPos.x), std::abs(aPos.y - bPos.y) };
	float ox = (aBoundary.x + bBoundary.x) / 2.0f - delta.x;
	float oy = (aBoundary.y + bBoundary.y) / 2.0f - delta.y;

	return Vec2(ox, oy);
}

Vec2 Physics::getOverLap(Vec2 aPos, Vec2 aBounds, Vec2 bPos, Vec2 bBounds)
{
	Vec2 delta = { std::abs(aPos.x - bPos.x), std::abs(aPos.y - bPos.y) };
	float ox = (aBounds.x + bBounds.x) / 2.0f - delta.x;
	float oy = (aBounds.y + bBounds.y) / 2.0f - delta.y;

	return Vec2(ox, oy);
}

void Physics::steeringChase(const Vec2& pos, const Vec2& targetPos, Vec2& vel, const float speed)
{
	Vec2 dir = targetPos - pos;
	dir.normalize();
	vel = dir * speed;
}

void Physics::steeringSeek(const Vec2& pos, const Vec2& targetPos, Vec2& vel, const float turnSpeed, const float speed)
{
	Vec2 desired = targetPos - pos;
	desired.normalize();
	desired = desired * speed;
	Vec2 steering = desired - vel;
	steering = steering * turnSpeed;
	vel = vel + steering;
}

void Physics::steeringFlee(const Vec2& pos, const Vec2& targetPos, Vec2& vel, const float turnSpeed, const float speed)
{
	Vec2 desired = pos - targetPos;
	desired.normalize();
	desired = desired * speed;
	Vec2 steering = desired - vel;
	steering = steering * turnSpeed;
	vel = vel + steering;
}


bool Physics::inRange(const Vec2& pos, const Vec2& targetPos, const Vec2& rangeSize)
{
	Vec2 minRange = targetPos - rangeSize.x/2;
	Vec2 maxRange = targetPos + rangeSize.y/2;
	return pos.x >= minRange.x && pos.x <= maxRange.x &&
		   pos.y >= minRange.y && pos.y <= maxRange.y;
}

Vec2 Physics::rotatePointAroundPivot(const Vec2& point, const Vec2& pivot, float angle)
{
	float s = sin(angle);
	float c = cos(angle);

	// Translate point back to origin:
	Vec2 translatedPoint = point - pivot;

	// Rotate point
	float xNew = translatedPoint.x * c - translatedPoint.y * s;
	float yNew = translatedPoint.x * s + translatedPoint.y * c;

	// Translate point back:
	Vec2 rotatedPoint = Vec2(xNew, yNew) + pivot;
	return rotatedPoint;
}