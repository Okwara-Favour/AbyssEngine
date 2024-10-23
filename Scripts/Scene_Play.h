#pragma once

#include "Physics.h"
#include "Assets.h"
#include "Components.h"
#include "Scene.h"
#include "Grid.h"
#include "Camera.h"
#include "City.h"
#include "Search.h"

class Scene_Play : public Scene
{
	struct PlayerConfig {
		float GX = 0.0f, GY = 0.0f, S = 0.0f, MS = 0.0f,
			SX = 0.0f, SY = 0.0f, BX = 0.0f, BY = 0.0f;
		std::string Anim = "None";
		int layer = 0;
		PlayerConfig() {};
		PlayerConfig(float gx, float gy, float s, float ms, float sx, float sy,
					 float bx, float by, std::string anim)
					:GX(gx), GY(gy), S(s), MS(ms), SX(sx), SY(sy),
					 BX(bx), BY(by), Anim(anim) {};
	};
	struct PlayerBullet {
		float SW = 0, SH = 0, SX = 0, SY = 0, LS = 0;
		PlayerBullet() {};
		PlayerBullet(float sw, float sh, float sx, float sy, float ls)
			:SW(sw), SH(sh), SX(sx), SY(sy), LS(ls) {};
	};
	bool hasStarted = false;
	std::map<std::string, Color> cellConfig = { {"wall", Color(0,0,0)}, {"entity", Color(255,255,10)}, {"path", Color(255,25,255)} };
	std::vector<std::string> tagChoices = { "None", "wall", "player", "enemy", "boss", "item", "brick", "light", "particleSystem" };
	std::vector<std::string> spriteChoices = { "None", "wall", "player", "enemy", "boss", "item", "brick" };
	std::vector<std::string> specialChoices = { "light", "particleSystem" };
protected:
	std::string							m_levelPath;
	std::shared_ptr<Entity>				m_player;
	PlayerConfig						playerConfig;
	PlayerBullet						playerBulletConfig;
	std::map<char, std::string>			m_wall;
	Vec2				m_gridSize = Vec2(64, 64);
	sf::Color			m_gameColor = sf::Color(sf::Color(135, 206, 235));
	sf::Color			m_pauseColor = sf::Color(150, 206, 235);
	sf::Text			m_gridText;
	sf::RenderTexture	m_gridTexture;
	sf::Sprite			m_gridSprite;
	Vec2				m_currentCellView;
	bool				m_drawGridTexture = false;
	bool				m_toggleTexture = true;
	bool				m_toggleCollision = false;
	bool				m_paused = false;
	bool				m_flagEnd = false;
	Grid				m_grid;
	Vec2				m_maxCameraGridSize;
	Camera				m_camera;
	Search				m_search = Search();

	void init(const std::string& levelPath);
	void loadLevel(const std::string& fileName);
	void sLifespan();
	void sState();
	void sAnimation();
	void sMovement();
	void sEnemySpawner();
	void sSystem();
	void sCollision();
	void sDoRender();
	void sDoAction(const Action& action);
	void sDebug();
	void onEnd();

	void cellRender();
	void viewRender();

	void loadFromFile(const std::string& fileName);
	void playerMovement();
	void outOfBounds();
	void wallCollision(std::shared_ptr<Entity>& object, std::shared_ptr<Entity>& wall, const Vec2& overlap, const Vec2& prevOverlap);
	void tileEnemyCollision(const std::shared_ptr<Entity>& e);
	void tilePlayerCollision(const std::shared_ptr<Entity>& e);
	void moveCamera();

	bool insideViewCellBounds(const std::shared_ptr<Entity>& entity, Vec2& targetCell);
	bool insideViewBounds(const std::shared_ptr<Entity>& entity);
	bool winGame();

	Vec2 resizedPosition(const Vec2& pos, const std::shared_ptr<Entity>& entity);
	std::vector<bool> collisionLocation(const Vec2& a, const Vec2& b, const Vec2& prevOverlap);

	void spawnEntity(const std::string& animName, const Vec2& Gpos, const Vec2& size, const Vec2& bbox, bool repeat, std::shared_ptr<Entity>& entity);
	std::shared_ptr<Entity>& createEntity(const std::string& animName, const Vec2& Gpos, const Vec2& size, const float speed, const float maxSpeed, const Vec2& bbox, bool repeat, const std::string& type, const int layer, const std::shared_ptr<Entity>& parent = nullptr);
	std::shared_ptr<Entity>& createLightSource(const std::string& texName, const Vec2& Gpos, const float radius, const int lines, const Vec2& angles, const sf::Color& color, const int layer, const std::shared_ptr<Entity>& parent = nullptr);
	void spawnDec(const std::string& animName, const Vec2& Gpos, const Vec2& size, const Vec2& bbox, bool repeat, std::shared_ptr<Entity>& entity);
	void spawnPlayer();
	void spawnPlayerBullet();
	void spawnBullet(std::shared_ptr<Entity> entity);
	void spawnEnemyBullet();
	void sortEntities();
public:
	Scene_Play(GameEngine* gameEngine, const std::string& levelPath);

	void makeGridTexture(sf::Text& text, sf::RenderTexture& gridTexture, sf::Sprite& gridSprite);

	void update();
};

