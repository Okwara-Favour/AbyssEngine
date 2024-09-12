#pragma once
#include "AbstractEngineTab.hpp"

class Entity;

class Hierarchy : public AbstractEngineTab
{
	std::map<ImGuiKey, std::string> keyAction;
	std::string name = "";
	std::shared_ptr<Entity> editingEntity = nullptr;
public:
	void Init(Editor& editor) override;
	void Update(Editor& editor) override;
	void DisplayEntities(Editor& editor);
};

