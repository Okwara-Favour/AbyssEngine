#pragma once
#include "AbstractEngineTab.hpp"

class EngineSettings : public AbstractEngineTab
{
	std::vector<std::string> fileList = { "Save" };
	std::vector<std::string> editList = { "Undo", "Redo" };
	std::vector<std::string> gameObjectList = { "Add", "Remove" };
	std::vector<std::string> addList = { "Rectangle" };
	std::vector<std::string> winList = { "Close", "FullScreen"};
	void createEntity(Editor& editor, const std::string& type);
public:
	void Init(Editor& editor) override;
	void Update(Editor& editor) override;
};

