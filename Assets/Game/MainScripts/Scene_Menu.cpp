#include "Scene_Menu.h"
#include "GameEngine.h"
#include "Scene_Play.h"
#include "Scene_Debug.h"

Scene_Menu::Scene_Menu(GameEngine* gameEngine)
	:Scene(gameEngine)
{
	init();
}

void Scene_Menu::init()
{
	registerAction(sf::Keyboard::W,			"UP");
	registerAction(sf::Keyboard::S,			"DOWN");
	registerAction(sf::Keyboard::D,			"PLAY");
	registerAction(sf::Keyboard::Escape,	"QUIT");

	m_title = "Menu";

	m_menuStrings.push_back("New Game");
	m_menuStrings.push_back("Load Game");
	m_menuStrings.push_back("Settings");
	m_menuStrings.push_back("Quit");

	m_levelPaths.push_back("Levels/level1.txt");
	m_levelPaths.push_back("Levels/level2.txt");
	m_levelPaths.push_back("Levels/level3.txt");

	sf::Text text;
	text.setFont(m_game->assets().getFont("Arial"));
	text.setCharacterSize(64);
	text.scale(width() / m_game->defaultWindowSize().x, height() / m_game->defaultWindowSize().y);

	for (int i = 0; i < m_menuStrings.size(); i++)
	{
		m_menuTexts.push_back(text);
	}
	m_menuTexts[0].setFillColor(sf::Color::Red);
}

void Scene_Menu::update()
{
	m_entityManager.update();
	sDoRender();
}

void Scene_Menu::moveUpMenu()
{
	m_menuTexts[m_selectedMenuIndex].setFillColor(sf::Color::White);
	m_selectedMenuIndex = (m_selectedMenuIndex - 1) % m_menuStrings.size();
	m_menuTexts[m_selectedMenuIndex].setFillColor(sf::Color::Red);
}
void Scene_Menu::moveDownMenu()
{
	m_menuTexts[m_selectedMenuIndex].setFillColor(sf::Color::White);
	m_selectedMenuIndex = (m_selectedMenuIndex + 1) % m_menuStrings.size();
	m_menuTexts[m_selectedMenuIndex].setFillColor(sf::Color::Red);
}

void Scene_Menu::sDoAction(const Action& action)
{
	if (action.type() == "START")
	{
		if (action.name() == "UP")
		{
			moveUpMenu();
		}
		else if (action.name() == "DOWN")
		{
			moveDownMenu();
		}
		else if (action.name() == "PLAY")
		{
			if (m_menuStrings[m_selectedMenuIndex] == "New Game")
			{
				changeLevel("levels/level0.txt");
				std::cout << getLevel() << std::endl;
				m_game->changeScene("PLAY", std::make_shared<Scene_Play>(m_game, getLevel()));
			}
		}
		else if (action.name() == "QUIT")
		{
			onEnd();
		}
	}

}

void Scene_Menu::sDoRender()
{
	m_game->window().clear(sf::Color::Blue);
	
	int startPosX = width()/2;
	int startPosY = height()/4;
	for (int i = 0; i < m_menuStrings.size(); i++)
	{
		sf::Vector2f s = m_menuTexts[i].getGlobalBounds().getSize();
		m_menuTexts[i].setString(m_menuStrings[i]);
		m_menuTexts[i].setPosition(startPosX, startPosY);
		m_menuTexts[i].setOrigin(s.x/2, s.y/2);
		m_game->window().draw(m_menuTexts[i]);
		startPosY += height()/8;
	}

	m_game->window().display();
}

void Scene_Menu::onEnd()
{
	m_game->quit();
}