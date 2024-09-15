#include "GameEngine.h"
#include "Scene_Menu.h"
#include "Scene_Play.h"
#include "Scene_Debug.h"

GameEngine::GameEngine(const std::string path)
{
	init(path);
}

void GameEngine::init(const std::string path)
{
	m_assets.loadFromFile(path);
	m_window.create(sf::VideoMode(1280, 640), "Eat or Die", sf::Style::Titlebar);
	m_window.setFramerateLimit(60);
	ImGui::SFML::Init(m_window);
	changeScene("MENU", std::make_shared<Scene_Menu>(this));
}
void GameEngine::update()
{
	currentScene()->update();
}
void GameEngine::sUserInput()
{
	sf::Event event;
	while (m_window.pollEvent(event))
	{
		ImGui::SFML::ProcessEvent(event);
		if (event.type == sf::Event::Closed) {
			quit();
		}

		if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased)
		{
			if (currentScene()->getActionMap().find(event.key.code) == currentScene()->getActionMap().end())
			{
				continue; 
			}

			const std::string actionType = (event.type == sf::Event::KeyPressed) ? "START" : "END";

			currentScene()->doAction(Action(currentScene()->getActionMap().at(event.key.code), actionType));
		}

		auto mousePos = sf::Mouse::getPosition(m_window);
		m_mouse.m_pos = Vec2(mousePos.x, mousePos.y);

		if (event.type == sf::Event::MouseButtonPressed)
		{
			if (!ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow)) {
				switch (event.mouseButton.button)
				{
				case sf::Mouse::Left: { m_mouse.m_leftClick = true;	m_mouse.m_leftPress = true;	break; }
				case sf::Mouse::Middle: { m_mouse.m_middleClick = true; m_mouse.m_middlePress = true; break; }
				case sf::Mouse::Right: { m_mouse.m_rightClick = true;	m_mouse.m_rightPress = true; break; }
				default: break;
				}
			}
		}

		if (event.type == sf::Event::MouseButtonReleased)
		{
			switch (event.mouseButton.button)
			{
			case sf::Mouse::Left:	{ m_mouse.m_leftPress = false;		break; }
			case sf::Mouse::Middle: { m_mouse.m_middlePress = false;	break; }
			case sf::Mouse::Right:	{ m_mouse.m_rightPress = false;		break; }
			default: break;
			}
		}

		if (event.type == sf::Event::MouseMoved)
		{
			m_mouse.m_mouseMove = true;
		}
	}
}

std::shared_ptr<Scene> GameEngine::currentScene()
{
	return m_scenes[m_currentScene];
}

void GameEngine::changeScene(const std::string& sceneName, std::shared_ptr<Scene> scene, bool endCurrentScene)
{
	if (scene)
	{
		m_scenes[sceneName] = scene;
	}
	else
	{
		if (m_scenes.find(sceneName) == m_scenes.end())
		{
			std::cerr << "Warning: Scene does not exist: " << sceneName << std::endl;
			return;
		}
	}

	if (endCurrentScene)
	{
		m_scenes.erase(m_scenes.find(m_currentScene));
	}

	m_currentScene = sceneName;
}

void GameEngine::quit()
{
	m_running = false;
}
void GameEngine::run()
{
	while (isRunning())
	{
		sUserInput();
		update();
		resetMouseClick();
	}
}

void GameEngine::resetMouseClick()
{
	m_mouse.m_leftClick = false;
	m_mouse.m_middleClick = false;
	m_mouse.m_rightClick = false;
	m_mouse.m_mouseMove = false;
}

sf::RenderWindow& GameEngine::window()
{
	return m_window;
}

Assets& GameEngine::assets()
{
	return m_assets;
}

bool GameEngine::isRunning()
{
	return m_running && m_window.isOpen();
}
const Vec2& GameEngine::mousePos() const
{
	return m_mouse.m_pos;
}
const bool GameEngine::mouseLeftClick() const
{
	return m_mouse.m_leftClick;
}
const bool GameEngine::mouseMiddleClick() const
{
	return m_mouse.m_middleClick;
}
const bool GameEngine::mouseRightClick() const
{
	return m_mouse.m_rightClick;
}
const bool GameEngine::mouseLeftPress() const
{
	return m_mouse.m_leftPress;
}
const bool GameEngine::mouseMiddlePress() const
{
	return m_mouse.m_middlePress;
}
const bool GameEngine::mouseRightPress() const
{
	return m_mouse.m_rightPress;
}
const bool GameEngine::mouseMoved() const
{
	return m_mouse.m_mouseMove;
}

const Vec2& GameEngine::defaultWindowSize() const
{
	return defaultSize;
}