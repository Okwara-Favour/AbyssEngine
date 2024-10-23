#pragma once
#include "AbstractEngineTab.hpp"
#include "SFML/Graphics.hpp"
#include <chrono>

class Entity;

class Display : public AbstractEngineTab
{
	sf::Clock clock;
	float duration = 0.25;
	sf::View mainViewPort;
	sf::RectangleShape mainRect;
	sf::Vector2f m_stackedMoveChange;
	float m_stackedZoomChange = 1.0f;
	sf::Color editorViewColor = sf::Color(100, 100, 100);
	sf::Color viewColor = sf::Color(100, 100, 100);
	bool entityClicked = false;
	ImVec2 windowPos;
	ImVec2 contentMin;
	ImVec2 contentMax;
	ImVec2 viewPos;
	ImVec2 viewScale;
	std::map<ImGuiKey, std::string> keyAction;
	void DisplaySelectedBound(Editor& editor);
	void DisplaySelected(std::shared_ptr<Entity>& entity, Editor& editor);
	bool EntityContainsPos(std::shared_ptr<Entity>& entity, sf::Vector2f& pos);
	sf::Vector2f WorldPos(Editor& editor, float x, float y);
	void EntityMouseDrag(std::shared_ptr<Entity>& entity, Editor& editor);
	void HandleKeyActions(Editor& editor);
	void MenuTab(Editor& editor);
public:
	void Init(Editor& editor) override;
	void Update(Editor& editor) override;
};

