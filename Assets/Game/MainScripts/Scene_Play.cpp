#include "Scene_Play.h"
#include "GameEngine.h"
#include "Scene_Menu.h"
#include "Scene_Debug.h"
#include "Physics.h"
#include "Collision.h"

Scene_Play::Scene_Play(GameEngine* gameEngine, const std::string& levelPath)
	:Scene(gameEngine)
	, m_levelPath(levelPath)
{
	init(m_levelPath);
}


void Scene_Play::init(const std::string& levelPath)
{
	registerAction(sf::Keyboard::P,		"PAUSE");
	registerAction(sf::Keyboard::Escape,"QUIT");
	registerAction(sf::Keyboard::T,		"TOGGLE_TEXTURE");		//toggles textures
	registerAction(sf::Keyboard::C,		"TOGGLE_COLLISION");	//toggles collision boxes
	registerAction(sf::Keyboard::G,		"TOGGLE_GRID");			//toggles grid texture
	registerAction(sf::Keyboard::W,		"UP");
	registerAction(sf::Keyboard::S,		"DOWN");
	registerAction(sf::Keyboard::A,		"LEFT");
	registerAction(sf::Keyboard::D,		"RIGHT");
	registerAction(sf::Keyboard::F,		"SHOOT");
	registerAction(sf::Keyboard::Tab,   "DEBUG");

	
	m_gridText.setCharacterSize(12);
	m_gridText.setFont(m_game->assets().getFont("Arial"));

	changeLevel(levelPath);
	loadLevel(levelPath);
	makeGridTexture(m_gridText, m_gridTexture, m_gridSprite);
	
}

