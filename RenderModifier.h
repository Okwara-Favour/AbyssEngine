#pragma once
#include "AbstractEngineTab.hpp"

class Entity;

class RenderModifier : public AbstractEngineTab
{
	float minScale = 0.0001;
	void ParentChild(Editor& editor, std::shared_ptr<Entity>& entity);
	void SetTransform(std::shared_ptr<Entity>& entity);
public:
	void Init(Editor& editor) override;
	void Update(Editor& editor) override;
};
