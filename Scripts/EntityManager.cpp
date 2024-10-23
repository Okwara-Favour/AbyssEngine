#include "EntityManager.h"

EntityManager::EntityManager() {};

EntityVec& EntityManager::getEntities()
{
	return m_entities;
}

EntityVec& EntityManager::getEntities(const std::string& tag)
{
	return m_entityMap[tag];
}

std::shared_ptr<Entity> EntityManager::safeGetEntity(const size_t id)
{
	//if you want safety
	auto it = m_uniqueEntityMap.find(id);
	if (it != m_uniqueEntityMap.end()) {
		return it->second;
	}
	return nullptr;
}

std::shared_ptr<Entity>& EntityManager::getEntity(const size_t id)
{
	return m_uniqueEntityMap[id];
}

std::shared_ptr<Entity> EntityManager::getEntityName(const std::string& name)
{
	for (auto& e : m_entities)
	{
		if (e->hasComponent<CName>())
		{
			if (e->getComponent<CName>().name == name)
			{
				return e;
			}
		}
	}
	return nullptr;
}

std::shared_ptr<Entity> EntityManager::MakeEntityCopy(const std::shared_ptr<Entity>& entity)
{
	if (!entity) return nullptr;
	std::shared_ptr<Entity> copyEntity = std::shared_ptr<Entity>(new Entity(m_totalEntities, entity->tag()));
	copyEntity->m_active = entity->m_active;
	copyEntity->m_dead = entity->m_dead;
	copyEntity->m_components = entity->m_components;

	for (auto& sc : entity->m_scriptables)
	{
		Scriptable newSc;
		newSc.name = sc.second.name;
		newSc.hasMapped = std::make_shared<bool>(*sc.second.hasMapped);
		newSc.destroy = std::make_shared<bool>(*sc.second.destroy);
		newSc.variableMap = sc.second.variableMap;
		copyEntity->addScriptable(newSc);
	}
	return copyEntity;
}


std::shared_ptr<Entity> EntityManager::Instantiate(const std::shared_ptr<Entity>& entity)
{
	if (!entity) return nullptr;

	std::shared_ptr<Entity> copyEntity = std::shared_ptr<Entity>(new Entity(m_totalEntities++, entity->tag()));
	copyEntity->m_active = entity->m_active;
	copyEntity->m_dead = entity->m_dead;
	copyEntity->m_components = entity->m_components;

	std::string newName = "(copy" + std::to_string(copyEntity->id()) + ")";
	if (copyEntity->hasComponent<CName>())
	{
		newName = entity->getComponent<CName>().name + newName;
		copyEntity->addComponent<CName>(newName);
	}
	else { entity->addComponent<CName>(newName); }

	for (auto& sc : entity->m_scriptables)
	{
		Scriptable newSc;
		newSc.name = sc.second.name;
		newSc.hasMapped = std::make_shared<bool>(*sc.second.hasMapped);
		newSc.destroy = std::make_shared<bool>(*sc.second.destroy);
		newSc.variableMap = sc.second.variableMap;
		copyEntity->addScriptable(newSc);
	}
	m_toAdd.push_back(copyEntity);

	if (entity->hasComponent<CChildren>())
	{
		auto& Children = entity->getComponent<CChildren>();
		copyEntity->addComponent<CChildren>();
		for (auto& e : Children.childEntities)
		{
			auto childEntity = std::any_cast<std::shared_ptr<Entity>>(e.second);
			auto child = Instantiate(childEntity);
			auto& parentTrans = copyEntity->getComponent<CTransform>();
			auto& prevChildData = child->getComponent<CParent>();
			auto& childTrans = child->getComponent<CTransform>();
			Vec2 diff = prevChildData.initialPosition - childTrans.pos;
			child->getComponent<CTransform>().pos = parentTrans.pos + diff;
			child->getComponent<CTransform>().angle = parentTrans.angle;
			child->addComponent<CParent>(copyEntity->id(), copyEntity->tag(), parentTrans.pos, parentTrans.scale, parentTrans.angle);
			copyEntity->getComponent<CChildren>().children.push_back({ child->id(), child->tag() });
			copyEntity->getComponent<CChildren>().childEntities[child->id()] = MakeEntityCopy(child);
		}
	}
	return copyEntity;
}