void Scene_Play::loadFromFile(const std::string& fileName)
{
	std::ifstream file(fileName);
	std::string line;
	bool checkMaze = false;
	std::vector<std::pair<int, std::shared_ptr<Entity>>> familyStack;
	std::shared_ptr<Entity> parent = nullptr;
	int row = 0;
	while (std::getline(file, line))
	{
		std::istringstream iss(line);
		std::string name;
		iss >> name;
		if (name == "Color")
		{
			std::string color;
			float r, g, b;
			iss >> color >> r >> g >> b;
			if (color == "Game") { m_gameColor = sf::Color(r, g, b); }
			if (color == "Pause") { m_pauseColor = sf::Color(r, g, b); }
		}
		if (name == "Camera")
		{
			size_t sizeX, sizeY;
			iss >> sizeX >> sizeY;
			m_maxCameraGridSize = Vec2(sizeX, sizeY);
			Vec2 winsize(m_game->window().getSize().x, m_game->window().getSize().y);
			Vec2 viewSize(winsize.x / sizeX, winsize.y / sizeY);
			m_camera = Camera(winsize, Vec2(0, 0), viewSize, Vec2(sizeX, sizeY));
		}
		if (name == "Grid")
		{
			size_t sizeX, sizeY;
			iss >> sizeX >> sizeY;
			Vec2 winSize(m_game->window().getSize().x, m_game->window().getSize().y);
			Vec2 size(sizeX, sizeY);
			Vec2 cellsize(winSize.x / size.x, winSize.y / size.y);
			m_gridSize = cellsize;
			std::cout << m_gridSize.toString() << std::endl;
			m_grid = Grid(size, cellsize);
		}
		if (name == "Wall")
		{
			char letterWall;
			std::string animName;
			iss >> letterWall >> animName;
			m_wall[letterWall] = animName;
		}
		if (name == "MazeEnd") { checkMaze = false; }
		if (checkMaze)
		{
			for (int col = 0; col < line.size(); col++)
			{
				if (line[col] == 'o') m_grid.get(col, row).color = cellConfig["path"];
				else
				{
					m_grid.get(col, row).color = cellConfig["wall"];
					//auto wall = m_entityManager.addEntity("wall");
					//spawnEntity("None", Vec2(col, row), m_gridSize, m_gridSize, true, wall);
					createEntity("None", Vec2(col, row), m_gridSize, 0, 0, m_gridSize, true, "tile", 0);
				}
			}
			row++;
		}
		if (name == "MazeBegin") { checkMaze = true; }
		if (name == "player")
		{
			int noofKids = 0;
			Vec2 size;
			Vec2 bbox;
			iss >> playerConfig.GX >> playerConfig.GY >> playerConfig.S >> playerConfig.MS;
			iss >> size.x >> size.y >> bbox.x >> bbox.y;
			iss >> playerConfig.Anim;
			iss >> playerConfig.layer;
			iss >> noofKids;
			size = adjustByWindow(size);
			bbox = adjustByWindow(bbox);
			playerConfig.SX = size.x, playerConfig.SY = size.y;
			playerConfig.BX = bbox.x, playerConfig.BY = bbox.y;
			spawnPlayer();
			if (noofKids > 0)
			{
				parent = m_player;
				familyStack.push_back({ noofKids, parent });
			}
		}
		if (name == "wall")
		{
			std::string animName = "";
			int layer = 0;
			float cellX = 0, cellY = 0, spd = 0, maxSpd = 0, sizeX = 0, sizeY = 0, bbX = 0, bbY = 0;
			iss >> cellX >> cellY >> spd >> maxSpd >> sizeX >> sizeY >> bbX >> bbY >> animName >> layer;
			Vec2 size = adjustByWindow(Vec2(sizeX, sizeY));
			Vec2 bbox = adjustByWindow(Vec2(bbX, bbY));
			createEntity(animName, Vec2(cellX, cellY), size, spd, maxSpd, bbox, true, name, layer);
		}
		if (name == "child" && parent)
		{
			std::string childName;
			iss >> childName;
			int& fIndex = familyStack[familyStack.size() - 1].first;
			fIndex--;
			if (fIndex <= 0) familyStack.pop_back();
			if (std::find(tagChoices.begin(), tagChoices.end(), childName) != tagChoices.end())
			{
				std::string animName = "", texName;
				int lines = 0, layer = 0, noofKids = 0;
				float cellX = 0, cellY = 0, spd = 0, maxSpd = 0, sizeX = 0, sizeY = 0, bbX = 0, bbY = 0;
				float sa = 0, ea = 0, rad = 0, r = 0, g = 0, b = 0, a = 0;
				int which = 0;
				if (std::find(specialChoices.begin(), specialChoices.end(), childName) != specialChoices.end())
				{
					which = (childName == "light") ? 1 : 2;
					iss >> cellX >> cellY >> sa >> ea >> r >> g >> b >> a >> texName >> rad >> lines >> layer >> noofKids;
				}
				else iss >> cellX >> cellY >> spd >> maxSpd >> sizeX >> sizeY >> bbX >> bbY >> animName >> layer >> noofKids;
				Vec2 size = adjustByWindow(Vec2(sizeX, sizeY));
				Vec2 bbox = adjustByWindow(Vec2(bbX, bbY));
				std::shared_ptr<Entity> entity = nullptr;
				if (which == 1) entity = createLightSource(texName, Vec2(cellX, cellY), rad, lines, Vec2(sa, ea), sf::Color(r, g, b, a), layer, parent);
				else entity = createEntity(animName, Vec2(cellX, cellY), size, spd, maxSpd, bbox, true, childName, layer, parent);
				if (noofKids > 0)
				{
					parent = entity;
					familyStack.push_back({ noofKids, parent });
				}
				else
				{
					if (!familyStack.empty()) parent = familyStack[familyStack.size() - 1].second;
					else parent = nullptr;
				}
			}
		}
	}
	file.close();
}

void Scene_Play::loadLevel(const std::string& fileName)
{
	m_entityManager = EntityManager();

	loadFromFile(fileName);
}


void Scene_Play::sState()
{
}

