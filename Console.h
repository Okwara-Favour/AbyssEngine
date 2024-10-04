#pragma once
#include "AbstractEngineTab.hpp"

class Console : public AbstractEngineTab
{
	std::vector<std::string> errorStack;
	const int maxError = 999;
public:
	void Init(Editor& editor) override;
	void Update(Editor& editor) override;
	void ConsoleText(const std::string& errorString) override;
};
