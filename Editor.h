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
	std::vector<std::pair<EntityManager, std::shared_ptr<Entity>>> EMHistory;
	std::vector<std::pair<EntityManager, std::shared_ptr<Entity>>> EMHistoryRecover;
	std::pair<EntityManager, std::shared_ptr<Entity>> EM;
	bool check = false;
public:
	void Save(EntityManager& em, std::shared_ptr<Entity>& e)
	{
		EMHistoryRecover.clear();
		size_t maxUndoDepth = 20;
		if (EMHistory.size() >= maxUndoDepth)
		{
			EMHistory.erase(EMHistory.begin());
		}
		EMHistory.push_back({ em, (e) ? std::make_shared<Entity>(*e) : nullptr });
	}
	void Undo(EntityManager& em, std::shared_ptr<Entity>& e)
	{
		if (!EMHistory.empty())
		{
			check = true;
			EM = EMHistory.back();
			EMHistoryRecover.push_back({ em, (e) ? std::make_shared<Entity>(*e) : nullptr });
			EMHistory.pop_back();
		}
	}

	void Redo(EntityManager& em, std::shared_ptr<Entity>& e)
	{
		if (!EMHistoryRecover.empty())
		{
			check = true;
			EM = EMHistoryRecover.back();
			EMHistory.push_back({ em, (e)? std::make_shared<Entity>(*e) : nullptr});
			EMHistoryRecover.pop_back();
		}
	}

	void Execute(EntityManager& em, std::shared_ptr<Entity>& e)
	{
		if (check)
		{
			em = EM.first;
			e = EM.second;
			check = false;
		}
	}
};

class Editor
{
	std::vector<std::unique_ptr<AbstractEngineTab>> engineTabs;
	sf::RenderWindow* window = nullptr;
	sf::Clock deltaClock;
	Command command;
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
	void Save();
	void Undo();
	void Redo();
};