void Scene_Play::sAnimation()
{
	auto& pAnim  = m_player->getComponent<CAnimation>();
	auto& pState = m_player->getComponent<CState>();
	
	if (pState.prevState != pState.state)
	{
		if (pState.state == "idle") 
		{
			auto& anim = m_game->assets().getAnimation(playerConfig.Anim);
			pAnim.animation = Animation(anim, Vec2(playerConfig.SX, playerConfig.SY));
		}
		if (pState.state == "walk")
		{
			auto& anim = m_game->assets().getAnimation("PleeWalk");
			pAnim.animation = Animation(anim, Vec2((playerConfig.SX * 1.5), playerConfig.SY * 1.5));
		}

	}

	for (auto& e : m_entityManager.getEntities())
	{
		if (!e->hasComponent<CAnimation>()) { continue; }
		if (e->getComponent<CAnimation>().repeat)
		{
			e->getComponent<CAnimation>().animation.update();
		}
		else
		{
			if (!e->getComponent<CAnimation>().animation.hasEnded())
			{
				e->getComponent<CAnimation>().animation.update();
			}
		}
	}
}
void Scene_Play::outOfBounds()
{
	
}
void Scene_Play::playerMovement()
{
	Physics physics;
	auto& pTrans = m_player->getComponent<CTransform>();
	auto& pTarg	 = m_player->getComponent<CTarget>();
	auto& pState = m_player->getComponent<CState>();
	auto& pSize = m_player->getComponent<CSize>();
	auto& pBounds = m_player->getComponent<CBoundingbox>();
	auto& pLight = m_player->getComponent<CLightSource>();
	auto& pChildren = m_player->getComponent<CChildren>();
	pTrans.prevPos = pTrans.pos;
	pState.prevState = pState.state;
	pState.state = "idle";
	if (m_game->mouseLeftClick())
	{
		Vec2 mousePos = windowToWorld(m_game->mousePos());
		Vec2 mouseGridPos = midPixelToGrid(mousePos, m_gridSize);
		if (!m_grid.get(mouseGridPos.x, mouseGridPos.y).color.equals(cellConfig["wall"]))
		{
			pTarg.pos = mousePos;
			Vec2 pGridSize(pBounds.size.x / m_gridSize.x, pBounds.size.y / m_gridSize.y);
		}
	}
	Vec2 pathPos = pTarg.pos;

	float angle = std::atan2(pTrans.velocity.y, pTrans.velocity.x);
	
	physics.steeringChase(pTrans.pos, pathPos, pTrans.velocity, pTrans.speed);
	if (!physics.inRange(pTrans.pos, pathPos, adjustByWindow(Vec2(10,10)))) {
		pTrans.angle = (angle * 180 / pTrans.pos.pi) - 90;
		pTrans.pos += pTrans.velocity;
		pState.state = "walk";
	}

	Vec2 prevCellPos = midPixelToGrid(pTrans.prevPos, m_gridSize);
	Vec2 curCellPos = midPixelToGrid(pTrans.pos, m_gridSize);
	m_grid.get(prevCellPos.x, prevCellPos.y).color = cellConfig["path"];
	m_grid.get(curCellPos.x, curCellPos.y).color = cellConfig["entity"];
	pLight.lightSource.setLightSource(pTrans.pos);
	pLight.lightSource.setAngle(pTrans.angle + 180);
	pLight.lightSource.reMake();
	for (auto& c : pChildren.children)
	{
		auto& child = m_entityManager.getEntity(c.first, c.second);
		auto& childLight = child->getComponent<CLightSource>().lightSource;
		childLight.setLightSource(pTrans.pos);
		childLight.setAngle(pTrans.angle + 180);
		childLight.reMake();
	}
}
void Scene_Play::sMovement()
{
	playerMovement();
}
void Scene_Play::sEnemySpawner()
{
}

Vec2 Scene_Play::resizedPosition(const Vec2& pos, const std::shared_ptr<Entity>& entity)
{
	auto& entityBdry = entity->getComponent<CBoundingbox>().size;
	auto& entitySize = entity->getComponent<CSize>().size;
	Vec2  entityPos = { pos.x,pos.y + (entitySize.y - entityBdry.y) / 2.0f };
	return entityPos;
}

void Scene_Play::tileEnemyCollision(const std::shared_ptr<Entity>& e)
{
	
}

std::vector<bool> Scene_Play::collisionLocation(const Vec2& a, const Vec2& b, const Vec2& prevOverlap)
{
	//up: 0, down: 1, left: 2, right: 3
	std::vector<bool> locationBool = { 0,0,0,0 };
	if (a.y < b.y && prevOverlap.x > 0.01f)
	{
		locationBool[0] = 1;
	}
	if (a.y > b.y && prevOverlap.x > 0.01f)
	{
		locationBool[1] = 1;
	}
	if (a.x < b.x && prevOverlap.y > 0.01f)
	{
		locationBool[2] = 1;
	}
	if (a.x > b.x && prevOverlap.y > 0.01f)
	{
		locationBool[3] = 1;
	}

	return locationBool;
}

void Scene_Play::tilePlayerCollision(const std::shared_ptr<Entity>& e)
{
	
}

