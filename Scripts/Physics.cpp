#include "Physics.h"
#include "Components.h"

std::pair<bool, Vec2> Physics::getBoxOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
	auto& aTransform = a->getComponent<CTransform>();
	auto& aBoundary = a->getComponent<CBoxCollider>();
	Vec2 aPos = aTransform.pos;

	auto& bTransform = b->getComponent<CTransform>();
	auto& bBoundary = b->getComponent<CBoxCollider>();
	auto& bSize = b->getComponent<CSize>().size;
	Vec2 bPos = bTransform.pos;
	return getBoxOverlap(aPos, aBoundary.size, bPos, bBoundary.size);
}
std::pair<bool, Vec2> Physics::getBoxPrevOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
	auto& aTransform = a->getComponent<CTransform>();
	auto& aBoundary = a->getComponent<CBoxCollider>();
	Vec2 aPos = aTransform.prevPos;

	auto& bTransform = b->getComponent<CTransform>();
	auto& bBoundary = b->getComponent<CBoxCollider>();
	auto& bSize = b->getComponent<CSize>().size;
	Vec2 bPos = bTransform.prevPos;

	return getBoxOverlap(aPos, aBoundary.size, bPos, bBoundary.size);
}
std::pair<bool, Vec2> Physics::getCircleOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
	auto& aTransform = a->getComponent<CTransform>();
	auto& aCollider = a->getComponent<CCircleCollider>(); // Assuming this component exists
	Vec2 aPos = aTransform.pos;
	float aRadius = aCollider.radius;
	// Get components for entity b
	auto& bTransform = b->getComponent<CTransform>();
	auto& bCollider = b->getComponent<CCircleCollider>(); // Assuming this component exists
	Vec2 bPos = bTransform.pos;
	float bRadius = bCollider.radius;
	return getCircleOverlap(aPos, aRadius, bPos, bRadius);
}
std::pair<bool, Vec2> Physics::getCirclePrevOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
	auto& aTransform = a->getComponent<CTransform>();
	auto& aCollider = a->getComponent<CCircleCollider>(); // Assuming this component exists
	Vec2 aPos = aTransform.prevPos;
	float aRadius = aCollider.radius;
	// Get components for entity b
	auto& bTransform = b->getComponent<CTransform>();
	auto& bCollider = b->getComponent<CCircleCollider>(); // Assuming this component exists
	Vec2 bPos = bTransform.prevPos;
	float bRadius = bCollider.radius;
	return getCircleOverlap(aPos, aRadius, bPos, bRadius);
}

std::pair<bool, Vec2> Physics::getBoxCircleOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
	auto& aTransform = a->getComponent<CTransform>();
	auto& aBox = a->getComponent<CBoxCollider>();
	Vec2 aPos = aTransform.pos;
	float halfWidth = aBox.size.x / 2.0f;
	float halfHeight = aBox.size.y / 2.0f;

	// Get components for entity b (Circle)
	auto& bTransform = b->getComponent<CTransform>();
	auto& bCircle = b->getComponent<CCircleCollider>();
	Vec2 bPos = bTransform.pos;
	float bRadius = bCircle.radius;

	return getBoxCircleOverlap(aPos, Vec2(halfWidth, halfHeight), bPos, bRadius);
}
std::pair<bool, Vec2> Physics::getBoxCirclePrevOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
	auto& aTransform = a->getComponent<CTransform>();
	auto& aBox = a->getComponent<CBoxCollider>();
	Vec2 aPos = aTransform.prevPos;
	float halfWidth = aBox.size.x / 2.0f;
	float halfHeight = aBox.size.y / 2.0f;

	// Get components for entity b (Circle)
	auto& bTransform = b->getComponent<CTransform>();
	auto& bCircle = b->getComponent<CCircleCollider>();
	Vec2 bPos = bTransform.prevPos;
	float bRadius = bCircle.radius;

	return getBoxCircleOverlap(aPos, Vec2(halfWidth, halfHeight), bPos, bRadius);
}


std::pair<bool, Vec2> Physics::getOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
	if (a->hasComponent<CBoxCollider>() && b->hasComponent<CBoxCollider>())
	{
		return getBoxOverlap(a, b);
	}
	if (a->hasComponent<CCircleCollider>() && b->hasComponent<CCircleCollider>())
	{
		return getCircleOverlap(a, b);
	}
	if (a->hasComponent<CBoxCollider>() && b->hasComponent<CCircleCollider>())
	{
		return getBoxCircleOverlap(a, b);
	}
	if (a->hasComponent<CCircleCollider>() && b->hasComponent<CBoxCollider>())
	{
		return getBoxCircleOverlap(b, a);
	}
	return { false, Vec2() };
}
std::pair<bool, Vec2> Physics::getPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
	if (a->hasComponent<CBoxCollider>() && b->hasComponent<CBoxCollider>())
	{
		return getBoxPrevOverlap(a, b);
	}
	if (a->hasComponent<CCircleCollider>() && b->hasComponent<CCircleCollider>())
	{
		return getCirclePrevOverlap(a, b);
	}
	if (a->hasComponent<CBoxCollider>() && b->hasComponent<CCircleCollider>())
	{
		return getBoxCirclePrevOverlap(a, b);
	}
	if (a->hasComponent<CCircleCollider>() && b->hasComponent<CBoxCollider>())
	{
		return getBoxCirclePrevOverlap(b, a);
	}
	return { false, Vec2() };
}

std::pair<bool, Vec2> Physics::getBoxOverlap(Vec2 aPos, Vec2 aBounds, Vec2 bPos, Vec2 bBounds)
{
	Vec2 delta = { aPos.x - bPos.x, aPos.y - bPos.y };
	float ox = (aBounds.x + bBounds.x) / 2.0f - std::abs(delta.x);
	float oy = (aBounds.y + bBounds.y) / 2.0f - std::abs(delta.y);

	if (ox > 0 && oy > 0)
	{
		if (ox < oy)
		{
			return { true, Vec2(delta.x > 0 ? ox : -ox, 0) };
		}
		else
		{
			return { true, Vec2(0, delta.y > 0 ? oy : -oy) };
		}
	}
	return { false, Vec2() };
}

std::pair<bool, Vec2> Physics::getCircleOverlap(Vec2 aPos, float aRadius, Vec2 bPos, float bRadius)
{
	Vec2 delta = bPos - aPos;
	float distance = delta.length();
	// Calculate the overlap
	float overlap = (aRadius + bRadius) - distance;
	// If there is overlap, return the overlap distance in the direction of delta
	if (overlap > 0)
	{
		delta.normalize();
		Vec2 overlapVector = delta * overlap;
		return { true, overlapVector };
	}
	// No overlap
	return { false, Vec2() };
}

std::pair<bool, Vec2> Physics::getBoxCircleOverlap(Vec2 aPos, Vec2 aHalfBounds, Vec2 bPos, float bRadius)
{
	Vec2 closestPoint = {
	   std::max(aPos.x - aHalfBounds.x, std::min(bPos.x, aPos.x + aHalfBounds.x)),
	   std::max(aPos.y - aHalfBounds.y, std::min(bPos.y, aPos.y + aHalfBounds.y))
	};

	// Calculate the distance between the closest point and the circle center
	Vec2 delta = bPos - closestPoint;
	float distance = delta.length();

	float overlap = bRadius - distance;

	if (overlap > 0)
	{
		delta.normalize();
		Vec2 overlapVector = delta * overlap;
		return { true, overlapVector };
	}

	// No overlap
	return { false, Vec2() };
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