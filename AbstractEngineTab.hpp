#pragma once
#include <SFML/Graphics.hpp>
#include "imgui.h"
#include "imgui-SFML.h"

class Editor;

class AbstractEngineTab
{
public:
	virtual ~AbstractEngineTab() = default;
	virtual void Init(Editor& editor) = 0;
	virtual void Update(Editor& editor) = 0;
};
