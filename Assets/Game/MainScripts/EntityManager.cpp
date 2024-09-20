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

std::shared_ptr<Entity>& EntityManager::getEntity(const size_t id, const std::string& tag)
{
	std::string uniqueTag = tag + std::to_string(id);
	 //if you want safety
	/*auto it = m_uniqueEntityMap.find(uniqueTag);
	if (it != m_uniqueEntityMap.end()) {
		return it->second;
	}
	else {
		throw std::runtime_error("Entity not found");
	}*/
	return m_uniqueEntityMap[uniqueTag];
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
			auto& child = getEntity(c.first, c.second);
			child->getComponent<CParent>().tag = newTag;
		}
	}

	if (entity->hasComponent<CParent>())
	{
		auto& p = entity->getComponent<CParent>();
		auto& parent = getEntity(p.id, p.tag);
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
	std::string oldUniqueTag = oldTag + std::to_string(entity->id());
	auto uniqueEntity = m_uniqueEntityMap.find(oldUniqueTag);
	if (uniqueEntity != m_uniqueEntityMap.end() && uniqueEntity->second == entity) {
		m_uniqueEntityMap.erase(uniqueEntity);
		m_uniqueEntityMap[newTag + std::to_string(entity->id())] = entity;
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
		m_uniqueEntityMap[e->tag() + std::to_string(e->id())] = e;
	}

	for (auto& e : m_toDestroy)
	{
		auto entityIt = std::find(m_entities.begin(), m_entities.end(), e);
		if (entityIt != m_entities.end()) {
			m_entities.erase(entityIt);
		}
		auto entityMapIt = std::find(m_entityMap[(e->tag())].begin(), m_entityMap[(e->tag())].end(), e);
		if (entityMapIt != m_entityMap[(e->tag())].end()) {
			m_entityMap[(e->tag())].erase(entityMapIt);
		}
		m_uniqueEntityMap.erase(e->tag() + std::to_string(e->id()));
	}

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
	for (auto& e : other.m_entities)
	{
		other.m_entityMap[e->tag()].push_back(e);
		other.m_uniqueEntityMap[e->tag() + std::to_string(e->id())] = e;
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