void Scene_Play::wallCollision(std::shared_ptr<Entity>& object, std::shared_ptr<Entity>& wall, const Vec2& overlap, const Vec2& prevOverlap)
{
	Physics physics;
	auto& objTrans = object->getComponent<CTransform>();
	auto& wallTrans = wall->getComponent<CTransform>();
	std::vector<bool> collisionLoc = collisionLocation(objTrans.pos, wallTrans.pos, prevOverlap);
	if (collisionLoc[0])
	{
		objTrans.pos.y = objTrans.pos.y - overlap.y;
		physics.steeringFlee(objTrans.pos, wallTrans.pos, objTrans.velocity, 0.2, objTrans.speed);
	}
	if (collisionLoc[1])
	{
		objTrans.pos.y = objTrans.pos.y + overlap.y;
		physics.steeringFlee(objTrans.pos, wallTrans.pos, objTrans.velocity, 0.2, objTrans.speed);
	}
	if (collisionLoc[2])
	{
		objTrans.pos.x = objTrans.pos.x - overlap.x;
		physics.steeringFlee(objTrans.pos, wallTrans.pos, objTrans.velocity, 0.2, objTrans.speed);
	}
	if (collisionLoc[3])
	{
		objTrans.pos.x = objTrans.pos.x + overlap.x;
		physics.steeringFlee(objTrans.pos, wallTrans.pos, objTrans.velocity, 0.2, objTrans.speed);
	}
}

void Scene_Play::sCollision()
{
	Collision collision(m_entityManager);
	collision.resolveCollisionWith(m_player, "tile", 0, [this](std::shared_ptr<Entity>& object, std::shared_ptr<Entity>& wall, const Vec2& overlap, const Vec2& prevOverlap) {
		this->wallCollision(object, wall, overlap, prevOverlap);
		});
}

void Scene_Play::sLifespan() 
{
	
}

void Scene_Play::sSystem()
{
	Physics physics;
	for (auto& e : m_entityManager.getEntities())
	{
		if (e->hasComponent<CChildren>())
		{
			auto& eTrans = e->getComponent<CTransform>();
			auto& eSize = e->getComponent<CSize>().size;
			auto& eChildren = e->getComponent<CChildren>().children;
			for (auto& c : eChildren)
			{
				auto& child = m_entityManager.getEntity(c.first, c.second);
				auto& parent = child->getComponent<CParent>();
				auto& cTrans = child->getComponent<CTransform>();
				auto& cSize = child->getComponent<CSize>().size;

				Vec2 scale = eSize / parent.initialSize;
				cTrans.pos = eTrans.pos - Vec2(parent.initialDistance.x * scale.x, parent.initialDistance.y * scale.y);
				cTrans.pos = physics.rotatePointAroundPivot(cTrans.pos, eTrans.pos, eTrans.angle * (eTrans.pos.pi / 180));

				cTrans.angle += eTrans.angle - parent.angle;
				parent.angle = eTrans.angle;

				Vec2 sizeDiff = eSize - parent.size;
				Vec2 prevCSize = cSize;
				cSize += sizeDiff;

				if (prevCSize != cSize)
				{
					if (child->hasComponent<CAnimation>()) {
						auto& anim = child->getComponent<CAnimation>().animation;
						anim = Animation(anim, cSize);
					}
					child->getComponent<CShape>().rectangle.setSize(sf::Vector2f(cSize.x, cSize.y));
				}

				parent.size = eSize;
			}
		}
	}
}


bool Scene_Play::insideViewCellBounds(const std::shared_ptr<Entity>& entity, Vec2& targetCell)
{
	auto& eP = entity->getComponent<CTransform>().pos;
	auto& eS = entity->getComponent<CSize>().size;
	auto& cs = m_camera.cameraCellSize();
	Vec2 UL = midPixelToGrid(Vec2(eP.x - eS.x / 2.0f, eP.y - eS.y / 2.0f), cs).makeWholeNumber();
	Vec2 UR = midPixelToGrid(Vec2(eP.x + eS.x / 2.0f, eP.y - eS.y / 2.0f), cs).makeWholeNumber();
	Vec2 DL = midPixelToGrid(Vec2(eP.x - eS.x / 2.0f, eP.y + eS.y / 2.0f), cs).makeWholeNumber();
	Vec2 DR = midPixelToGrid(Vec2(eP.x + eS.x / 2.0f, eP.y + eS.y / 2.0f), cs).makeWholeNumber();
	return targetCell == UL || targetCell == UR || targetCell == DL || targetCell == DR;
}

