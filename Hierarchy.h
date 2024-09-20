#pragma once
#include "AbstractEngineTab.hpp"

class Entity;

class Hierarchy : public AbstractEngineTab
{
	sf::Clock clock;
	float duration = 0.25f;
	int clickCount = 0;
	std::map<ImGuiKey, std::string> keyAction;
	char nameBuffer[50] = {};
	std::shared_ptr<Entity> editingEntity = nullptr;
	void editEntityName(Editor& editor, std::shared_ptr<Entity> entity);
	void ParentChildDropdown(Editor& editor, std::shared_ptr<Entity> entity);
public:
	void Init(Editor& editor) override;
	void Update(Editor& editor) override;
	void DisplayEntities(Editor& editor);
};

