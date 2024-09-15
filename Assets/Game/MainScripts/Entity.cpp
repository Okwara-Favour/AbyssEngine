#include "Entity.h"


Entity::Entity(const size_t& id, const std::string& tag)
	: m_id(id)
	, m_tag(tag)
{}

void Entity::destroy()
{
	m_dead = true;
}

void Entity::disable()
{
	m_active = false;
}

void Entity::enable()
{
	m_active = true;
}

size_t Entity::id() const
{
	return m_id;
}

bool Entity::isActive() const
{
	return m_active;
}

bool Entity::isAlive() const
{
	return !m_dead;
}

void Entity::setTag(const std::string& tag)
{
	m_tag = tag;
}
const std::string Entity::tag() const
{
	return m_tag;
}


