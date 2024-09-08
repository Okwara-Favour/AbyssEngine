#include "Components.h"

CTransform::CTransform() {}
CTransform::CTransform(const Vec2& p)
	:pos(p)
	,prevPos(p)
{}
CTransform::CTransform(const Vec2& p, const Vec2& v, const Vec2& sc, float a)
	:pos(p)
	,prevPos(p)
	,velocity(v)
	,scale(sc)
	,angle(a)
{}
CTransform::CTransform(const Vec2& p, const Vec2& v, float a, float ms, float s)
	:pos(p)
	,prevPos(p)
	, velocity(v)
	, angle(a)
	, maxSpeed(ms)
	, speed(s)
{}

CName::CName() {}
CName::CName(const std::string& n)
	:name(n)
{}

CShape::CShape() {}
CShape::CShape(float width, float height, const sf::Color& fill, const sf::Color& outline, float thickness)
	:rectangle(sf::Vector2f(width, height))
{
	rectangle.setFillColor(fill);
	rectangle.setOutlineColor(outline);
	rectangle.setOutlineThickness(thickness);
	rectangle.setOrigin(width/2.0f, height/2.0f);
}
CShape::CShape(sf::RectangleShape& rect)
	:rectangle(rect)
{
	rectangle.setFillColor(rect.getFillColor());
	rectangle.setOutlineColor(rect.getOutlineColor());
	rectangle.setOutlineThickness(rect.getOutlineThickness());
	rectangle.setOrigin(rect.getSize().x / 2.0f, rect.getSize().y / 2.0f);
}

CCollision::CCollision() {}
CCollision::CCollision(float r)
	:radius(r)
{}

CSize::CSize() {}
CSize::CSize(const Vec2& s)
	:size(s)
{}

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
CParent::CParent(const size_t i, const std::string& t, const Vec2& distance, const Vec2& size, const float a)
	: id(i), tag(t), initialDistance(distance), size(size), initialSize(size), angle(a) {}

CParticleSystem::CParticleSystem(const ParticleSystem& ps) :particleSystem(ps) {}

CLightSource::CLightSource(const LightSource& ls) : lightSource(ls) {}