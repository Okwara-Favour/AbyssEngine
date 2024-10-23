#pragma once
#include "EntityManager.h"
class Physics
{
	std::pair<bool, Vec2> getBoxOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);
	std::pair<bool, Vec2> getBoxPrevOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);
	std::pair<bool, Vec2> getCircleOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);
	std::pair<bool, Vec2> getCirclePrevOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);
	std::pair<bool, Vec2> getBoxCircleOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);
	std::pair<bool, Vec2> getBoxCirclePrevOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);
public:
	std::pair<bool, Vec2> getOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);
	std::pair<bool, Vec2> getPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);
	std::pair<bool, Vec2> getBoxOverlap(Vec2 aPos, Vec2 aBounds, Vec2 bPos, Vec2 bBounds);
	std::pair<bool, Vec2> getCircleOverlap(Vec2 aPos, float aRadius, Vec2 bPos, float bRadius);
	std::pair<bool, Vec2> getBoxCircleOverlap(Vec2 aPos, Vec2 aHalfBounds, Vec2 bPos, float bRadius);
	void steeringChase(const Vec2& pos, const Vec2& targetPos, Vec2& vel, const float speed);
	void steeringSeek(const Vec2& pos, const Vec2& targetPos, Vec2& vel, const float turnSpeed, const float speed);
	void steeringFlee(const Vec2& pos, const Vec2& targetPos, Vec2& vel, const float turnSpeed, const float speed);
	bool inRange(const Vec2& pos, const Vec2& targetPos, const Vec2& rangeSize);
	Vec2 rotatePointAroundPivot(const Vec2& point, const Vec2& pivot, float angle);
};

