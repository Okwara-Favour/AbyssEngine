#include "PhysicsManager.h"
#include "../Editor.h"

void PhysicsManager::OnLuaCollisionEnter(Editor& editor, Scriptable& script, GameObject& entity)
{
	sol::function OnCollisionEnter = (script.scriptClass)["OnCollisionEnter"];
	if (OnCollisionEnter.valid()) {
		try
		{
			for (auto& colId : entity->firstCollisionIds)
			{
				auto& collidedEntity = editor.entityManager.getEntity(colId.first);
				if (collidedEntity && !colId.second)
				{
					(script.instance)["entity"] = entity;
					sol::table OnColEnterTab = OnCollisionEnter(script.instance, collidedEntity);
					(script.instance) = OnColEnterTab;
					colId.second = true;
				}
			}
		}
		catch (const sol::error& e) {
			editor.ConsoleText("Lua error in OnCollision: " + std::string(e.what()));
		}
	}
}
void PhysicsManager::OnLuaCollisionExit(Editor& editor, Scriptable& script, GameObject& entity)
{
	sol::function OnCollisionExit = (script.scriptClass)["OnCollisionExit"];
	if (OnCollisionExit.valid()) {
		try
		{
			for (auto iter = entity->firstCollisionIds.begin(); iter != entity->firstCollisionIds.end();)
			{
				auto& collidedEntity = editor.entityManager.getEntity(iter->first);
				if (!collidedEntity)
				{
					iter = entity->firstCollisionIds.erase(iter);
				}
				else
				{
					if (!collision.checkCollison(entity, collidedEntity) && iter->second)
					{
						(script.instance)["entity"] = entity;
						sol::table OnColExitTab = OnCollisionExit(script.instance, collidedEntity);
						(script.instance) = OnColExitTab;
						iter = entity->firstCollisionIds.erase(iter);
					}
					else {
						++iter;
					}
				}
			}
		}
		catch (const sol::error& e) {
			editor.ConsoleText("Lua error in OnCollisionExit: " + std::string(e.what()));
		}
	}
}

void PhysicsManager::OnLuaCollision(Editor& editor, Scriptable& script, GameObject& entity)
{
	sol::function OnCollision = (script.scriptClass)["OnCollision"];
	if (OnCollision.valid()) {
		try
		{
			for (auto& colId : entity->firstCollisionIds)
			{
				auto& collidedEntity = editor.entityManager.getEntity(colId.first);
				if (collidedEntity && collision.checkCollison(entity, collidedEntity))
				{
					(script.instance)["entity"] = entity;
					sol::table OnColTab = OnCollision(script.instance, collidedEntity);
					(script.instance) = OnColTab;
				}
			}
		}
		catch (const sol::error& e) {
			editor.ConsoleText("Lua error in OnCollision: " + std::string(e.what()));
		}
	}
	
}

void PhysicsManager::Update(EntityManager& EM, std::shared_ptr<Entity>& entity)
{
	if (!(entity->hasComponent<CBoxCollider>() || entity->hasComponent<CCircleCollider>()))
	{
		return;
	}
	for (auto& e : EM.getEntities())
	{
		if (!(e->hasComponent<CBoxCollider>() || e->hasComponent<CCircleCollider>()))
		{
			continue;
		}
		if (e->id() == entity->id()) continue;

		if (collision.checkCollison(entity, e))
		{
			auto& ids = entity->firstCollisionIds;
			if (ids.find(e->id()) == ids.end())
			{
				ids[e->id()] = false;
			}
		}
	}
}