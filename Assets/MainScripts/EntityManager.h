#pragma once
#include <vector>
#include <map>
#include <memory>
#include <algorithm>
#include <string>
#include "Entity.h"

typedef std::vector<std::shared_ptr<Entity>> EntityVec;
typedef std::map<std::string, EntityVec> EntityMap;
typedef std::map<std::string, std::shared_ptr<Entity>> UniqueEntityMap;
typedef std::vector<std::pair<std::shared_ptr<Entity>, std::string>> EntityStringPair;
class EntityManager
{
	EntityVec			m_entities;
	EntityMap			m_entityMap;
	UniqueEntityMap		m_uniqueEntityMap;
	EntityVec			m_toAdd;
	EntityVec			m_toDestroy;
	EntityStringPair	m_toChangeTag;
	size_t				m_totalEntities = 0;
	void modifyEntityTag(std::shared_ptr<Entity> entity, const std::string& newTag);
	EntityVec deepCopyEntityVec(const EntityVec& vec);
	EntityMap deepCopyEntityMap(const EntityMap& map);
	UniqueEntityMap deepCopyUniqueEntityMap(const UniqueEntityMap& map);
	EntityStringPair deepCopyEntityStringPair(const EntityStringPair& pairVec);
public:
    EntityManager(const EntityManager& other) {
        *this = other;  // Delegate to the assignment operator
    }
	EntityManager();
	EntityVec& getEntities();
	EntityVec& getEntities(const std::string& tag);
	std::shared_ptr<Entity>& getEntity(const size_t id, const std::string& tag);
	std::shared_ptr<Entity> addEntity(const std::string& tag);
	void changeTag(std::shared_ptr<Entity>& e, const std::string& newTag);
	void destroyEntity(std::shared_ptr<Entity> e);
	void update();

	EntityManager& operator=(const EntityManager& other);
};
