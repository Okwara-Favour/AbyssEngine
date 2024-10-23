#pragma once
#include <sol/sol.hpp>
#include "Collision.h"

class Editor;
class ScriptManager;

class PhysicsManager
{
	Collision collision;
protected:
	friend ScriptManager;
	void OnLuaCollision(Editor& editor, Scriptable& script, GameObject& entity);
	void OnLuaCollisionEnter(Editor& editor, Scriptable& script, GameObject& entity);
	void OnLuaCollisionExit(Editor& editor, Scriptable& script, GameObject& entity);
public:
	void Update(EntityManager& EM, std::shared_ptr<Entity>& entity);
};

