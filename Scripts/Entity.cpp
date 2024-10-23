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

void Entity::addScriptable(const Scriptable& script) {
	removeScriptable(script.name);
	m_scriptables[script.name] = script;
}

Scriptable& Entity::getScriptable(const std::string& name)
{
	if (m_scriptables.find(name) != m_scriptables.end())
	{
		return m_scriptables[name];
	}
	throw std::runtime_error("Error - Scriptable '" + name + "' is not present.");
}

const std::map<std::string, Scriptable>& Entity::getScriptables()
{
	return m_scriptables;
}

bool Entity::hasScriptable(const std::string& name)
{
	if (m_scriptables.find(name) != m_scriptables.end())
	{
		return true;
	}
	return false;
}

bool Entity::hasAnyScriptable()
{
	return !m_scriptables.empty();
}

void Entity::removeScriptable(const std::string& name)
{
	if (hasScriptable(name))
	{
		auto& scriptable = getScriptable(name);
		(*scriptable.destroy) = true;
		m_scriptables.erase(name);
	}
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
		"hasBoxRender", & Entity::hasComponent<CBoxRender>,
		"addBoxRender", sol::overload(
			[](std::shared_ptr<Entity>& entity) {
				return entity->addComponent<CBoxRender>();
			},
			[](std::shared_ptr<Entity>& entity, CBoxRender& c) {
				return entity->addComponent<CBoxRender>(c.rectangle);
			}
		),
		"getBoxRender", [](std::shared_ptr<Entity>& entity) {
			return entity->getComponent<CBoxRender>();
		},
		"hasCircleRender", & Entity::hasComponent<CCircleRender>,
		"addCircleRender", sol::overload(
			[](std::shared_ptr<Entity>& entity) {
				return entity->addComponent<CCircleRender>();
			},
			[](std::shared_ptr<Entity>& entity, CCircleRender& c) {
				return entity->addComponent<CCircleRender>(c.circle);
			}
		),
		"getCircleRender", [](std::shared_ptr<Entity>& entity) {
			return entity->getComponent<CCircleRender>();
		},
		"hasCamera", & Entity::hasComponent<CCamera>,
		"addCamera", sol::overload(
			[](std::shared_ptr<Entity>& entity) {
				return entity->addComponent<CCamera>();
			},
			[](std::shared_ptr<Entity>& entity, CCamera& c) {
				return entity->addComponent<CCamera>(c.camera);
			}
		),
		"getCamera", [](std::shared_ptr<Entity>& entity) {
			return entity->getComponent<CCamera>();
		},
		"hasBoxCollider", & Entity::hasComponent<CBoxCollider>,
		"addBoxCollider", sol::overload(
			[](std::shared_ptr<Entity>& entity) {
				return entity->addComponent<CBoxCollider>();
			},
			[](std::shared_ptr<Entity>& entity, float width, float height) {
				return entity->addComponent<CBoxCollider>(width, height);
			}
		),
		"getBoxCollider", [](std::shared_ptr<Entity>& entity) {
			return entity->getComponent<CBoxCollider>();
		}
		,
		"hasCircleCollider", & Entity::hasComponent<CCircleCollider>,
		"addCircleCollider", sol::overload(
			[](std::shared_ptr<Entity>& entity) {
				return entity->addComponent<CCircleCollider>();
			},
			[](std::shared_ptr<Entity>& entity, float radius) {
				return entity->addComponent<CCircleCollider>(radius);
			}
		),
		"getCircleCollider", [](std::shared_ptr<Entity>& entity) {
			return entity->getComponent<CCircleCollider>();
		}
	);
}