bool Scene_Play::insideViewBounds(const std::shared_ptr<Entity>& entity)
{
	return	!outOfViewBounds(m_camera.m_view, entity);
}

void Scene_Play::cellRender()
{
	for (auto& e : m_entityManager.getEntities())
	{
		if (e->tag() == "tile") { continue; }
		if (!insideViewCellBounds(e, m_currentCellView)) { continue; }
		if (e->hasComponent<CAnimation>())
		{
			auto& sprite = e->getComponent<CAnimation>().animation.getSprite();
			auto& spriteTransform = e->getComponent<CTransform>();
			sprite.setPosition(spriteTransform.pos.x, spriteTransform.pos.y);
			sprite.setRotation(spriteTransform.angle);
			m_game->window().draw(sprite);
		}
		else {
			auto& sprite = e->getComponent<CShape>().rectangle;
			auto& spriteTransform = e->getComponent<CTransform>();
			sprite.setPosition(spriteTransform.pos.x, spriteTransform.pos.y);
			sprite.setRotation(spriteTransform.angle);
			m_game->window().draw(sprite);
		}
	}
}

void Scene_Play::viewRender()
{
	for (auto& e : m_entityManager.getEntities())
	{
		if (e->tag() == "tile") { continue; }
		if (!insideViewBounds(e)) { continue; }

		if (e->hasComponent<CLightSource>())
		{
			e->getComponent<CLightSource>().lightSource.drawPolygons(m_game->window(), true);
		}

		if (e->hasComponent<CAnimation>())
		{
			auto& sprite = e->getComponent<CAnimation>().animation.getSprite();
			auto& spriteTransform = e->getComponent<CTransform>();
			sprite.setPosition(spriteTransform.pos.x, spriteTransform.pos.y);
			sprite.setRotation(spriteTransform.angle);
			m_game->window().draw(sprite);
		}
		else {
			auto& sprite = e->getComponent<CShape>().rectangle;
			auto& spriteTransform = e->getComponent<CTransform>();
			sprite.setPosition(spriteTransform.pos.x, spriteTransform.pos.y);
			sprite.setRotation(spriteTransform.angle);
			m_game->window().draw(sprite);
		}
		if (e->hasComponent<CParticleSystem>())
		{
			e->getComponent<CParticleSystem>().particleSystem.update();
			e->getComponent<CParticleSystem>().particleSystem.draw(m_game->window());
		}
	}
}

void Scene_Play::sDoRender()
{
	m_game->window().clear(sf::Color(89, 0, 0));
	if(m_drawGridTexture) m_grid.render(m_game->window(), false);
	else viewRender();
	m_game->window().display();
}
void Scene_Play::sDoAction(const Action& action)
{
	if (action.type() == "START")
	{
		if (action.name() == "QUIT") { onEnd(); }
		if (action.name() == "TOGGLE_GRID") { m_drawGridTexture = !m_drawGridTexture; };
		if (action.name() == "TOGGLE_TEXTURE") { m_toggleTexture = !m_toggleTexture; };
		if (action.name() == "TOGGLE_COLLISION") { m_toggleCollision = !m_toggleCollision; };
		if (action.name() == "PAUSE") { m_paused = !m_paused;  }
		if (action.name() == "DEBUG") { m_game->changeScene("DEBUG", std::make_shared<Scene_Debug>(m_game, getLevel())); }
	}
	else if(action.type() == "END")
	{	}
}
void Scene_Play::sDebug()
{
	
}

void Scene_Play::onEnd()
{
	m_camera.createFullView();
	m_game->window().setView(m_camera.m_view);
	m_game->changeScene("MENU", nullptr, true);
	m_flagEnd = true;
}