std::shared_ptr<Entity> EntityManager::Instantiate(const std::shared_ptr<Entity>& entity, const CTransform& transform)
{
	if (!entity) return nullptr;
	std::shared_ptr<Entity> copyEntity = std::shared_ptr<Entity>(new Entity(m_totalEntities, entity->tag()));
	copyEntity->m_active = entity->m_active;
	copyEntity->m_dead = entity->m_dead;
	copyEntity->m_components = entity->m_components;

	float px = transform.pos.x, py = transform.pos.y;
	float scx = transform.scale.x, scy = transform.scale.y;
	float a = transform.angle;
	copyEntity->addComponent<CTransform>(Vec2(px, py), Vec2(scx, scy), a);

	std::string newName = "(copy" + std::to_string(m_totalEntities) + ")";
	if (copyEntity->hasComponent<CName>())
	{
		newName = entity->getComponent<CName>().name + newName;
		copyEntity->addComponent<CName>(newName);
	}
	else
	{
		copyEntity->addComponent<CName>(newName);
	}
	for (auto& sc : entity->m_scriptables)
	{
		Scriptable newSc;
		newSc.name = sc.second.name;
		newSc.hasMapped = std::make_shared<bool>(*sc.second.hasMapped);
		newSc.destroy = std::make_shared<bool>(*sc.second.destroy);
		newSc.variableMap = sc.second.variableMap;
		copyEntity->addScriptable(newSc);
	}
	m_totalEntities++;
	m_toAdd.push_back(copyEntity);
	return copyEntity;
}


std::shared_ptr<Entity> EntityManager::addEntity(const std::string& tag)
{
	auto e = std::shared_ptr<Entity>(new Entity(m_totalEntities++, tag));
	m_toAdd.push_back(e);
	return e;
}
void EntityManager::destroyEntity(std::shared_ptr<Entity> e)
{
	if (e == nullptr) return;
	e->destroy();
	m_toDestroy.push_back(e);
}

void EntityManager::changeTag(std::shared_ptr<Entity>& e, const std::string& newTag)
{
	if (e == nullptr || e->tag() == newTag) return;
	m_toChangeTag.push_back({ e, newTag });
}

void EntityManager::modifyEntityTag(std::shared_ptr<Entity> entity, const std::string& newTag)
{
	if (!entity) return; // Guard against null pointers
	const std::string& oldTag = entity->tag();

	if (entity->hasComponent<CChildren>())
	{
		for (auto& c : entity->getComponent<CChildren>().children)
		{
			auto& child = getEntity(c.first);
			child->getComponent<CParent>().tag = newTag;
		}
	}

	if (entity->hasComponent<CParent>())
	{
		auto& p = entity->getComponent<CParent>();
		auto& parent = getEntity(p.id);
		for (auto& c : parent->getComponent<CChildren>().children)
		{
			if (c.first == entity->id() && c.second == entity->tag())
			{
				c.second = newTag;
				break;
			}
		}
	}

	// Remove the entity from the old tag in the entity map
	auto& oldVec = m_entityMap[oldTag];
	auto it = std::remove(oldVec.begin(), oldVec.end(), entity);
	if (it != oldVec.end()) {
		oldVec.erase(it, oldVec.end());
	}

	// Add the entity to the new tag in the entity map
	m_entityMap[newTag].push_back(entity);

	// If using m_uniqueEntityMap, update the unique entity references
	auto uniqueEntity = m_uniqueEntityMap.find(entity->id());
	if (uniqueEntity != m_uniqueEntityMap.end() && uniqueEntity->second == entity) {
		m_uniqueEntityMap.erase(uniqueEntity);
		m_uniqueEntityMap[entity->id()] = entity;
	}

	// Update the entity's internal tag (if it has one)
	entity->setTag(newTag);
}

void EntityManager::update()
{
	for (auto& e : m_toChangeTag)
	{
		modifyEntityTag(e.first, e.second);
	}

	for (auto& e : m_toAdd)
	{
		m_entities.push_back(e);
		m_entityMap[(e->tag())].push_back(e);
		m_uniqueEntityMap[e->id()] = e;
	}

	for (auto& e : m_toDestroy)
	{
		if (e->hasAnyScriptable())
		{
			for (auto& sc : e->m_scriptables)
			{
				(*sc.second.destroy) = true;
			}
		}
		auto entityIt = std::find(m_entities.begin(), m_entities.end(), e);
		if (entityIt != m_entities.end()) {
			m_entities.erase(entityIt);
		}
		auto entityMapIt = std::find(m_entityMap[(e->tag())].begin(), m_entityMap[(e->tag())].end(), e);
		if (entityMapIt != m_entityMap[(e->tag())].end()) {
			m_entityMap[(e->tag())].erase(entityMapIt);
		}
		m_uniqueEntityMap.erase(e->id());
	}

	//std::cout << m_entities.size() << std::endl;

	m_toAdd.clear();
	m_toDestroy.clear();
	m_toChangeTag.clear();
}


EntityVec EntityManager::deepCopyEntityVec(const EntityVec& vec) {
	EntityVec result;
	for (const auto& entity : vec) {
		result.push_back(std::make_shared<Entity>(*entity));  // Deep copy each entity
	}
	return result;
}

