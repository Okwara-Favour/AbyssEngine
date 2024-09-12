#pragma once
#include "AbstractEngineTab.hpp"

class Console : public AbstractEngineTab
{
	std::vector<std::string> errorStack;
	int maxError = 999;
public:
	void Init(Editor& editor) override;
	void Update(Editor& editor) override;
	void HandleError(const std::string& errorString) override;
};
