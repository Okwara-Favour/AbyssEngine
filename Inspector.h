#pragma once
#include "AbstractEngineTab.hpp"

namespace fs = std::filesystem;

class Inspector : public AbstractEngineTab
{
	std::vector<std::string> Tags = { "Default", "Player", "MainCamera", "Projectile", "Obstacle", "Enemy"};
	std::vector<std::string> CList = {"Renderer", "BoxCollider", "CircleCollider", "Camera"};
	void handleTags(Editor& editor);
	void handleComponents(Editor& editor);
	void displayComponents(Editor& editor);
public:
	void Init(Editor& editor) override;
	void Update(Editor& editor) override;
};
