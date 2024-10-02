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

void Entity::Lua(sol::state& lua)
{
	lua.new_usertype<Entity>("Entity",
		"id", &Entity::id,
		"tag", &Entity::tag,
		"hasTransform", &Entity::hasComponent<CTransform>,
		"addTransform", [](std::shared_ptr<Entity>& entity, const Vec2& pos, const Vec2& scale, float angle) {
			return entity->addComponent<CTransform>(pos, scale, angle);
		},
		"getTransform", [](std::shared_ptr<Entity>& entity) {
			return &entity->getComponent<CTransform>();
		},
		"hasName", &Entity::hasComponent<CName>,
		"addName", [](std::shared_ptr<Entity>& entity, const std::string& name) {
			return entity->addComponent<CName>(name);
		},
		"getName", [](std::shared_ptr<Entity>& entity) {
			return entity->getComponent<CName>();
		},
		"hasSize", &Entity::hasComponent<CSize>,
		"addSize", [](std::shared_ptr<Entity>& entity, const Vec2& size) {
			return entity->addComponent<CSize>(size);
		},
		"getSize", [](std::shared_ptr<Entity>& entity) {
			return entity->getComponent<CSize>();
		},
		"hasRectangleShape", & Entity::hasComponent<CRectangleShape>,
		"addRectangleShape", sol::overload(
			[](std::shared_ptr<Entity>& entity) {
				return entity->addComponent<CRectangleShape>();
			},
			[](std::shared_ptr<Entity>& entity, CRectangleShape& c) {
				return entity->addComponent<CRectangleShape>(c.rectangle);
			}
		),
		"getRectangleShape", [](std::shared_ptr<Entity>& entity) {
			return entity->getComponent<CRectangleShape>();
		},
		"hasCircleShape", & Entity::hasComponent<CCircleShape>,
		"addCircleShape", sol::overload(
			[](std::shared_ptr<Entity>& entity) {
				return entity->addComponent<CCircleShape>();
			},
			[](std::shared_ptr<Entity>& entity, CCircleShape& c) {
				return entity->addComponent<CCircleShape>(c.circle);
			}
		),
		"getCircleShape", [](std::shared_ptr<Entity>& entity) {
			return entity->getComponent<CCircleShape>();
		}
	);
}


