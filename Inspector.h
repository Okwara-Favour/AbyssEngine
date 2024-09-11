#pragma once
#include "AbstractEngineTab.hpp"

class Inspector : public AbstractEngineTab
{
	std::vector<std::string> Tags = { "Default", "Player" };
	std::vector<std::string> CList = {"Renderer"};
	void handleTags(Editor& editor);
	void handleComponents(Editor& editor);
	void displayComponents(Editor& editor);
public:
	void Init(Editor& editor) override;
	void Update(Editor& editor) override;
};
