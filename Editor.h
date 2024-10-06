#pragma once
#include <fstream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <deque>

extern "C"
{
#include "LUA/include/lauxlib.h"
#include "LUA/include/lua.h"
#include "LUA/include/lualib.h"
}

#ifdef _WIN64
#pragma comment(lib, "LUA/lua54.lib")
#endif

#include <sol/sol.hpp>

#include "imgui.h"
#include "imgui-SFML.h"
#include "NLOHMANN/json.hpp"
#include "AbstractEngineTab.hpp"
#include "Inspector.h"
#include "EngineSettings.h"
#include "Console.h"
#include "Files.h"
#include "Hierarchy.h"
#include "Display.h"
#include "RenderModifier.h"
#include "Assets/Game/MainScripts/EntityManager.h"
#include "ScriptManager.h"
#include "EventListener.hpp"

class Command
{
	std::vector<std::pair<EntityManager, std::shared_ptr<Entity>>> EMHistory;
	std::vector<std::pair<EntityManager, std::shared_ptr<Entity>>> EMHistoryRecover;
	std::pair<EntityManager, std::shared_ptr<Entity>> EM;
	bool check = false;
public:
	void Save(EntityManager& em, std::shared_ptr<Entity>& e);
	void Undo(EntityManager& em, std::shared_ptr<Entity>& e);
	void Redo(EntityManager& em, std::shared_ptr<Entity>& e);
	void Execute(EntityManager& em, std::shared_ptr<Entity>& e);
	void Clear();
};

namespace fs = std::filesystem;
class Editor
{
	std::map<std::string, std::unique_ptr<AbstractEngineTab>> engineTabs;
	sf::RenderWindow	window;
	sf::Clock			deltaClock;
	Command				command;
	bool				close = false;
	bool				load = false;
	bool				fullScreen = false;
	bool				isFullScreen = false;
	EntityManager		EMCOPY;
protected:
	friend Files;
	friend EngineSettings;
	friend Inspector;
	friend Display;
	friend Hierarchy;
	friend RenderModifier;
	float duration = 0.25;
	float translateFactor = 1.0f;
	float scaleFactor = 0.5f;
	float rotateFactor = 0.5f;
	ScriptManager scriptManager;
	std::shared_ptr<Entity> parentEntity = nullptr;
	fs::path currentDirectory = fs::current_path();
	std::map<std::string, std::string> texturePathMap;
	std::map<std::string, sf::Texture> textureMap;
	std::map<std::string, Animation> animationMap;
	std::vector<Animation> anim_toAdd;
	std::vector<Animation> anim_toRemove;
	void addAnimation(const Animation& anim);
	void removeAnimation(const Animation& anim);
	void updateAnimation();
public:
	bool					gameMode = false;
	ImVec2					startPosition;
	std::shared_ptr<Entity> selectedEntity = nullptr;
	EntityManager			entityManager;
	EventListener			eventListener;
	Editor();
	const sf::Vector2u WinSize() { return window.getSize(); }
	void Init();
	void ProcessEvent(sf::Event& event);
	void Render();
	void CloseTabs();
	void MainPage();
	void Update();
	void Save();
	void Undo();
	void Redo();
	void Run();
	void ConsoleText(const std::string& error);
	void AddChildEntitiesToSceneFile(nlohmann::json& dict, const std::shared_ptr<Entity> parent);
	void LoadChildEntitiesFromSceneFile(const nlohmann::json& dict, const std::shared_ptr<Entity> parent);
	void SaveScene();
	void LoadScene();
	void CloseEditor();
	void ToggleFullScreen();
	void StartGame();
	void QuitGame();
	const bool HasClosed() const;
	const bool FullScreen() const;
	bool isMouseInTab();
};