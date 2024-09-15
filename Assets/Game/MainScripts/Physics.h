#pragma once
#include "EntityManager.h"
class Physics
{
public:
	Vec2 getOverLap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);
	Vec2 getPreviousOverLap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);
	Vec2 getOverLap(Vec2 aPos, Vec2 aBounds, Vec2 bPos, Vec2 bBounds);
	void steeringChase(const Vec2& pos, const Vec2& targetPos, Vec2& vel, const float speed);
	void steeringSeek(const Vec2& pos, const Vec2& targetPos, Vec2& vel, const float turnSpeed, const float speed);
	void steeringFlee(const Vec2& pos, const Vec2& targetPos, Vec2& vel, const float turnSpeed, const float speed);
	bool inRange(const Vec2& pos, const Vec2& targetPos, const Vec2& rangeSize);
	Vec2 rotatePointAroundPivot(const Vec2& point, const Vec2& pivot, float angle);
};

