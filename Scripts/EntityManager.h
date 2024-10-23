#pragma once
#include <vector>
#include <map>
#include <memory>
#include <algorithm>
#include <string>
#include "Entity.h"

typedef std::shared_ptr<Entity> GameObject;
typedef std::vector<std::shared_ptr<Entity>> EntityVec;
typedef std::map<std::string, EntityVec> EntityMap;
typedef std::map<size_t, std::shared_ptr<Entity>> UniqueEntityMap;
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
public:
	EntityManager();
	EntityVec& getEntities();
	EntityVec& getEntities(const std::string& tag);
	std::shared_ptr<Entity> safeGetEntity(const size_t id);
	std::shared_ptr<Entity>& getEntity(const size_t id);
	std::shared_ptr<Entity> getEntityName(const std::string& name);
	std::shared_ptr<Entity> addEntity(const std::string& tag);
	std::shared_ptr<Entity> MakeEntityCopy(const std::shared_ptr<Entity>& entity);
	std::shared_ptr<Entity> Instantiate(const std::shared_ptr<Entity>& entity);
	std::shared_ptr<Entity> Instantiate(const std::shared_ptr<Entity>& entity, const CTransform& transform);
	void changeTag(std::shared_ptr<Entity>& e, const std::string& newTag);
	void destroyEntity(std::shared_ptr<Entity> e);
	void ChangeParent(std::shared_ptr<Entity>& entity, const std::shared_ptr<Entity>& parent);
	void MakeIndependent(const std::shared_ptr<Entity>& entity);
	void DeleteEntity(const std::shared_ptr<Entity>& entity);
	void update();
	void copyTo(EntityManager& other);
	static void Lua(sol::state& lua);
};
