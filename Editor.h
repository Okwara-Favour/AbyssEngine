#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <deque>
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

class Command
{
	std::vector<EntityManager> EMHistory;
	std::vector<EntityManager> EMHistoryRecover;
	EntityManager EM;
	bool check = false;
public:
	void Save(EntityManager& em)
	{
		EMHistoryRecover.clear();
		size_t maxUndoDepth = 20;
		if (EMHistory.size() >= maxUndoDepth)
		{
			EMHistory.erase(EMHistory.begin());
		}
		EMHistory.push_back(em);
	}
	void Undo(EntityManager& em)
	{
		if (!EMHistory.empty())
		{
			check = true;
			EM = EMHistory.back();
			EMHistoryRecover.push_back(em);
			EMHistory.pop_back();
		}
	}

	void Redo(EntityManager& em)
	{
		if (!EMHistoryRecover.empty())
		{
			check = true;
			EM = EMHistoryRecover.back();
			EMHistory.push_back(em);
			EMHistoryRecover.pop_back();
		}
	}

	void Execute(EntityManager& em)
	{
		if (check)
		{
			em = EM;
			check = false;
		}
	}
};

class Editor
{
	std::vector<std::unique_ptr<AbstractEngineTab>> engineTabs;
	sf::RenderWindow* window = nullptr;
	sf::Clock deltaClock;
public:
	Command command;
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

