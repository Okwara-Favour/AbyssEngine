#include "Scene.h"
#include "Physics.h"
#include "GameEngine.h"

Scene::Scene()
{
	m_game = nullptr;
}
Scene::Scene(GameEngine* game)
	:m_game(game)
{}
void Scene::update()
{}
void Scene::sDoAction(const Action& action)
{}
void Scene::sDoRender()
{}
void Scene::simulate(const size_t frames)
{}
void Scene::doAction(const Action& action)
{
	sDoAction(action);
}
void Scene::registerAction(int inputKey, const std::string& actionName)
{
	m_actionMap[inputKey] = actionName;
}

void Scene::changeLevel(std::string level)
{
	m_currentLevel = level;
}

size_t Scene::width() const
{
	return m_game->window().getSize().x;
}
size_t Scene::height() const
{
	return m_game->window().getSize().y;
}

void Scene::setPaused(bool paused)
{
	m_paused = paused;
}

size_t Scene::currentFrame() const
{
	return m_currentFrame;
}

std::string Scene::getLevel() const
{
	return m_currentLevel;
}

bool Scene::hasEnded() const
{
	return m_hasEnded;
}
const ActionMap& Scene::getActionMap() const
{
	return m_actionMap;
}

Vec2 Scene::gridToMidPixel(Vec2 pos, std::shared_ptr<Entity>& entity, Vec2 cellSize)
{
	auto& size = entity->getComponent<CSize>().size;
	float posX = (pos.x * cellSize.x) + (size.x / 2.0f);
	float posY = (pos.y * cellSize.y) + (size.y / 2.0f);
	return Vec2(posX, posY);
}

Vec2 Scene::midPixelToGrid(Vec2 pos, Vec2 cellSize)
{
	return Vec2(pos.x / cellSize.x, pos.y / cellSize.y);
}

Vec2 Scene::midPixelToGrid(const std::shared_ptr<Entity>& entity, Vec2 cellSize)
{
	auto sPos = entity->getComponent<CTransform>().pos;
	return Vec2(sPos.x / cellSize.x, sPos.y / cellSize.y);
}

Vec2 Scene::pixelToGrid(const std::shared_ptr<Entity>& entity, Vec2 cellSize)
{
	auto sPos = entity->getComponent<CTransform>().pos;
	auto& sSize = entity->getComponent<CSize>().size;
	float gridX = (sPos.x - (sSize.x / 2.0f)) / cellSize.x;
	float gridY = (sPos.y - (sSize.y / 2.0f)) / cellSize.y;
	return Vec2(gridX, gridY);
}

Vec2 Scene::windowToWorld(const Vec2& pos)
{
	sf::Vector2i pixelPos(static_cast<int>(pos.x), static_cast<int>(pos.y));
	const sf::View& view = m_game->window().getView();
	sf::Vector2f worldPos = m_game->window().mapPixelToCoords(pixelPos, view);
	return Vec2(worldPos.x, worldPos.y);
}

bool Scene::outOfViewBounds(const sf::View& view, const std::shared_ptr<Entity>& entity)
{
	auto sPos = entity->getComponent<CTransform>().pos;
	auto& sSize = entity->getComponent<CSize>().size;
	Physics physics;
	auto viewCenter = view.getCenter();
	auto viewSize = view.getSize();
	Vec2 overlap = physics.getOverLap(sPos, sSize, Vec2(viewCenter.x, viewCenter.y), Vec2(viewSize.x, viewSize.y));
	return !(overlap.x > 0.0 && overlap.y > 0.0);
}

bool Scene::outOfViewBounds(const sf::View& view, const Vec2& targetPos)
{
	float viewLeft = view.getCenter().x - (view.getSize().x / 2);
	float viewTop = view.getCenter().y - (view.getSize().y / 2);
	float viewRight = viewLeft + view.getSize().x;
	float viewBottom = viewTop + view.getSize().y;
	return targetPos.x < viewLeft || targetPos.y < viewTop || targetPos.x > viewRight || targetPos.y > viewBottom;
}

Vec2 Scene::adjustByWindow(const Vec2& vec)
{
	Vec2 scale(width() / m_game->defaultWindowSize().x, height() / m_game->defaultWindowSize().y);
	return Vec2(vec.x * scale.x, vec.y * scale.y);
}