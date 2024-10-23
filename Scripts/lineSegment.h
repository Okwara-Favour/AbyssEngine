#pragma once
#include "Vec2.h"
struct Intersect
{
	bool m_result;
	Vec2 m_pos;
	Intersect(bool res, const Vec2& p);
};
class lineSegment
{
public:
	Vec2 m_startPos;
	Vec2 m_endPos;
	lineSegment(const Vec2& sp, const Vec2& ep);
	Intersect intersect(const lineSegment& other);
	lineSegment scaleDirec(float s);
};

