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
	/* //if you want safety
	auto it = m_uniqueEntityMap.find(uniqueTag);
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

	// Remove the entity from the old tag in the entity map
	auto& oldVec = m_entityMap[oldTag];
	auto it = std::remove(oldVec.begin(), oldVec.end(), entity);
	if (it != oldVec.end()) {
		oldVec.erase(it, oldVec.end());
	}

	// Add the entity to the new tag in the entity map
	m_entityMap[newTag].push_back(entity);

	// If using m_uniqueEntityMap, update the unique entity references
	auto uniqueEntity = m_uniqueEntityMap.find(oldTag);
	if (uniqueEntity != m_uniqueEntityMap.end() && uniqueEntity->second == entity) {
		m_uniqueEntityMap.erase(uniqueEntity);
		m_uniqueEntityMap[newTag] = entity;
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

EntityManager& EntityManager::operator=(const EntityManager& other) {
	if (this != &other) {
		// Perform deep copying of each vector/map
		m_entities = deepCopyEntityVec(other.m_entities);
		m_entityMap = deepCopyEntityMap(other.m_entityMap);
		m_uniqueEntityMap = deepCopyUniqueEntityMap(other.m_uniqueEntityMap);
		m_toAdd = deepCopyEntityVec(other.m_toAdd);
		m_toDestroy = deepCopyEntityVec(other.m_toDestroy);
		m_toChangeTag = deepCopyEntityStringPair(other.m_toChangeTag);
		m_totalEntities = other.m_totalEntities;
	}
	return *this;
}

EntityVec EntityManager::deepCopyEntityVec(const EntityVec& vec) {
	EntityVec result;
	for (const auto& entity : vec) {
		result.push_back(std::make_shared<Entity>(*entity));  // Deep copy each entity
	}
	return result;
}

EntityMap EntityManager::deepCopyEntityMap(const EntityMap& map) {
	EntityMap result;
	for (const auto& pair : map) {
		result[pair.first] = deepCopyEntityVec(pair.second);
	}
	return result;
}

UniqueEntityMap EntityManager::deepCopyUniqueEntityMap(const UniqueEntityMap& map) {
	UniqueEntityMap result;
	for (const auto& pair : map) {
		result[pair.first] = std::make_shared<Entity>(*pair.second);  // Deep copy each entity
	}
	return result;
}

EntityStringPair EntityManager::deepCopyEntityStringPair(const EntityStringPair& pairVec) {
	EntityStringPair result;
	for (const auto& pair : pairVec) {
		result.emplace_back(std::make_shared<Entity>(*pair.first), pair.second);  // Deep copy each entity
	}
	return result;
}