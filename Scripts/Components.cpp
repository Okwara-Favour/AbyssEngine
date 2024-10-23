#include "Components.h"

CTransform::CTransform(const Vec2& p, const Vec2& sc, float a)
	:pos(p)
	,prevPos(p)
	,scale(sc)
	,angle(a)
{}

void CTransform::Lua(sol::state& lua)
{
	lua.new_usertype<CTransform>("CTransform",
		sol::constructors<CTransform(), CTransform(const Vec2&, const Vec2&, float)>(),
		"pos", &CTransform::pos,
		"prevPos", &CTransform::prevPos,
		"scale", &CTransform::scale,
		"angle", &CTransform::angle
	);
}

CName::CName() {}
CName::CName(const std::string& n)
	:name(n)
{}

void CName::Lua(sol::state& lua)
{
	lua.new_usertype<CName>("CName",
		sol::constructors<CName(), CName(const std::string&)>(),
		"name", &CName::name
	);
}

CBoxRender::CBoxRender()
	:rectangle(sf::Vector2f(width, height))
{
	rectangle.setFillColor(sf::Color::White);
	rectangle.setOrigin(width/2.0f, height/2.0f);
}
CBoxRender::CBoxRender(sf::RectangleShape& rect)
	:rectangle(rect) {}

void CBoxRender::Lua(sol::state& lua)
{
	lua.new_usertype<CBoxRender>("CBoxRender",
		sol::constructors<CBoxRender(), CBoxRender(sf::RectangleShape&)>(),
		"rectangle", &CBoxRender::rectangle,
		"fillColor", &CBoxRender::fillColor,
		"outlineColor", &CBoxRender::outlineColor,
		"outlineThickness", &CBoxRender::outlineThickness
	);
}

CCircleRender::CCircleRender()
	:circle(radius)
{
	circle.setFillColor(sf::Color::White);
	circle.setOrigin(radius, radius);
}
CCircleRender::CCircleRender(sf::CircleShape& cir)
	:circle(cir) {}

void CCircleRender::Lua(sol::state& lua)
{
	lua.new_usertype<CCircleRender>("CCirlceShape",
		sol::constructors<CCircleRender(), CCircleRender(sf::CircleShape&)>(),
		"circle", &CCircleRender::circle,
		"fillColor", &CCircleRender::fillColor,
		"outlineColor", &CCircleRender::outlineColor,
		"outlineThickness", &CCircleRender::outlineThickness,
		"pointCount", &CCircleRender::pointCount
	);
}

CBoxCollider::CBoxCollider(const float w, const float h)
	:size(w, h), gSize(w, h) {}

void CBoxCollider::Lua(sol::state& lua)
{
	lua.new_usertype<CBoxCollider>("CBoxCollider",
		sol::constructors<CBoxCollider(), CBoxCollider(float, float)>(),

		"getSize", [](const CBoxCollider& self) { return self.size; },
		"getGlobalSize", [](const CBoxCollider& self) { return self.gSize; },

		"offset", &CBoxCollider::offset
	);
}

CCircleCollider::CCircleCollider(const float r)
	:radius(r), gRadius(r) {}

void CCircleCollider::Lua(sol::state& lua)
{
	lua.new_usertype<CCircleCollider>("CCircleCollider",
		sol::constructors<CCircleCollider(), CCircleCollider(float)>(),

		"getRadius", [](const CCircleCollider& self) { return self.radius; },
		"getGlobalRadius", [](const CCircleCollider& self) { return self.gRadius; },

		"offset", &CCircleCollider::offset
	);
}

CSize::CSize() {}
CSize::CSize(const Vec2& s)
	:size(s)
{}
void CSize::Lua(sol::state& lua)
{
	lua.new_usertype<CSize>("CSize",
		sol::constructors<CSize(), CSize(const Vec2&)>(),
		"size", &CSize::size
	);
}

CAnimation::CAnimation() {}
CAnimation::CAnimation(const Animation& anim, bool r)
	:animation(anim)
	,repeat(r)
{}

CAI::CAI() {}
CAI::CAI(int sT)
	:shootTimer(sT)
	,setDuration(sT)
{}

CTarget::CTarget(){}
CTarget::CTarget(const Vec2& p):pos(p)
{}

CLayer::CLayer() {}
CLayer::CLayer(int l) : layer(l) {}

CChildren::CChildren(const std::vector<std::pair<size_t, std::string>>& c) : children(c) {}
CParent::CParent(const size_t i, const std::string& t, const Vec2& pos, const Vec2& scale, const float a)
	: id(i), tag(t), initialPosition(pos), initialScale(scale), initialAngle(a) {}

CParticleSystem::CParticleSystem(const ParticleSystem& ps) :particleSystem(ps) {}

CLightSource::CLightSource(const LightSource& ls) : lightSource(ls) {}

CCamera::CCamera(const Camera& c) : camera(c) {}
void CCamera::Lua(sol::state& lua)
{
	lua.new_usertype<CCamera>("CCamera",
		sol::constructors<CCamera()>(),
		"camera", &CCamera::camera
	);
}