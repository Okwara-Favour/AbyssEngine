#pragma once
#include <SFML/Graphics.hpp>
#include "imgui.h"
#include "imgui-SFML.h"
#include <filesystem>

class Editor;

class AbstractEngineTab
{
public:
	virtual ~AbstractEngineTab() = default;
	virtual void Init(Editor& editor) = 0;
	virtual void Update(Editor& editor) = 0;
	virtual void ConsoleText(const std::string& errorString) {};
	virtual void OnStartGame() {};
	virtual void OnEndGame() {};
};
