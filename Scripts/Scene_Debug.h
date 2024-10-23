#pragma once
#include <fstream>
#include <algorithm>
#include "Physics.h"
#include "Assets.h"
#include "Components.h"
#include "Scene.h"
#include "Grid.h"
#include "Camera.h"
#include "City.h"
class Scene_Debug : public Scene
{
	struct EntityConfig {
		float px = 0, py = 0, s = 0, ms = 0, sx = 0, sy = 0, bx = 0, by = 0;
		std::string tag = "", anim = "";
		int layer = 0;
		std::shared_ptr<Entity> parent = nullptr;
	};

	struct LightConfig {
		float px = 0, py = 0, sa = 45, ea = 135, rad = 200, r = 0, g = 0, b = 0, a = 255;
		std::string tex = "";
		int layer = 0, l = 30;
		std::shared_ptr<Entity> parent = nullptr;
	};
	std::map<std::string, Color> cellConfig = { {"wall", Color(0,0,0)}, {"entity", Color(255,255,10)}, {"path", Color(255,25,255)} };
	std::vector<std::string> tagCells = { "wall", "entity", "path" };
	std::vector<std::string> tagChoices = { "None", "wall", "player", "enemy", "boss", "item", "brick", "light", "particleSystem"};
	std::vector<std::string> spriteChoices = { "None", "wall", "player", "enemy", "boss", "item", "brick" };
	std::vector<std::string> specialChoices = { "light", "particleSystem" };
	std::vector<std::string> tagAnimations;
	std::vector<std::string> tagTextures;
	int currentTagIndex = 0;
	int currentTagCellIndex = 0;
	int currentTagAnimIndex = 0;
	int currentTagTextIndex = 0;
	int modCurrTagAnimIndex = 0;
	int lightSrcTagTextIndex = 0;
protected:
	std::string							m_levelPath;
	std::shared_ptr<Entity>				m_pickedEntity = nullptr;
	std::shared_ptr<Entity>				m_parentEntity = nullptr;
	EntityConfig						entityConfig;
	LightConfig							lightConfig;
	std::map<char, std::string>			m_wall;
	Vec2					m_currentCellView;
	Vec2					m_gridSize = Vec2(64, 64);
	sf::Color				m_gameColor = sf::Color(sf::Color(135, 206, 235));
	sf::Color				m_pauseColor = sf::Color(150, 206, 235);
	sf::Text				m_gridText;
	sf::RenderTexture		m_gridTexture;
	sf::Sprite				m_gridSprite;
	sf::Clock				m_deltaClock;
	bool					m_drawGridTexture = false;
	bool					m_enableGridMod = false;
	bool					m_toggleTexture = true;
	bool					m_toggleCollision = false;
	bool					m_toggleFullView = true;
	bool					m_togglePick = false;
	bool					m_toggleDrag = false;
	bool					m_allowPicking = false;
	Grid					m_grid;
	Vec2					m_maxCameraGridSize;
	Camera					m_camera;
	std::vector<std::string> m_file;
	std::vector<std::string> m_fileCopy;


	void init(const std::string& levelPath);
	void loadLevel(const std::string& fileName);
	void sSystem();
	void sAnimation();
	void sEnemySpawner();
	void sDoRender();
	void sDoAction(const Action& action);
	void sDebug();
	void onEnd();

	void fullRender();
	void cellRender();
	void loadFromFile(const std::string& fileName);
	void changeCellCamera(int x, int y);
	void moveCamera();
	void moveLightSource();

	bool winGame();

	Vec2 resizedPosition(const Vec2& pos, const std::shared_ptr<Entity>& entity);

	void spawnEntity();
	void spawnEntity(const std::string& animName, const Vec2& Gpos, const Vec2& size, const Vec2& bbox, bool repeat, std::shared_ptr<Entity>& entity, const int layer);
	std::shared_ptr<Entity>& createEntity(const std::string& animName, const Vec2& Gpos, const Vec2& size, const float speed, const float maxSpeed, const Vec2& bbox, bool repeat, const std::string& type, const int layer, const std::shared_ptr<Entity>& parent = nullptr);
	std::shared_ptr<Entity>& createLightSource(const std::string& texName, const Vec2& Gpos, const float radius, const int lines, const Vec2& angles, const sf::Color& color, const int layer, const std::shared_ptr<Entity>& parent = nullptr);
	void spawnDec(const std::string& animName, const Vec2& Gpos, const Vec2& size, const Vec2& bbox, bool repeat, std::shared_ptr<Entity>& entity);
	void spawnPlayer();
	void spawnPlayerBullet();
	void spawnBullet(std::shared_ptr<Entity> entity);
	void spawnEnemyBullet();
	bool insideViewCellBounds(const std::shared_ptr<Entity>& entity, Vec2& targetCell);
	bool insideViewBounds(const std::shared_ptr<Entity>& entity);
	bool outOfBounds(const std::shared_ptr<Entity>& entity, const Vec2& targetPos);

	void makeEntity();
	void makeLightSource();
	void pickEntity();
	void modifyPickedEntity();
	void modifyEntity();
	void modifyLightSource();
	void modifyParticleSystem();
	void settings();
	void entityToFile(std::string& entityLine,int& index, const std::shared_ptr<Entity>& entity);
	void lightSourceToFile(std::string& entityLine, int& index, const std::shared_ptr<Entity>& entity);
	void writeToFile();
	void writeEntityToFile(const std::shared_ptr<Entity>& entity);
	void deleteEntityFromFile(const std::shared_ptr<Entity>& entity);
	void writeMazeToFIle();
	void sortEntities();
	void changeParent(const std::shared_ptr<Entity>& entity, const std::shared_ptr<Entity>& parent);
	void makeIndependent(const std::shared_ptr<Entity>& entity);
	void deleteEntity(const std::shared_ptr<Entity>& entity);
public:
	Scene_Debug(GameEngine* gameEngine, const std::string& levelPath);

	void makeGridTexture(sf::Text& text, sf::RenderTexture& gridTexture, sf::Sprite& gridSprite);
	void update();
};

