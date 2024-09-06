#pragma once
#include "AbstractEngineTab.hpp"

class Display : public AbstractEngineTab
{
public:
	void Init(Editor& editor) override;
	void Update(Editor& editor) override;
};

