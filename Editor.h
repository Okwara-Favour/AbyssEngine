#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "imgui.h"
#include "imgui-SFML.h"
#include "AbstractEngineTab.hpp"
#include "Inspector.h"
#include "EngineSettings.h"
#include "Console.h"
#include "Files.h"
#include "Hierarchy.h"
#include "Display.h"
#include "Assets/MainScripts/EntityManager.h"

class Editor
{
	std::vector<std::unique_ptr<AbstractEngineTab>> engineTabs;
	sf::RenderWindow* window = nullptr;
	sf::Clock deltaClock;
public:
	std::shared_ptr<Entity> selectedEntity = nullptr;
	EntityManager entityManager;
	Editor();
	Editor(sf::RenderWindow& w);
	void Init();
	void ProcessEvent(sf::Event& event);
	void Render();
	void Close();
	void MainPage();
	void Update();
};

