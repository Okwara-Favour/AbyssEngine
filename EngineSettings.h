#pragma once
#include "AbstractEngineTab.hpp"

namespace fs = std::experimental::filesystem;
class Entity;

class EngineSettings : public AbstractEngineTab
{
	std::vector<std::string> fileList = {"Import", "Save", "Load"};
	std::vector<std::string> editList = { "Undo", "Redo" };
	std::vector<std::string> gameObjectList = { "Set Parent Candidate", "Add", "Remove" };
	std::vector<std::string> addList = { "Rectangle" };
	std::vector<std::string> winList = { "Close", "FullScreen"};
	void ChangeParent(Editor& editor, std::shared_ptr<Entity>& entity, const std::shared_ptr<Entity>& parent);
	void MakeIndependent(Editor& editor, const std::shared_ptr<Entity>& entity);
	void DeleteEntity(Editor& editor, const std::shared_ptr<Entity>& entity);
	void createEntity(Editor& editor, const std::string& type);
	void ImportFiles(Editor& editor);
public:
	void Init(Editor& editor) override;
	void Update(Editor& editor) override;
};

