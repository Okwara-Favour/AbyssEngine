#pragma once
#include "Assets.h"
#include "Scene.h"
#include <SFML/Graphics.hpp>
#include "imgui.h"
#include "imgui-SFML.h"
#include <memory>


typedef std::map<std::string, std::shared_ptr<Scene>> SceneMap;
class GameEngine
{
	struct Mouse
	{
		Vec2 m_pos;
		bool m_mouseMove	= false;
		bool m_leftClick	= false;
		bool m_middleClick	= false;
		bool m_rightClick	= false;
		bool m_leftPress	= false;
		bool m_middlePress	= false;
		bool m_rightPress	= false;
		Mouse() {}
	};
protected:
	SceneMap			m_scenes;
	sf::RenderWindow	m_window;
	Assets				m_assets;
	std::string			m_currentScene = "default";
	bool				m_running = true;
	size_t				m_simulationSpeed = 1;
	Mouse				m_mouse;
	Vec2				defaultSize = Vec2(1280, 640);
	void init(const std::string path);
	void update();
	void sUserInput();
	std::shared_ptr<Scene> currentScene();
	void resetMouseClick();
public:
	GameEngine(const std::string path);
	void changeScene(const std::string& sceneName, std::shared_ptr<Scene> scene, bool endCurrentScene = false);
	void quit();
	void run();

	const Vec2& mousePos() const;
	const bool	mouseLeftClick() const;
	const bool	mouseMiddleClick() const;
	const bool	mouseRightClick() const;
	const bool	mouseLeftPress() const;
	const bool	mouseMiddlePress() const;
	const bool	mouseRightPress() const;
	const bool	mouseMoved() const;
	const Vec2& defaultWindowSize() const;
	sf::RenderWindow& window();
	Assets& assets();
	bool isRunning();
};

