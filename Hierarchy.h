#pragma once
#include "AbstractEngineTab.hpp"

class Hierarchy : public AbstractEngineTab
{
public:
	void Init(Editor& editor) override;
	void Update(Editor& editor) override;
};

