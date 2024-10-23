#pragma once
#include <string>
#include <tuple>
#include <typeinfo>
#include "Components.h"
#include "Scriptable.hpp"

class EntityManager;
class ScriptManager;
class PhysicsManager;
class Inspector;

typedef std::tuple
<
	CName,
	CTransform,
	CBoxCollider,
	CCircleCollider,
	CAnimation,
	CCamera,
	CSize,
	CBoxRender,
	CCircleRender,
	CAI,
	CTarget,
	CLayer,
	CChildren,
	CParent,
	CParticleSystem,
	CLightSource
> ComponentTuple;

typedef std::map<std::string, Scriptable> Scripts;

class Entity
{
	friend class EntityManager;

	bool		   m_dead = false;
	bool		   m_active =  true;
	std::string	   m_tag =	   "default";
	size_t		   m_id =	   0;
	ComponentTuple m_components;

	Entity(const size_t& id, const std::string& tag);

	void			  setTag(const std::string& tag);
	void			  destroy();
protected:
	friend Inspector;
	friend ScriptManager;
	friend PhysicsManager;
	Scripts		   m_scriptables;
	std::map<size_t, bool> firstCollisionIds = {};
public:
	size_t			  id()		 const;
	bool			  isAlive()	 const;
	bool			  isActive() const;
	const std::string tag()		 const;
	void			  disable();
	void			  enable();
	template <typename C>
	bool hasComponent() const
	{
		return std::get<C>(m_components).has;
	}

	template <typename C, typename... CArgs>
	C& addComponent(CArgs&&... mArgs)
	{
		auto& component = std::get<C>(m_components);
		component = C(std::forward<CArgs>(mArgs)...);
		component.has = true;
		return component;
	}

	template <typename C>
	C& getComponent()
	{
		if (hasComponent<C>()) return std::get<C>(m_components);
		else throw std::runtime_error("Error - Component of type '" + std::string(typeid(C).name()) + "' is not present.");
	}

	template <typename C>
	const C& getComponent() const
	{
		if (hasComponent<C>()) return std::get<C>(m_components);
		else throw std::runtime_error("Error - Component of type '" + std::string(typeid(C).name()) + "' is not present.");
	}

	template <typename C>
	void removeComponent()
	{
		std::get<C>(m_components) = C();
	}

	void addScriptable(const Scriptable& script);
	Scriptable& getScriptable(const std::string& name);
	const std::map<std::string, Scriptable>& getScriptables();
	bool hasScriptable(const std::string& name);
	bool hasAnyScriptable();
	void removeScriptable(const std::string& name);

	static void Lua(sol::state& lua);
};

