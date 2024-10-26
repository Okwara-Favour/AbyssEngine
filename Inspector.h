#pragma once
#include "AbstractEngineTab.hpp"

namespace fs = std::filesystem;

class Entity;

class Inspector : public AbstractEngineTab
{
	std::vector<std::string> Tags = { "Default", "Player", "MainCamera", "Projectile", "Obstacle", "Enemy"};
	std::vector<std::string> CList = {"Renderer", "BoxCollider", "CircleCollider", "Camera"};
	void handleTags(Editor& editor, std::shared_ptr<Entity>& selected);
	void handleComponents(Editor& editor, std::shared_ptr<Entity>& selected);
	void displayComponents(Editor& editor, std::shared_ptr<Entity>& selected);
public:
	void Init(Editor& editor) override;
	void Update(Editor& editor) override;
};
