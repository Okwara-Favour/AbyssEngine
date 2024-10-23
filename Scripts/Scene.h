#pragma once
#include "Action.h"
#include "EntityManager.h"
#include <memory>


class GameEngine;

typedef std::map<int, std::string> ActionMap;
class Scene
{
protected:
	GameEngine*		m_game;
	EntityManager	m_entityManager;
	size_t			m_currentFrame = 0;
	ActionMap		m_actionMap;
	bool			m_paused = false;
	bool			m_hasEnded = false;
	std::string		m_currentLevel = "None";

	virtual void onEnd() = 0;
	void setPaused(bool paused);

public:
	Scene();
	Scene(GameEngine* game);
	virtual void update() = 0;
	virtual void sDoAction(const Action& action) = 0;
	virtual void sDoRender() = 0;
	virtual void doAction(const Action& action);
	void simulate(const size_t frames);
	void registerAction(int inputKey, const std::string& actionName);
	void changeLevel(std::string level);

	size_t		width() const;
	size_t		height() const;
	size_t		currentFrame() const;
	std::string getLevel() const;

	bool hasEnded() const;
	const ActionMap& getActionMap() const;
	bool outOfViewBounds(const sf::View& view, const std::shared_ptr<Entity>& entity);
	bool outOfViewBounds(const sf::View& view, const Vec2& targetPos);

	Vec2 gridToMidPixel(Vec2 pos, std::shared_ptr<Entity>& entity, Vec2 cellSize);
	Vec2 midPixelToGrid(Vec2 pos, Vec2 cellSize);
	Vec2 midPixelToGrid(const std::shared_ptr<Entity>& entity, Vec2 cellSize);
	Vec2 pixelToGrid(const std::shared_ptr<Entity>& entity, Vec2 cellSize);
	Vec2 windowToWorld(const Vec2& pos);
	Vec2 adjustByWindow(const Vec2& vec);
};

