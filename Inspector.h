#pragma once
#include "AbstractEngineTab.hpp"

class Inspector : public AbstractEngineTab
{
public:
	void Init(Editor& editor) override;
	void Update(Editor& editor) override;
};
