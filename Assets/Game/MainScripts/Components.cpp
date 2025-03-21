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

CRectangleShape::CRectangleShape()
	:rectangle(sf::Vector2f(width, height))
{
	rectangle.setFillColor(sf::Color::White);
	rectangle.setOrigin(width/2.0f, height/2.0f);
}
CRectangleShape::CRectangleShape(sf::RectangleShape& rect)
	:rectangle(rect) {}

void CRectangleShape::Lua(sol::state& lua)
{
	lua.new_usertype<CRectangleShape>("CRectangleShape",
		sol::constructors<CRectangleShape()>(),
		"rectangle", &CRectangleShape::rectangle
	);
}

CCircleShape::CCircleShape()
	:circle(radius)
{
	circle.setFillColor(sf::Color::White);
	circle.setOrigin(radius, radius);
}
CCircleShape::CCircleShape(sf::CircleShape& cir)
	:circle(cir) {}

void CCircleShape::Lua(sol::state& lua)
{
	lua.new_usertype<CCircleShape>("CCirlceShape",
		sol::constructors<CCircleShape()>(),
		"circle", &CCircleShape::circle
	);
}


CCollision::CCollision() {}
CCollision::CCollision(float r)
	:radius(r)
{}

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


CScore::CScore() {}
CScore::CScore(int s)
	:score(s)
{}

CLifespan::CLifespan() {}
CLifespan::CLifespan(int duration, int frame)
	: lifespan(duration)
	, frameCreated(frame)
{}

CInput::CInput() 
{}

CBoundingbox::CBoundingbox() {}
CBoundingbox::CBoundingbox(const Vec2& s)
	:size(s)
	,halfSize(s.x / 2.0f, s.y / 2.0f)
{}

CState::CState() {}
CState::CState(const std::string& s)
	:state(s), prevState(s)
{}

CGravity::CGravity() {}
CGravity::CGravity(float g)
	:gravity(g)
{}

CAnimation::CAnimation() {}
CAnimation::CAnimation(const Animation& anim, bool r, int i)
	:animation(anim)
	,repeat(r)
	,index(i)
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