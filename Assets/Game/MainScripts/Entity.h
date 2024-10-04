#pragma once
#include <string>
#include <tuple>
#include <typeinfo>
#include "Components.h"
#include "Scriptable.hpp"

class EntityManager;
class ScriptManager;
class Inspector;

typedef std::tuple
<
	CName,
	CTransform,
	CLifespan,
	CInput,
	CBoundingbox,
	CAnimation,
	CGravity,
	CState,
	CSize,
	CRectangleShape,
	CCircleShape,
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
	Scripts		   m_scriptables;
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

	void addScriptable(const Scriptable& script) {
		m_scriptables[script.name] = script;
	}

	Scriptable& getScriptable(const std::string& name)
	{
		if (m_scriptables.find(name) != m_scriptables.end())
		{
			return m_scriptables[name];
		}
		throw std::runtime_error("Error - Scriptable '" + name + "' is not present.");
	}

	const std::map<std::string, Scriptable>& getScriptables()
	{
		return m_scriptables;
	}

	bool hasScriptable(const std::string& name)
	{
		if (m_scriptables.find(name) != m_scriptables.end())
		{
			return true;
		}
		return false;
	}

	bool hasAnyScriptable()
	{
		return !m_scriptables.empty();
	}

	void removeScriptable(const std::string& name)
	{
		auto& scriptable = getScriptable(name);
		(*scriptable.destroy) = true;
		m_scriptables.erase(name);
	}

	void UpdateScripts()
	{
		for (const auto& script : m_scriptables)
		{
			//script->Update();
		}
	}

	static void Lua(sol::state& lua);
};

