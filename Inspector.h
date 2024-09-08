#pragma once
#include "AbstractEngineTab.hpp"

class Inspector : public AbstractEngineTab
{
	std::vector<std::string> Tags = { "Default", "Player" };
	void handleTags(Editor& editor);
public:
	void Init(Editor& editor) override;
	void Update(Editor& editor) override;
};
