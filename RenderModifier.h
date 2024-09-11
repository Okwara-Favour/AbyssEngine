#pragma once
#include "AbstractEngineTab.hpp"

class Entity;

class RenderModifier : public AbstractEngineTab
{
	void SetTransform(std::shared_ptr<Entity>& entity);
public:
	void Init(Editor& editor) override;
	void Update(Editor& editor) override;
};
