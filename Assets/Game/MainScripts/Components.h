#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include "Vec2.h"
#include "Animation.h"
#include "ParticleSystem.h"
#include "LightSource.h"

class Component
{
public:
	bool has = false;
};

class CTransform:public Component
{
public:
	Vec2 pos		= { 0, 0 };
	Vec2 prevPos	= { 0, 0 };
	Vec2 scale		= { 1, 1 };
	float angle = 0;
	CTransform(const Vec2& p = Vec2(0,0), const Vec2& sc = Vec2(1,1), float a = 0);
};

class CName :public Component
{
public:
	std::string name = "None";
	CName();
	CName(const std::string& n);
};

class CRectangleShape :public Component
{
	float width = 30, height = 30;
public:
	sf::RectangleShape rectangle;
	CRectangleShape();
	CRectangleShape(sf::RectangleShape& rect);
};

class CCircleShape :public Component
{
	float radius = 15;
public:
	sf::CircleShape circle;
	CCircleShape();
	CCircleShape(sf::CircleShape& cir);
};

class CCollision :public Component
{
public:
	float radius = 0;
	CCollision();
	CCollision(float r);
};

class CSize :public Component
{
public:
	Vec2 size = {30, 30 };
	CSize();
	CSize(const Vec2& s);
};

class CScore :public Component
{
public:
	int score = 0;
	CScore();
	CScore(int s);
};

class CLifespan :public Component
{
public:
	int lifespan = 0;
	int frameCreated = 0;
	CLifespan();
	CLifespan(int duration, int frame);
};

class CInput :public Component
{
public:
	bool up			= false;
	bool left		= false;
	bool right		= false;
	bool down		= false;
	bool shoot		= false;
	bool canJump	= true;
	bool canShoot	= true;

	CInput();
};

class CBoundingbox :public Component
{
public:
	Vec2 size;
	Vec2 halfSize;

	CBoundingbox();
	CBoundingbox(const Vec2& s);
};

class CState :public Component
{
public:
	std::string state = "";
	std::string prevState = "";
	bool standing = false;
	bool attacking = false;
	CState();
	CState(const std::string& s);
};

class CGravity :public Component
{
public:
	float gravity = 0;
	CGravity();
	CGravity(float g);
};

class CAnimation :public Component
{
public:
	Animation animation;
	int index = 0;
	bool repeat = false;
	CAnimation();
	CAnimation(const Animation& anim, bool r = false, int i = 0);
};

class CAI : public Component
{
public:
	int shootTimer = 10;
	int setDuration = 10;
	bool move = false;
	CAI();
	CAI(int sT);
};

class CTarget : public Component
{
public:
	Vec2 pos;
	CTarget();
	CTarget(const Vec2& p);
};

class CLayer : public Component
{
public:
	int layer = 0;
	CLayer();
	CLayer(int l);
};

class CChildren : public Component
{
public:
	std::vector<std::pair<size_t, std::string>> children;
	CChildren(const std::vector<std::pair<size_t, std::string>>& c = {});
};

class CParent : public Component
{
public:
	size_t id = 0;
	std::string tag = "";
	Vec2 childPos;
	Vec2 childScale = Vec2(1,1);
	float childAngle = 0;
	Vec2 initialPosition;
	Vec2 initialScale;
	float initialAngle;
	CParent(const size_t i = 0, const std::string& t = "", const Vec2& pos = Vec2(), const Vec2& scale = Vec2(), const float a = 0.0f);
};

class CParticleSystem : public Component
{
public:
	ParticleSystem particleSystem;
	CParticleSystem(const ParticleSystem& ps = ParticleSystem());
};

class CLightSource : public Component
{
public:
	LightSource lightSource;
	int index = 0;
	CLightSource(const LightSource& ls = LightSource());
};