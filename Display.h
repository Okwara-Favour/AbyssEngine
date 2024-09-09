#pragma once
#include "AbstractEngineTab.hpp"
#include "SFML/Graphics.hpp"

class Display : public AbstractEngineTab
{
	sf::RenderTexture displayTexture;
public:
	void Init(Editor& editor) override;
	void Update(Editor& editor) override;
};

