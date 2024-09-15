#pragma once
#include "Scene.h"
#include <vector>


class Scene_Menu:public Scene
{
protected:
	std::string					m_title;
	std::vector<std::string>	m_menuStrings;
	std::vector<std::string>	m_levelPaths;
	std::vector<sf::Text>		m_menuTexts;
	int							m_selectedMenuIndex = 0;

	void init();
	void update();
	void onEnd();
	void sDoAction(const Action& action);

	void moveUpMenu();
	void moveDownMenu();

public:
	Scene_Menu(GameEngine * gameEngine = nullptr);
	void sDoRender();
	//void doAction(const Action& action);
};

