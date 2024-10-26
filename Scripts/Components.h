#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include <sol/sol.hpp>
#include "Vec2.h"
#include "Animation.h"
#include "ParticleSystem.h"
#include "LightSource.h"
#include "Camera.h"
#include <any>

class Component
{
public:
	bool has = false;
};



class Editor;
class Hierarchy;

class CTransform :public Component
{
protected:
	friend Editor;
	static void Lua(sol::state& lua);
public:
	Vec2 pos = { 0, 0 };
	Vec2 prevPos = { 0, 0 };
	Vec2 scale = { 1, 1 };
	float angle = 0;
	CTransform(const Vec2& p = Vec2(0, 0), const Vec2& sc = Vec2(1, 1), float a = 0);
};

class CName :public Component
{
protected:
	friend Editor;
	static void Lua(sol::state& lua);
public:
	std::string name = "None";
	CName();
	CName(const std::string& n);
};

class CBoxRender :public Component
{
	float width = 30, height = 30;
protected:
	friend Editor;
	static void Lua(sol::state& lua);
public:
	sf::RectangleShape	rectangle;
	sf::Color			fillColor = sf::Color::White;
	sf::Color			outlineColor = sf::Color::Black;
	float				outlineThickness = 0;
	CBoxRender();
	CBoxRender(sf::RectangleShape& rect);
	void setOutlineThickness(float thickness) { outlineThickness = thickness; }
};

class CCircleRender :public Component
{
	float radius = 15;
protected:
	friend Editor;
	static void Lua(sol::state& lua);
public:
	sf::CircleShape circle;
	sf::Color		fillColor = sf::Color::White;
	sf::Color		outlineColor = sf::Color::Black;
	float			outlineThickness = 0;
	size_t			pointCount = 30;
	CCircleRender();
	CCircleRender(sf::CircleShape& cir);
};

class CBoxCollider : public Component
{
protected:
	friend Editor;
	static void Lua(sol::state& lua);
public:
	Vec2	size;
	Vec2	offset;
	Vec2	gSize;
	CBoxCollider(const float w = 0.0f, const float h = 0.0f);
};

class CCircleCollider : public Component
{
protected:
	friend Editor;
	static void Lua(sol::state& lua);
public:
	float radius;
	float offset = 0;
	float gRadius;
	CCircleCollider(const float r = 0.0f);
};

class CSize :public Component
{
protected:
	friend Editor;
	static void Lua(sol::state& lua);
public:
	Vec2 size = {30, 30 };
	CSize();
	CSize(const Vec2& s);
};

class CAnimation :public Component
{
public:
	Animation	animation;
	bool		repeat = false;
	sf::Color	fillColor = sf::Color::White;
	CAnimation();
	CAnimation(const Animation& anim, bool r = false);
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
protected:
	friend Hierarchy;
	bool _editor_use_open = false;
public:
	std::vector<std::pair<size_t, std::string>> children;
	std::map<size_t, std::any> childEntities;
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

class CCamera : public Component
{
protected:
	friend Editor;
	static void Lua(sol::state& lua);
public:
	Camera camera;
	CCamera(const Camera& c = Camera());
};