void Scene_Play::spawnPlayer()
{
	std::cout << m_camera.cameraCellSize().x << std::endl;
	auto inverseLight = m_entityManager.addEntity("inverseLight");
	m_player = m_entityManager.addEntity("player");
	m_player->addComponent<CSize>(Vec2(playerConfig.SX, playerConfig.SY));
	if (!(playerConfig.Anim == "None")) {
		auto& anim = m_game->assets().getAnimation(playerConfig.Anim);
		auto modAnim = Animation(anim, Vec2(playerConfig.SX, playerConfig.SY));
		m_player->addComponent<CAnimation>(modAnim, true);
	}
	if (playerConfig.BX > 0 || playerConfig.BY > 0) m_player->addComponent<CBoundingbox>(Vec2(playerConfig.BX, playerConfig.BY));
	auto ppos = gridToMidPixel(Vec2(playerConfig.GX, playerConfig.GY), m_player, m_gridSize);
	m_player->addComponent<CTransform>(ppos, Vec2(), 0, playerConfig.MS, playerConfig.S);
	m_player->addComponent<CTarget>(gridToMidPixel(Vec2(playerConfig.GX, playerConfig.GY), m_player, m_gridSize));
	m_player->addComponent<CShape>(playerConfig.SX, playerConfig.SY, sf::Color(100, 255, 0), sf::Color(255, 255, 255), 1.0f);
	m_player->addComponent<CState>("idle");
	m_player->addComponent<CLayer>(playerConfig.layer);

	Vec2 winSize(m_game->window().getSize().x, m_game->window().getSize().y);
	/*LightSource lightsource(ppos, winSize, m_camera.cameraCellSize().x, m_game->assets().getTexture("AreaVisual"),
		sf::Color::Black, 50, 135.0f, 225.0f);
	//lightsource.enableOrDisableTexture(false);
	m_player->addComponent<CLightSource>(lightsource);*/
	
	//inverseLight->addComponent<CTransform>(ppos);

	//LightSource inverselightsource(ppos, winSize, m_camera.cameraCellSize().x, m_game->assets().getTexture("AreaVisual"),
		//sf::Color::Black, 50, 225.0f, 135.0f);
	//inverselightsource.enableOrDisableTexture(false);
	//inverseLight->addComponent<CLightSource>(inverselightsource);
	//inverseLight->addComponent<CParent>(m_player->id(), m_player->tag(), Vec2(), Vec2(playerConfig.SX, playerConfig.SY), 0.0f);
	
	//m_player->addComponent<CChildren>();
	//m_player->getComponent<CChildren>().children.push_back({ inverseLight->id(), inverseLight->tag() });
	
	auto pgpos = midPixelToGrid(m_player, m_gridSize);
	m_grid.get(pgpos.x, pgpos.y).color = cellConfig["entity"];
}

void Scene_Play::spawnPlayerBullet()
{
	
}

void Scene_Play::spawnBullet(std::shared_ptr<Entity> entity)
{
	
}

void Scene_Play::spawnEnemyBullet()
{
	
}

void Scene_Play::update()
{
	m_entityManager.update();
	if(!hasStarted) sortEntities(), hasStarted = true;
	if (!m_paused)
	{
		sMovement();
		sCollision();
		spawnPlayerBullet();
		sSystem();
		sAnimation();
		spawnEnemyBullet();
		moveCamera();
		sLifespan();
	}
	sDoRender();
	m_currentFrame++;
}

void Scene_Play::makeGridTexture(sf::Text& text, sf::RenderTexture& gridTexture, sf::Sprite& gridSprite)
{
	
}

void Scene_Play::moveCamera()
{
	auto& pPos = m_player->getComponent<CTransform>().pos;
	auto& pSize = m_player->getComponent<CSize>().size;
	m_camera.followEntity(pPos, m_camera.cameraCellSize(), Vec2(0,0), Vec2(width(), height()));
	//m_camera.createCellView(Vec2(0, 0));
	//m_camera.createFullView();
	m_game->window().setView(m_camera.m_view);
}

void Scene_Play::spawnEntity(const std::string& animName, const Vec2& Gpos, const Vec2& size, const Vec2& bbox, bool repeat, std::shared_ptr<Entity>& entity)
{
	auto& anim = m_game->assets().getAnimation(animName);
	auto modAnim = Animation(anim, Vec2(size.x, size.y));
	entity->addComponent<CAnimation>(modAnim, repeat);
	entity->addComponent<CSize>(size);
	entity->addComponent<CBoundingbox>(bbox);
	entity->addComponent<CTransform>(gridToMidPixel(Gpos, entity, m_gridSize));
	entity->addComponent<CShape>(size.x, size.y, sf::Color(0, 0, 0, 0), sf::Color(255, 255, 255), 1.0f);
}

