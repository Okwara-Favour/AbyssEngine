#include "lineSegment.h"
#include <iostream>

Intersect::Intersect(bool res, const Vec2& p)
	:m_result(res)
	,m_pos(p)
{}

lineSegment::lineSegment(const Vec2& sp, const Vec2& ep)
	:m_startPos(sp)
	,m_endPos(ep)
{}

Intersect lineSegment::intersect(const lineSegment& other)
{
	Vec2 r = (this->m_endPos - this->m_startPos);
	Vec2 s = (other.m_endPos - other.m_startPos);
	float rxs = r * s;
	Vec2 cma = other.m_startPos - this->m_startPos;
	float t = (cma * s) / (rxs);
	float u = (cma * r) / (rxs);
	if (t >= 0 && t <= 1 && u >= 0 && u <= 1)
		return Intersect(true, Vec2(this->m_startPos.x + t * r.x, this->m_startPos.y + t * r.y));
	else
		return Intersect(false, Vec2(0, 0));
}

lineSegment lineSegment::scaleDirec(float s)
{
	Vec2 r = this->m_endPos - this->m_startPos;
	Vec2 nep = this->m_startPos + (r * s);
	return lineSegment(this->m_startPos, nep);
}