void EntityManager::copyTo(EntityManager& other)
{
	if (this == &other) return;

	other.m_totalEntities = m_totalEntities;
	other.m_entities = deepCopyEntityVec(m_entities);
	other.m_entityMap.clear();
	other.m_uniqueEntityMap.clear();
	other.m_toAdd.clear();
	other.m_toChangeTag.clear();
	other.m_toDestroy.clear();

	for (auto& e : other.m_entities)
	{
		other.m_entityMap[e->tag()].push_back(e);
		other.m_uniqueEntityMap[e->id()] = e;
		std::pair<std::shared_ptr<Entity>, std::string> changeElem = { e, e->tag() };
		if (std::find(m_toChangeTag.begin(), m_toChangeTag.end(), changeElem) != m_toChangeTag.end())
		{
			other.m_toChangeTag.push_back(changeElem);
		}
		if (std::find(m_toDestroy.begin(), m_toDestroy.end(), e) != m_toDestroy.end())
		{
			other.m_toDestroy.push_back(e);
		}
	}
	other.m_toAdd = deepCopyEntityVec(m_toAdd);
}


void EntityManager::ChangeParent(std::shared_ptr<Entity>& entity, const std::shared_ptr<Entity>& parent)
{
	if (parent == nullptr) return;
	if (parent->id() == entity->id() && parent->tag() == entity->tag()) return;
	if (entity->hasComponent<CChildren>()) {
		for (auto& c : entity->getComponent<CChildren>().children)
		{
			if (c.first == parent->id() && c.second == parent->tag()) return;
		}
	}
	MakeIndependent(entity);
	auto& parentTrans = parent->getComponent<CTransform>();
	entity->addComponent<CParent>(parent->id(), parent->tag(), parentTrans.pos, parentTrans.scale, parentTrans.angle);
	if (!parent->hasComponent<CChildren>()) { parent->addComponent<CChildren>(); }
	parent->getComponent<CChildren>().children.push_back({ entity->id(), entity->tag() });
	parent->getComponent<CChildren>().childEntities[entity->id()] = MakeEntityCopy(entity);
}

void EntityManager::MakeIndependent(const std::shared_ptr<Entity>& entity)
{
	if (entity->hasComponent<CParent>())
	{
		auto& eParent = entity->getComponent<CParent>();
		auto& eParentEntity = getEntity(eParent.id);
		auto& ePPChildren = eParentEntity->getComponent<CChildren>().children;
		auto& ePPChildEntities = eParentEntity->getComponent<CChildren>().childEntities;
		auto it = std::find(ePPChildren.begin(), ePPChildren.end(), std::make_pair(entity->id(), entity->tag()));
		if (it != ePPChildren.end()) {
			auto& value = it->first;
			ePPChildEntities.erase(value);
			ePPChildren.erase(it);
		}
		if (ePPChildren.empty()) eParentEntity->removeComponent<CChildren>();
		entity->removeComponent<CParent>();
	}
}

void EntityManager::DeleteEntity(const std::shared_ptr<Entity>& entity)
{
	if (!entity) return;
	MakeIndependent(entity);
	if (entity->hasComponent<CChildren>())
	{
		auto& eChildren = entity->getComponent<CChildren>();
		for (int i = 0; i < eChildren.children.size(); i++)
		{
			auto& c = eChildren.children[i];
			auto& cEntity = getEntity(c.first);
			DeleteEntity(cEntity);
			i--;
		}
	}
	destroyEntity(entity);
}


void EntityManager::Lua(sol::state& lua)
{
	lua.new_usertype<EntityVec>("EntityVec",
		"size", &EntityVec::size,                      // Access size of the vector
		"get", [](EntityVec& vec, size_t i) {          // Get element by index
			return i < vec.size() ? vec[i] : nullptr;  // Return nullptr if out of bounds
		}
	);
	// Bind EntityManager and its methods
	lua.new_usertype<EntityManager>("EntityManager",
		"getEntityId", &EntityManager::safeGetEntity,
		"getEntityName", &EntityManager::getEntityName,
		"getEntities", sol::overload(
			static_cast<EntityVec & (EntityManager::*)()>(&EntityManager::getEntities),
			static_cast<EntityVec & (EntityManager::*)(const std::string&)>(&EntityManager::getEntities)
		),
		"changeTag", &EntityManager::changeTag,
		"Copy", &EntityManager::MakeEntityCopy,
		"Destroy", &EntityManager::DeleteEntity,
		"Instantiate", sol::overload(
			static_cast<std::shared_ptr<Entity>(EntityManager::*)(const std::shared_ptr<Entity>&)>(&EntityManager::Instantiate),
			static_cast<std::shared_ptr<Entity>(EntityManager::*)(const std::shared_ptr<Entity>&, const CTransform&)>(&EntityManager::Instantiate)
		)
	);
}