#pragma once
#include "AbstractEngineTab.hpp"
#include "SFML/Graphics.hpp"

class Entity;

class Display : public AbstractEngineTab
{
	sf::RenderTexture displayTexture;
	bool entityMouse = false;
	ImVec2 windowPos;
	ImVec2 contentMin;
	ImVec2 contentMax;
	ImVec2 viewPos;
	ImVec2 viewScale;
	std::map<ImGuiKey, std::string> keyAction;
	void DisplaySelected(std::shared_ptr<Entity>& entity, std::shared_ptr<Entity>& selected);
	bool EntityContainsPos(std::shared_ptr<Entity>& entity, sf::Vector2f& pos);
	sf::Vector2f WorldPos(float x, float y);
	void EntityMouseDrag(std::shared_ptr<Entity>& entity, Editor& editor);
	void HandleKeyActions();
	void MenuTab(Editor& editor);
	bool isMouseInTab();
public:
	void Init(Editor& editor) override;
	void Update(Editor& editor) override;
};