std::shared_ptr<Entity>& Scene_Play::createLightSource(const std::string& texName, const Vec2& Gpos,
	const float radius, const int lines, const Vec2& angles,
	const sf::Color& color, const int layer, const std::shared_ptr<Entity>& parent)
{
	auto light = m_entityManager.addEntity("light");
	light->addComponent<CSize>(adjustByWindow(Vec2(20, 20)));
	light->addComponent<CTransform>(gridToMidPixel(Gpos, light, m_gridSize));
	sf::Texture texture = (texName == "None") ? sf::Texture() : m_game->assets().getTexture(texName);
	int n = (lines > 0) ? lines : 1;
	float radi = abs(radius);
	LightSource lightsource(light->getComponent<CTransform>().pos, Vec2(width(), height()), adjustByWindow(Vec2(radi, radi)).x,
		texture, color, n, angles.x, angles.y);
	lightsource.reMake();
	light->addComponent<CLightSource>(lightsource);
	light->addComponent<CLayer>(layer);
	if (parent != nullptr)
	{
		auto& parentTrans = parent->getComponent<CTransform>();
		Vec2 distance = parentTrans.pos - light->getComponent<CTransform>().pos;
		light->addComponent<CParent>(parent->id(), parent->tag(), distance, parent->getComponent<CSize>().size, parentTrans.angle);
		if (!parent->hasComponent<CChildren>()) { parent->addComponent<CChildren>(); }
		parent->getComponent<CChildren>().children.push_back({ light->id(), light->tag() });
	}
	return light;
}

std::shared_ptr<Entity>& Scene_Play::createEntity(const std::string& animName, const Vec2& Gpos, const Vec2& size,
	const float speed, const float maxSpeed,
	const Vec2& bbox, bool repeat, const std::string& type, const int layer,
	const std::shared_ptr<Entity>& parent)
{
	auto entity = m_entityManager.addEntity(type);
	entity->addComponent<CSize>(Vec2(size.x, size.y));
	if (!(animName == "None")) {
		auto& anim = m_game->assets().getAnimation(animName);
		auto modAnim = Animation(anim, Vec2(size.x, size.y));
		entity->addComponent<CAnimation>(modAnim, repeat);
	}
	if (bbox.x > 0 || bbox.y > 0) entity->addComponent<CBoundingbox>(Vec2(bbox.x, bbox.y));
	entity->addComponent<CTransform>(gridToMidPixel(Gpos, entity, m_gridSize), Vec2(speed, speed), 0, maxSpeed, speed);
	entity->addComponent<CShape>(size.x, size.y, sf::Color(100, 255, 0), sf::Color(255, 255, 255), 1.0f);
	entity->addComponent<CLayer>(layer);
	auto gpos = midPixelToGrid(entity, m_gridSize);
	if (type != "wall" && type != "tile") m_grid.get(gpos.x, gpos.y).color = cellConfig["entity"];
	if (parent != nullptr)
	{
		auto& parentTrans = parent->getComponent<CTransform>();
		Vec2 distance = parentTrans.pos - entity->getComponent<CTransform>().pos;
		entity->addComponent<CParent>(parent->id(), parent->tag(), distance, parent->getComponent<CSize>().size, parentTrans.angle);
		if (!parent->hasComponent<CChildren>()) { parent->addComponent<CChildren>(); }
		parent->getComponent<CChildren>().children.push_back({ entity->id(), entity->tag() });
	}
	return entity;
}

void Scene_Play::spawnDec(const std::string& animName, const Vec2& Gpos, const Vec2& size, const Vec2& bbox, bool repeat, std::shared_ptr<Entity>& entity)
{
}

bool Scene_Play::winGame()
{
	return false;
}

void Scene_Play::sortEntities()
{
	auto compare = [](const std::shared_ptr<Entity>& a, const std::shared_ptr<Entity>& b) -> bool {
		bool aHasLayer = a->hasComponent<CLayer>();
		bool bHasLayer = b->hasComponent<CLayer>();

		if (aHasLayer && bHasLayer) {
			auto& aL = a->getComponent<CLayer>();
			auto& bL = b->getComponent<CLayer>();
			return aL.layer < bL.layer;
		}
		if (aHasLayer && !bHasLayer) {
			return true;
		}
		if (!aHasLayer && bHasLayer) {
			return false;
		}
		return false;
		};
	std::sort(m_entityManager.getEntities().begin(), m_entityManager.getEntities().end(), compare);
}