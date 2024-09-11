#pragma once
#include <string>
#include <tuple>
#include "Components.h"

class EntityManager;

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
	CShape,
	CAI,
	CTarget,
	CLayer,
	CChildren,
	CParent,
	CParticleSystem,
	CLightSource
> ComponentTuple;


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

	template <typename C>
	C& getC()
	{
		return std::get<C>(m_components);
	}

	template <typename C>
	const C& getC() const
	{
		return std::get<C>(m_components);
	}
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
		return getC<C>().has;
	}

	template <typename C, typename... CArgs>
	C& addComponent(CArgs&&... mArgs)
	{
		auto& component = getC<C>();
		component = C(std::forward<CArgs>(mArgs)...);
		component.has = true;
		return component;
	}

	template <typename C>
	C& getComponent()
	{
		if (hasComponent<C>()) return std::get<C>(m_components);
		else throw std::runtime_error("Component of the requested type is not present.");
	}

	template <typename C>
	const C& getComponent() const
	{
		if (hasComponent<C>()) return std::get<C>(m_components);
		else throw std::runtime_error("Component of the requested type is not present.");
	}

	template <typename C>
	void removeComponent()
	{
		getC<C>() = C();
	}
};

