#include "Scene_Debug.h"
#include "Scene_Play.h"
#include "GameEngine.h"
#include "Scene_Menu.h"
#include "Physics.h"

Scene_Debug::Scene_Debug(GameEngine* gameEngine, const std::string& levelPath)
	:Scene(gameEngine)
	, m_levelPath(levelPath)
{
	init(m_levelPath);
}


void Scene_Debug::init(const std::string& levelPath)
{
	registerAction(sf::Keyboard::P, "PAUSE");
	registerAction(sf::Keyboard::Escape, "QUIT");
	registerAction(sf::Keyboard::T, "TOGGLE_TEXTURE");		//toggles textures
	registerAction(sf::Keyboard::C, "TOGGLE_COLLISION");	//toggles collision boxes
	registerAction(sf::Keyboard::G, "TOGGLE_GRID");			//toggles grid texture
	registerAction(sf::Keyboard::V, "TOGGLE_FULL_VIEW");	//toggles view type
	registerAction(sf::Keyboard::W, "UP");
	registerAction(sf::Keyboard::S, "DOWN");
	registerAction(sf::Keyboard::A, "LEFT");
	registerAction(sf::Keyboard::D, "RIGHT");
	registerAction(sf::Keyboard::Tab, "PLAY");


	m_gridText.setCharacterSize(12);
	m_gridText.setFont(m_game->assets().getFont("Arial"));

	tagAnimations.push_back("None");
	for (auto& e : m_game->assets().getAnimations())
	{
		tagAnimations.push_back(e.first);
	}

	tagTextures.push_back("None");
	for (auto& t : m_game->assets().getTextures())
	{
		tagTextures.push_back(t.first);
	}
	loadLevel(levelPath);
	makeGridTexture(m_gridText, m_gridTexture, m_gridSprite);
	std::cout << "Debug Mode " << std::endl;
}

void Scene_Debug::loadFromFile(const std::string& fileName)
{
	std::ifstream file(fileName);
	std::string line;
	bool checkMaze = false;
	bool ignoreMaze = false;
	bool ignoreEntityConfig = false;
	std::vector<std::pair<int, std::shared_ptr<Entity>>> familyStack;
	std::shared_ptr<Entity> parent = nullptr;
	int row = 0;
	while (std::getline(file, line))
	{
		std::istringstream iss(line);
		std::string name;
		iss >> name;
		if (!ignoreEntityConfig)
		{
			if (name == "MazeEnd") ignoreMaze = false;
			if (!ignoreMaze) m_file.push_back(line);
			if (name == "MazeBegin") ignoreMaze = true;
		}
		else if(name == "Entity")
		{
			m_file.push_back(line);
		}
		if (name == "Config")
		{
			ignoreEntityConfig = true;
		}

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
				if (line[col] == 'o') {
					m_grid.get(col, row).color = cellConfig["path"];
				}
				else
				{
					auto wall = m_entityManager.addEntity("test");
					m_grid.get(col, row).color = cellConfig["wall"];
					//spawnEntity(m_wall[line[col]], Vec2(col, row), m_gridSize, m_gridSize, true, wall);
					spawnEntity("None", Vec2(col, row), m_gridSize, m_gridSize, true, wall, 0);
				}
			}
			row++;
		}
		if (name == "MazeBegin") { checkMaze = true; }
		if (std::find(tagChoices.begin(), tagChoices.end(), name) != tagChoices.end())
		{
			std::string animName = "", texName = "";
			int lines = 0, layer = 0, noofKids = 0;
			float cellX = 0, cellY = 0, spd = 0, maxSpd = 0, sizeX = 0, sizeY = 0, bbX = 0, bbY = 0;
			float sa = 0, ea = 0, rad = 0, r = 0, g = 0, b = 0, a = 0;
			int which = 0;
			if (std::find(specialChoices.begin(), specialChoices.end(), name) != specialChoices.end())
			{
				which = (name == "light") ? 1 : 2;
				iss >> cellX >> cellY >> sa >> ea >> r >> g >> b >> a >> texName >> rad >> lines >> layer >> noofKids;
			}
			else iss >> cellX >> cellY >> spd >> maxSpd >> sizeX >> sizeY >> bbX >> bbY >> animName >> layer >> noofKids;
			Vec2 size = adjustByWindow(Vec2(sizeX, sizeY));
			Vec2 bbox = adjustByWindow(Vec2(bbX, bbY));

			std::shared_ptr<Entity> temp = nullptr;
			if (which == 1) temp = createLightSource(texName, Vec2(cellX, cellY), rad, lines, Vec2(sa, ea), sf::Color(r, g, b, a), layer);
			else temp = createEntity(animName, Vec2(cellX, cellY), size, spd, maxSpd, bbox, true, name, layer);
			if (noofKids > 0) 
			{
				parent = temp;
				familyStack.push_back({ noofKids, parent });
			}
		}
		if (name == "child")
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
	m_fileCopy = m_file;
}

void Scene_Debug::loadLevel(const std::string& fileName)
{
	m_entityManager = EntityManager();

	loadFromFile(fileName);

	spawnPlayer();
}



void Scene_Debug::sSystem()
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

void Scene_Debug::sAnimation()
{
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

void Scene_Debug::sEnemySpawner()
{
}

Vec2 Scene_Debug::resizedPosition(const Vec2& pos, const std::shared_ptr<Entity>& entity)
{
	auto& entityBdry = entity->getComponent<CBoundingbox>().size;
	auto& entitySize = entity->getComponent<CSize>().size;
	Vec2  entityPos = { pos.x,pos.y + (entitySize.y - entityBdry.y) / 2.0f };
	return entityPos;
}

void Scene_Debug::fullRender()
{
	for (auto& e : m_entityManager.getEntities())
	{
		auto& spriteTransform = e->getComponent<CTransform>();
		auto& spriteSize = e->getComponent<CSize>();
		if (e->hasComponent<CAnimation>())
		{
			auto& sprite = e->getComponent<CAnimation>().animation.getSprite();
			sprite.setPosition(spriteTransform.pos.x, spriteTransform.pos.y);
			sprite.setRotation(spriteTransform.angle);
			m_game->window().draw(sprite);
		}
		else {
			auto& sprite = e->getComponent<CShape>().rectangle;
			sprite.setPosition(spriteTransform.pos.x, spriteTransform.pos.y);
			sprite.setRotation(spriteTransform.angle);
			m_game->window().draw(sprite);
		}
		if (e->hasComponent<CLightSource>())
		{
			auto& LightSource = e->getComponent<CLightSource>();
			LightSource.lightSource.drawPolygons(m_game->window());
			sf::RectangleShape rectangle(sf::Vector2f(spriteSize.size.x, spriteSize.size.y));
			rectangle.setFillColor(sf::Color(0, 0, 0, 0));
			rectangle.setOutlineThickness(1);
			rectangle.setOutlineColor(sf::Color(20, 100, 255));
			rectangle.setPosition(sf::Vector2f(LightSource.lightSource.getLightSource().x, LightSource.lightSource.getLightSource().y));
			rectangle.setOrigin(spriteSize.size.x / 2.0f, spriteSize.size.y / 2.0f);
			m_game->window().draw(rectangle);
		}
		if (m_pickedEntity != nullptr && m_pickedEntity->tag() == e->tag() && m_pickedEntity->id() == e->id())
		{
			sf::RectangleShape rectangle(sf::Vector2f(spriteSize.size.x, spriteSize.size.y));
			rectangle.setFillColor(sf::Color(0, 0, 0, 0));
			rectangle.setOutlineThickness(1);
			rectangle.setOutlineColor(sf::Color(255, 100, 20));
			rectangle.setPosition(sf::Vector2f(spriteTransform.pos.x, spriteTransform.pos.y));
			rectangle.setOrigin(spriteSize.size.x / 2.0f, spriteSize.size.y / 2.0f);
			m_game->window().draw(rectangle);
		}
	}
}

void Scene_Debug::cellRender()
{
	for (auto& e : m_entityManager.getEntities())
	{
		if (!insideViewBounds(e)) { continue; }
		auto& spriteTransform = e->getComponent<CTransform>();
		auto& spriteSize = e->getComponent<CSize>();
		if (e->hasComponent<CAnimation>())
		{
			auto& sprite = e->getComponent<CAnimation>().animation.getSprite();
			sprite.setPosition(spriteTransform.pos.x, spriteTransform.pos.y);
			sprite.setRotation(spriteTransform.angle);
			m_game->window().draw(sprite);
		}
		else {
			auto& sprite = e->getComponent<CShape>().rectangle;
			sprite.setPosition(spriteTransform.pos.x, spriteTransform.pos.y);
			sprite.setRotation(spriteTransform.angle);
			m_game->window().draw(sprite);
		}
		if (e->hasComponent<CLightSource>())
		{
			e->getComponent<CLightSource>().lightSource.drawPolygons(m_game->window(), true);
		}
		if (m_pickedEntity != nullptr && m_pickedEntity->tag() == e->tag() && m_pickedEntity->id() == e->id())
		{
			sf::RectangleShape rectangle(sf::Vector2f(spriteSize.size.x, spriteSize.size.y));
			rectangle.setFillColor(sf::Color(0, 0, 0, 0));
			rectangle.setOutlineThickness(1);
			rectangle.setOutlineColor(sf::Color(255, 100, 20));
			rectangle.setPosition(sf::Vector2f(spriteTransform.pos.x, spriteTransform.pos.y));
			rectangle.setOrigin(spriteSize.size.x / 2.0f, spriteSize.size.y / 2.0f);
			m_game->window().draw(rectangle);
		}
	}
}

void Scene_Debug::sDoRender()
{
	ImGui::SFML::Update(m_game->window(), m_deltaClock.restart());
	m_game->window().clear(sf::Color::Blue);
	if (m_drawGridTexture) m_grid.render(m_game->window(), m_toggleFullView);
	else {
		if (m_toggleFullView) fullRender();
		else cellRender();
	}
	
	makeEntity();
	makeLightSource();
	modifyPickedEntity();
	settings();
	ImGui::SFML::Render(m_game->window());
	m_game->window().display();
}
void Scene_Debug::sDoAction(const Action& action)
{
	if (action.type() == "START")
	{
		if (action.name() == "QUIT") { onEnd(); }
		if (action.name() == "TOGGLE_GRID") { m_drawGridTexture = !m_drawGridTexture; };
		if (action.name() == "TOGGLE_TEXTURE") { m_toggleTexture = !m_toggleTexture; std::cout << modCurrTagAnimIndex << std::endl; };
		if (action.name() == "TOGGLE_COLLISION") { m_toggleCollision = !m_toggleCollision;};
		if (action.name() == "TOGGLE_FULL_VIEW") { m_toggleFullView = !m_toggleFullView; };
		if (action.name() == "UP") changeCellCamera(0, -1);
		if (action.name() == "DOWN") changeCellCamera(0, 1);
		if (action.name() == "LEFT") changeCellCamera(-1, 0);
		if (action.name() == "RIGHT") changeCellCamera(1, 0);
		if (action.name() == "PLAY") { m_game->changeScene("PLAY", nullptr, true); };
	}
	else if (action.type() == "END")
	{	}
}
void Scene_Debug::sDebug()
{

}

void Scene_Debug::onEnd()
{
	m_camera.createFullView();
	//sf::View view = m_game->window().getView();
	//view.setCenter(width() / 2, height() / 2);
	m_game->window().setView(m_camera.m_view);
	m_game->changeScene("MENU", nullptr, true);
}


void Scene_Debug::spawnPlayer()
{

}

void Scene_Debug::spawnPlayerBullet()
{

}

void Scene_Debug::spawnBullet(std::shared_ptr<Entity> entity)
{

}

void Scene_Debug::spawnEnemyBullet()
{

}


void Scene_Debug::update()
{
	m_entityManager.update();
	spawnPlayerBullet();
	sSystem();
	sAnimation();
	spawnEnemyBullet();
	moveCamera();
	moveLightSource();
	pickEntity();
	sDoRender();
	m_currentFrame++;
}

void Scene_Debug::makeGridTexture(sf::Text& text, sf::RenderTexture& gridTexture, sf::Sprite& gridSprite)
{

}

void Scene_Debug::changeCellCamera(int x, int y)
{
	Vec2 temp(m_currentCellView);
	temp += Vec2(x, y);
	if (!(temp.x < 0 || temp.y < 0 || temp.x >= m_maxCameraGridSize.x || temp.y >= m_maxCameraGridSize.y))
	{
		m_currentCellView = temp;
	}
}

void Scene_Debug::moveCamera()
{
	if(m_toggleFullView) m_camera.createFullView();
	else m_camera.createCellView(m_currentCellView);
	m_game->window().setView(m_camera.m_view);
}




void Scene_Debug::spawnEntity(const std::string& animName, const Vec2& Gpos, const Vec2& size, const Vec2& bbox, bool repeat, std::shared_ptr<Entity>& entity, const int layer)
{
	if (!(animName == "None")) {
		auto& anim = m_game->assets().getAnimation(animName);
		auto modAnim = Animation(anim, Vec2(size.x, size.y));
		entity->addComponent<CAnimation>(modAnim, repeat);
	}
	entity->addComponent<CSize>(size);
	entity->addComponent<CBoundingbox>(bbox);
	entity->addComponent<CTransform>(gridToMidPixel(Gpos, entity, m_gridSize));
	entity->addComponent<CShape>(size.x, size.y, sf::Color(0, 0, 0, 0), sf::Color(255, 255, 255), 1.0f);
	entity->addComponent<CLayer>(layer);
}

std::shared_ptr<Entity>& Scene_Debug::createEntity(	const std::string& animName, const Vec2& Gpos, const Vec2& size,
								const float speed, const float maxSpeed,
								const Vec2& bbox, bool repeat, const std::string& type, const int layer,
								const std::shared_ptr<Entity>& parent)
{
	auto entity = m_entityManager.addEntity(type);
	entity->addComponent<CSize>(Vec2(size.x, size.y));
	if (!(animName == "None")) {
		auto it = std::find(tagAnimations.begin(), tagAnimations.end(), animName);
		int index = (it != tagAnimations.end()) ? std::distance(tagAnimations.begin(), it) : 0;
		auto& anim = m_game->assets().getAnimation(animName);
		auto modAnim = Animation(anim, Vec2(size.x, size.y));
		entity->addComponent<CAnimation>(modAnim, repeat, index);
	}
	if(bbox.x > 0 || bbox.y > 0) entity->addComponent<CBoundingbox>(Vec2(bbox.x, bbox.y));
	entity->addComponent<CTransform>(gridToMidPixel(Gpos, entity, m_gridSize), Vec2(speed, speed), 0, maxSpeed, speed);
	entity->addComponent<CShape>(size.x, size.y, sf::Color(100, 255, 0), sf::Color(255, 255, 255), 1.0f);
	entity->addComponent<CLayer>(layer);
	auto gpos = midPixelToGrid(entity, m_gridSize);
	if (type != "wall") m_grid.get(gpos.x, gpos.y).color = cellConfig["entity"];
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

std::shared_ptr<Entity>& Scene_Debug::createLightSource(const std::string& texName, const Vec2& Gpos,
	const float radius, const int lines, const Vec2& angles,
	const sf::Color& color,	const int layer, const std::shared_ptr<Entity>& parent)
{
	auto light = m_entityManager.addEntity("light");
	light->addComponent<CSize>(adjustByWindow(Vec2(20, 20)));
	light->addComponent<CTransform>(gridToMidPixel(Gpos, light, m_gridSize));
	sf::Texture texture = (texName == "None") ? sf::Texture() : m_game->assets().getTexture(texName);
	int n = (lines > 0) ? lines : 1;
	LightSource lightsource(light->getComponent<CTransform>().pos, Vec2(width(), height()), abs(radius),
		texture, color, n, angles.x, angles.y);
	lightsource.reMake();
	light->addComponent<CLightSource>(lightsource);
	auto it = std::find(tagTextures.begin(), tagTextures.end(), texName);
	int index = (it != tagTextures.end()) ? std::distance(tagTextures.begin(), it) : 0;
	light->getComponent<CLightSource>().index = index;
	light->addComponent<CLayer>(layer);
	if (parent != nullptr)
	{
		auto& parentTrans = parent->getComponent<CTransform>();
		Vec2 distance = parentTrans.pos - light->getComponent<CTransform>().pos;
		light->addComponent<CParent>(parent->id(), parent->tag(), distance, parent->getComponent<CSize>().size, parentTrans.angle);
		if (!parent->hasComponent<CChildren>()) { parent->addComponent<CChildren>(); }
		parent->getComponent<CChildren>().children.push_back({ light->id(), light->tag() });
	}

	std::cout << "Light created" << std::endl;
	return light;
}


void Scene_Debug::spawnDec(const std::string& animName, const Vec2& Gpos, const Vec2& size, const Vec2& bbox, bool repeat, std::shared_ptr<Entity>& entity)
{
}

bool Scene_Debug::winGame()
{
	return false;
}

bool Scene_Debug::insideViewCellBounds(const std::shared_ptr<Entity>& entity, Vec2& targetCell)
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

bool Scene_Debug::insideViewBounds(const std::shared_ptr<Entity>& entity)
{
	return	!outOfViewBounds(m_camera.m_view, entity);
}


bool Scene_Debug::outOfBounds(const std::shared_ptr<Entity>& entity, const Vec2& targetPos)
{
	Vec2 pos = entity->getComponent<CTransform>().pos;
	Vec2 size = entity->getComponent<CSize>().size;
	return	targetPos.x < pos.x - size.x/2 || targetPos.y < pos.y - size.y / 2 ||
			targetPos.x > pos.x + size.x / 2 || targetPos.y > pos.y + size.y / 2;
}



void Scene_Debug::deleteEntityFromFile(const std::shared_ptr<Entity>& entity)
{

}

void Scene_Debug::moveLightSource()
{
	for (auto& e : m_entityManager.getEntities())
	{
		if (e->hasComponent<CLightSource>())
		{
			e->getComponent<CLightSource>().lightSource.setLightSource(
				e->getComponent<CTransform>().pos
			);
			e->getComponent<CLightSource>().lightSource.reMake();
		}
	}
}

void Scene_Debug::makeLightSource()
{
	ImGui::Begin("Make light");
	//Layer
	ImGui::Text("Layer");
	ImGui::InputInt("layer", &lightConfig.layer);
	//Lines
	ImGui::Text("Lines");
	ImGui::InputInt("No", &lightConfig.l);
	//Color
	float colorValues[] = { lightConfig.r / 255.0f, lightConfig.g / 255.0f, lightConfig.b / 255.0f, lightConfig.a / 255.0f };
	ImGui::Text("Color Picker");
	if (ImGui::ColorEdit4("Pick Color", colorValues)) {
		// Convert ImGui color to SFML color
		lightConfig.r = static_cast<sf::Uint8>(colorValues[0] * 255);
		lightConfig.g = static_cast<sf::Uint8>(colorValues[1] * 255);
		lightConfig.b = static_cast<sf::Uint8>(colorValues[2] * 255);
		lightConfig.a = static_cast<sf::Uint8>(colorValues[3] * 255);
	}

	ImGui::PushItemWidth(50);
	// Position
	ImGui::Text("Position");
	ImGui::InputFloat("PX", &lightConfig.px);
	ImGui::SameLine();
	ImGui::InputFloat("PY", &lightConfig.py);
	//Angles
	ImGui::Text("Angles");
	ImGui::InputFloat("Start", &lightConfig.sa);
	ImGui::SameLine();
	ImGui::InputFloat("End", &lightConfig.ea);
	//Extent
	ImGui::Text("Extent");
	ImGui::InputFloat("Radius", &lightConfig.rad);

	//Texture
	ImGui::Text("Texture");
	if (ImGui::Combo("Texture", &currentTagTextIndex, [](void* data, int idx, const char** out_text) {
		auto& vec = *static_cast<std::vector<std::string>*>(data);
		if (idx < 0 || idx >= vec.size()) return false;
		*out_text = vec[idx].c_str();
		return true;
		}, static_cast<void*>(&tagTextures), tagTextures.size())) {
		lightConfig.tex = tagTextures[currentTagTextIndex];
	}

	ImGui::Text("Parent Candidate");
	if (m_parentEntity == nullptr)
	{
		ImGui::Text("None");
	}
	else
	{
		ImGui::Text("Tag: %s", m_parentEntity->tag().c_str());
		ImGui::SameLine();
		ImGui::Text("Id: %zu", m_parentEntity->id());
		ImGui::SameLine();
		if (ImGui::Button("Set Parent")) {
			lightConfig.parent = m_parentEntity;
		}
	}

	ImGui::PopItemWidth();

	if (ImGui::Button("Spawn Light")) {
		auto& Light = createLightSource(lightConfig.tex, Vec2(lightConfig.px, lightConfig.py), lightConfig.rad, lightConfig.l,
			Vec2(lightConfig.sa, lightConfig.ea), sf::Color(lightConfig.r, lightConfig.g, lightConfig.b, lightConfig.a),
			lightConfig.layer, lightConfig.parent);
		//Light->getComponent<CLightSource>().index = currentTagTextIndex;
		lightConfig.parent = nullptr;
	}

	ImGui::End();
}

void Scene_Debug::makeEntity()
{
	ImGui::Begin("Make Entity");
	//Layer
	ImGui::Text("Layer");
	ImGui::InputInt("layer", &entityConfig.layer);

	ImGui::PushItemWidth(50);
	//Tag
	ImGui::Text("Tag");
	if (ImGui::Combo("Tag", &currentTagIndex, [](void* data, int idx, const char** out_text) {
		auto& vec = *static_cast<std::vector<std::string>*>(data);
		if (idx < 0 || idx >= vec.size()) return false;
		*out_text = vec[idx].c_str();
		return true;
		}, static_cast<void*>(&tagChoices), tagChoices.size())) {
		entityConfig.tag = tagChoices[currentTagIndex]; // Update the tag in entityConfig
	}

	// Position
	ImGui::Text("Position");
	ImGui::InputFloat("PX", &entityConfig.px);
	ImGui::SameLine();
	ImGui::InputFloat("PY", &entityConfig.py);

	// Size
	ImGui::Text("Size");
	ImGui::InputFloat("SX", &entityConfig.sx);
	ImGui::SameLine();
	ImGui::InputFloat("SY", &entityConfig.sy);

	// Boundary (same as Size)
	ImGui::Text("Collision box");
	ImGui::InputFloat("BX", &entityConfig.bx);
	ImGui::SameLine();
	ImGui::InputFloat("BY", &entityConfig.by);

	// Speed and Max Speed
	ImGui::Text("Speed and Max Speed");
	ImGui::InputFloat("Speed", &entityConfig.s);
	ImGui::SameLine();
	ImGui::InputFloat("Max Speed", &entityConfig.ms);

	//Animation
	ImGui::Text("Animation");
	if (ImGui::Combo("Animation", &currentTagAnimIndex, [](void* data, int idx, const char** out_text) {
		auto& vec = *static_cast<std::vector<std::string>*>(data);
		if (idx < 0 || idx >= vec.size()) return false;
		*out_text = vec[idx].c_str();
		return true;
		}, static_cast<void*>(&tagAnimations), tagAnimations.size())) {
		entityConfig.anim = tagAnimations[currentTagAnimIndex]; // Update the anim in entityConfig
	}

	ImGui::Text("Parent Candidate");
	if (m_parentEntity == nullptr)
	{
		ImGui::Text("None");
	}
	else 
	{
		ImGui::Text("Tag: %s", m_parentEntity->tag().c_str());
		ImGui::SameLine();
		ImGui::Text("Id: %zu", m_parentEntity->id());
		ImGui::SameLine();
		if (ImGui::Button("Set Parent")) {
			entityConfig.parent = m_parentEntity;
		}
	}
	ImGui::PopItemWidth();
	if (ImGui::Button("Spawn Entity")) {
		spawnEntity();
	}
	ImGui::End();
}

void Scene_Debug::spawnEntity()
{
	if (entityConfig.tag == "None" || entityConfig.tag == "")	return;
	std::cout << "entity created " << entityConfig.tag << std::endl;
	Vec2 pos(entityConfig.px, entityConfig.py);
	Vec2 size(entityConfig.sx, entityConfig.sy);
	Vec2 bbox(entityConfig.bx, entityConfig.by);
	if (entityConfig.anim == "None" || entityConfig.anim == "") {
		createEntity("None", pos, size, entityConfig.s, entityConfig.ms, bbox, false, entityConfig.tag, entityConfig.layer, entityConfig.parent);
	}
	else createEntity(entityConfig.anim, pos, size, entityConfig.s, entityConfig.ms, bbox, true, entityConfig.tag, entityConfig.layer, entityConfig.parent);
	entityConfig.parent = nullptr;
}

void Scene_Debug::pickEntity()
{
	Vec2 mousePos = windowToWorld(m_game->mousePos());
	int count = 0;
	if (m_game->mouseLeftClick())
	{
		if (!m_togglePick)
		{
			for (auto& e : m_entityManager.getEntities())
			{
				if (!outOfBounds(e, mousePos) && e->tag() != "test")
				{
					m_pickedEntity = e;
					m_togglePick = true;
					break;
				}
				count++;
			}
		}
		else { 
			m_togglePick = false;
			for (auto& e : m_entityManager.getEntities())
			{
				if (!outOfBounds(e, mousePos) && e->tag() != "test") break;
				count++;
			}
		}
	}

	if (m_togglePick && m_allowPicking && !m_enableGridMod)
	{
		auto& eTrans = m_pickedEntity->getComponent<CTransform>();
		auto& eSize = m_pickedEntity->getComponent<CSize>();
		eTrans.prevPos = eTrans.pos;
		eTrans.pos = mousePos;
		Vec2 prevCellPos = midPixelToGrid(eTrans.prevPos, m_gridSize);
		Vec2 curCellPos = midPixelToGrid(eTrans.pos, m_gridSize);
		auto& prevCellColor = m_grid.get(prevCellPos.x, prevCellPos.y).color;
		auto& curCellColor = m_grid.get(curCellPos.x, curCellPos.y).color;
		if (!prevCellColor.equals(cellConfig["wall"]))	prevCellColor = cellConfig["path"];
		if (!curCellColor.equals(cellConfig["wall"]))	curCellColor = cellConfig["entity"];
		if (m_pickedEntity->hasComponent<CParent>())
		{
			auto& parentCom = m_pickedEntity->getComponent<CParent>();
			auto& parent = m_entityManager.getEntity(parentCom.id, parentCom.tag);
			parentCom.initialDistance = parent->getComponent<CTransform>().pos - eTrans.pos;
		}
	}
	if (count >= m_entityManager.getEntities().size()) m_pickedEntity = nullptr;
}

void Scene_Debug::modifyEntity()
{
	auto& Size = m_pickedEntity->getComponent<CSize>();
	auto& trans = m_pickedEntity->getComponent<CTransform>();
	auto& Anim = m_pickedEntity->getComponent<CAnimation>();
	auto& Layer = m_pickedEntity->getComponent<CLayer>();

	ImGui::Begin("Modify Entity");
	//Layer
	ImGui::Text("Layer");
	ImGui::InputInt("layer", &Layer.layer);

	ImGui::PushItemWidth(50);

	ImGui::Text("Tag: %s", m_pickedEntity->tag().c_str());
	ImGui::SameLine();
	ImGui::Text("Id: %zu", m_pickedEntity->id());

	if (ImGui::Button("Become Parent Candidate?"))
	{
		m_parentEntity = m_pickedEntity;
	}

	if (m_pickedEntity->hasComponent<CParent>())
	{
		auto& parent = m_pickedEntity->getComponent<CParent>();
		ImGui::Text("Current Parent");
		ImGui::Text("CP-Tag: %s", parent.tag.c_str());
		ImGui::SameLine();
		ImGui::Text("CP-Id: %zu", parent.id);
	}

	ImGui::Text("New Parent");
	if (m_parentEntity == nullptr)
	{
		ImGui::Text("None");
	}
	else
	{
		ImGui::Text("NP-Tag: %s", m_parentEntity->tag().c_str());
		ImGui::SameLine();
		ImGui::Text("NP-Id: %zu", m_parentEntity->id());
		ImGui::SameLine();
		if (ImGui::Button("Set Parent")) {
			changeParent(m_pickedEntity, m_parentEntity);
		}
	}

	if (ImGui::Button("Become Independent?"))
	{
		makeIndependent(m_pickedEntity);
	}

	if (m_pickedEntity->hasComponent<CAnimation>()) {
		modCurrTagAnimIndex = m_pickedEntity->getComponent<CAnimation>().index;
	}
	else modCurrTagAnimIndex = 0;

	ImGui::Text("Animation");
	if (ImGui::Combo("Animation", &modCurrTagAnimIndex, [](void* data, int idx, const char** out_text) {
		auto& vec = *static_cast<std::vector<std::string>*>(data);
		if (idx < 0 || idx >= vec.size()) return false;
		*out_text = vec[idx].c_str();
		return true;
		}, static_cast<void*>(&tagAnimations), tagAnimations.size())) {
		if (!m_pickedEntity->hasComponent<CAnimation>() && tagAnimations[modCurrTagAnimIndex] != "None")
		{
			auto& anim = m_game->assets().getAnimation(tagAnimations[modCurrTagAnimIndex]);
			auto modAnim = Animation(anim, Vec2(Size.size.x, Size.size.y));
			m_pickedEntity->addComponent<CAnimation>(modAnim, true, modCurrTagAnimIndex);
		}
		if (m_pickedEntity->hasComponent<CAnimation>() && tagAnimations[modCurrTagAnimIndex] != "None")
		{
			auto& anim = m_game->assets().getAnimation(tagAnimations[modCurrTagAnimIndex]);
			Anim.animation = Animation(anim, Vec2(Size.size.x, Size.size.y));
			Anim.index = modCurrTagAnimIndex;
		}
		if (m_pickedEntity->hasComponent<CAnimation>() && tagAnimations[modCurrTagAnimIndex] == "None")
		{
			m_pickedEntity->removeComponent<CAnimation>();
		}
	}
	Vec2 gPos = midPixelToGrid(m_pickedEntity, m_gridSize);
	Vec2 piPos = pixelToGrid(m_pickedEntity, m_gridSize);
	// Position
	ImGui::Text("Mid-Position");
	ImGui::InputFloat("PX", &gPos.x);
	ImGui::SameLine();
	ImGui::InputFloat("PY", &gPos.y);
	trans.pos = Vec2(gPos.x * m_gridSize.x, gPos.y * m_gridSize.y);

	//Gridloc
	ImGui::Text("Position");
	ImGui::Text("LX: %f", piPos.x);
	ImGui::SameLine();
	ImGui::Text("LY: %f", piPos.y);

	//Rotation
	ImGui::Text("Rotation");
	ImGui::InputFloat("Angle", &trans.angle);

	// Size
	ImGui::Text("Size");
	ImGui::InputFloat("SX", &Size.size.x);
	ImGui::SameLine();
	ImGui::InputFloat("SY", &Size.size.y);

	if (m_pickedEntity->hasComponent<CBoundingbox>())
	{
		auto& BB = m_pickedEntity->getComponent<CBoundingbox>();
		// Boundary (same as Size)
		ImGui::Text("Collision box");
		ImGui::InputFloat("BX", &BB.size.x);
		ImGui::SameLine();
		ImGui::InputFloat("BY", &BB.size.y);
	}
	else
	{
		if (ImGui::Button("Add BoundingBox")) {
			m_pickedEntity->addComponent<CBoundingbox>();
		}
	}

	// Speed and Max Speed
	ImGui::Text("Speed and Max Speed");
	ImGui::InputFloat("Speed", &trans.speed);
	ImGui::SameLine();
	ImGui::InputFloat("Max Speed", &trans.maxSpeed);

	if (m_pickedEntity->hasComponent<CAnimation>())
	{
		Anim.animation = Animation(Anim.animation, Vec2(Size.size.x, Size.size.y));
	}
	auto& Shape = m_pickedEntity->getComponent<CShape>().rectangle;
	Shape.setSize(sf::Vector2f(Size.size.x, Size.size.y));
	auto& ShapeColor = Shape.getFillColor();
	float r = (float)ShapeColor.r;
	float g = (float)ShapeColor.g;
	float b = (float)ShapeColor.b;
	//Color
	ImGui::Text("Color");
	ImGui::InputFloat("r", &r);
	ImGui::SameLine();
	ImGui::InputFloat("g", &g);
	ImGui::SameLine();
	ImGui::InputFloat("b", &b);
	Shape.setFillColor(sf::Color(r, g, b, ShapeColor.a));

	ImGui::PopItemWidth();
	if (ImGui::Button("Delete entity?")) {
		Vec2 cellPos = midPixelToGrid(trans.pos, m_gridSize);
		m_grid.get(cellPos.x, cellPos.y).color = cellConfig["path"];

		if (m_parentEntity && m_parentEntity->id() == m_pickedEntity->id() && m_parentEntity->tag() == m_pickedEntity->tag())
		{
			m_parentEntity = nullptr;
		}
		deleteEntity(m_pickedEntity);
		m_pickedEntity = nullptr;
	}
	ImGui::End();
}
void Scene_Debug::modifyLightSource()
{
	auto& trans = m_pickedEntity->getComponent<CTransform>();
	auto& Layer = m_pickedEntity->getComponent<CLayer>();
	auto& LightSource = m_pickedEntity->getComponent<CLightSource>();

	ImGui::Begin("Modify Entity");
	//Layer
	ImGui::Text("Layer");
	ImGui::InputInt("layer", &Layer.layer);

	int lines = LightSource.lightSource.getLines();
	ImGui::Text("Lines");
	ImGui::InputInt("No", &lines);
	LightSource.lightSource.setLines(lines);

	ImGui::PushItemWidth(50);

	ImGui::Text("Tag: %s", m_pickedEntity->tag().c_str());
	ImGui::SameLine();
	ImGui::Text("Id: %zu", m_pickedEntity->id());

	if (ImGui::Button("Become Parent Candidate?"))
	{
		m_parentEntity = m_pickedEntity;
	}

	if (m_pickedEntity->hasComponent<CParent>())
	{
		auto& parent = m_pickedEntity->getComponent<CParent>();
		ImGui::Text("Current Parent");
		ImGui::Text("CP-Tag: %s", parent.tag.c_str());
		ImGui::SameLine();
		ImGui::Text("CP-Id: %zu", parent.id);
	}

	ImGui::Text("New Parent");
	if (m_parentEntity == nullptr)
	{
		ImGui::Text("None");
	}
	else
	{
		ImGui::Text("NP-Tag: %s", m_parentEntity->tag().c_str());
		ImGui::SameLine();
		ImGui::Text("NP-Id: %zu", m_parentEntity->id());
		ImGui::SameLine();
		if (ImGui::Button("Set Parent")) {
			changeParent(m_pickedEntity, m_parentEntity);
		}
	}

	if (ImGui::Button("Become Independent?"))
	{
		makeIndependent(m_pickedEntity);
	}

	lightSrcTagTextIndex = LightSource.index;

	ImGui::Text("Texture");
	if (ImGui::Combo("Texture", &lightSrcTagTextIndex, [](void* data, int idx, const char** out_text) {
		auto& vec = *static_cast<std::vector<std::string>*>(data);
		if (idx < 0 || idx >= vec.size()) return false;
		*out_text = vec[idx].c_str();
		return true;
		}, static_cast<void*>(&tagTextures), tagTextures.size())) {
		if (tagTextures[lightSrcTagTextIndex] == "None")
		{
			LightSource.lightSource.setTexture(sf::Texture());
		}
		else LightSource.lightSource.setTexture(m_game->assets().getTexture(tagTextures[lightSrcTagTextIndex]));
	}

	LightSource.index = lightSrcTagTextIndex;

	Vec2 gPos = midPixelToGrid(m_pickedEntity, m_gridSize);
	Vec2 piPos = pixelToGrid(m_pickedEntity, m_gridSize);
	// Position
	ImGui::Text("Mid-Position");
	ImGui::InputFloat("PX", &gPos.x);
	ImGui::SameLine();
	ImGui::InputFloat("PY", &gPos.y);
	trans.pos = Vec2(gPos.x * m_gridSize.x, gPos.y * m_gridSize.y);
	LightSource.lightSource.setLightSource(trans.pos);

	//Gridloc
	ImGui::Text("Position");
	ImGui::Text("LX: %f", piPos.x);
	ImGui::SameLine();
	ImGui::Text("LY: %f", piPos.y);

	//Rotation
	ImGui::Text("Rotation");
	ImGui::InputFloat("Angle", &trans.angle);
	LightSource.lightSource.setAngle(trans.angle);

	float extent = LightSource.lightSource.getExtent();
	ImGui::Text("Extent");
	ImGui::InputFloat("Radius", &extent);
	LightSource.lightSource.setExtent(extent);

	std::pair<float, float> defAngles = LightSource.lightSource.getDefaultAngle();
	ImGui::Text("Default Angles");
	ImGui::InputFloat("Start", &defAngles.first);
	ImGui::SameLine();
	ImGui::InputFloat("End", &defAngles.second);
	LightSource.lightSource.setDefaultAngles(defAngles.first, defAngles.second);

	ImGui::PopItemWidth();

	sf::Color& c = LightSource.lightSource.getColor();
	float colorValues[] = { c.r / 255.0f, c.g / 255.0f, c.b / 255.0f, c.a / 255.0f };
	ImGui::Text("Color Picker");
	if (ImGui::ColorEdit4("Pick Color", colorValues)) {
		// Convert ImGui color to SFML color
		c.r = static_cast<sf::Uint8>(colorValues[0] * 255);
		c.g = static_cast<sf::Uint8>(colorValues[1] * 255);
		c.b = static_cast<sf::Uint8>(colorValues[2] * 255);
		c.a = static_cast<sf::Uint8>(colorValues[3] * 255);
	}

	//LightSource.lightSource.reMake();

	if (ImGui::Button("Delete entity?")) {
		Vec2 cellPos = midPixelToGrid(trans.pos, m_gridSize);
		m_grid.get(cellPos.x, cellPos.y).color = cellConfig["path"];

		if (m_parentEntity && m_parentEntity->id() == m_pickedEntity->id() && m_parentEntity->tag() == m_pickedEntity->tag())
		{
			m_parentEntity = nullptr;
		}
		deleteEntity(m_pickedEntity);
		m_pickedEntity = nullptr;
	}

	ImGui::End();
}
void Scene_Debug::modifyParticleSystem()
{
	auto& Layer = m_pickedEntity->getComponent<CLayer>();
	auto& ParticleSystem = m_pickedEntity->getComponent<CParticleSystem>().particleSystem;
}


void Scene_Debug::modifyPickedEntity()
{
	if (m_pickedEntity == nullptr) { return; }
	if (m_pickedEntity->tag() == "light") { modifyLightSource(); }
	else { modifyEntity(); }
}

void Scene_Debug::settings()
{
	ImGui::Begin("Settings");
	ImGui::Checkbox("Pick Entities", &m_allowPicking);
	ImGui::Checkbox("Grid Texture", &m_drawGridTexture);
	if (m_drawGridTexture)
	{
		ImGui::Checkbox("Enable grid mod", &m_enableGridMod);
		if (m_enableGridMod) {
			if (ImGui::Combo("Cells", &currentTagCellIndex, [](void* data, int idx, const char** out_text) {
				auto& vec = *static_cast<std::vector<std::string>*>(data);
				if (idx < 0 || idx >= vec.size()) return false;
				*out_text = vec[idx].c_str();
				return true;
				}, static_cast<void*>(&tagCells), tagCells.size())) {}
			Vec2 mousePos = windowToWorld(m_game->mousePos());
			ImGui::Checkbox("Enable Drag", &m_toggleDrag);
			if (!m_toggleDrag)
			{
				if (m_game->mouseRightClick())
				{
					Vec2 mouseGridLoc(mousePos.x / m_gridSize.x, mousePos.y / m_gridSize.y);
					m_grid.get(mouseGridLoc.x, mouseGridLoc.y).color = cellConfig[tagCells[currentTagCellIndex]];
				}
			}
			else 
			{
				if (m_game->mouseRightPress())
				{
					Vec2 mouseGridLoc(mousePos.x / m_gridSize.x, mousePos.y / m_gridSize.y);
					m_grid.get(mouseGridLoc.x, mouseGridLoc.y).color = cellConfig[tagCells[currentTagCellIndex]];
				}
			}
		}
	}

	else { m_enableGridMod = false; }
	ImGui::Text("------------------------------");
	if (ImGui::Button("Save Changes")) {
		writeToFile();
	}
	ImGui::End();
}

void Scene_Debug::writeToFile()
{
	writeMazeToFIle();
	sortEntities();
	for (auto& e : m_entityManager.getEntities())
	{
		writeEntityToFile(e);
	}
	std::ofstream outFile(m_levelPath);
	if (!outFile.is_open()) {
		std::cerr << "Error: Could not open the file " << m_levelPath << " for writing." << std::endl;
		return;
	}
	for (const auto& line : m_file) {
		outFile << line << std::endl;
	}
	outFile.close();
	m_file = m_fileCopy;
}

void Scene_Debug::lightSourceToFile(std::string& entityLine, int& index, const std::shared_ptr<Entity>& entity)
{
	auto& LightSource = entity->getComponent<CLightSource>();
	auto& defAngles = LightSource.lightSource.getDefaultAngle();
	auto& color = LightSource.lightSource.getColor();
	auto& texName = tagTextures[LightSource.index];
	entityLine += pixelToGrid(entity, m_gridSize).toString() + " ";
	entityLine += std::to_string(defAngles.first) + " " + std::to_string(defAngles.second) + " ";
	entityLine += std::to_string(color.r) + " " + std::to_string(color.g) + " " + std::to_string(color.b) + " " + std::to_string(color.a) + " ";
	entityLine += texName + " " + std::to_string(LightSource.lightSource.getExtent()) + " ";
	entityLine += std::to_string(LightSource.lightSource.getLines()) + " ";
	if (entity->hasComponent<CLayer>())
	{
		auto& eL = entity->getComponent<CLayer>();
		entityLine += " " + std::to_string(eL.layer);
	}
	else entityLine += " 0";
	bool hasChildren = false;
	if (entity->hasComponent<CChildren>())
	{
		auto& eC = entity->getComponent<CChildren>();
		entityLine += " " + std::to_string(eC.children.size());
		hasChildren = true;
	}
	else entityLine += " 0";
	m_file.insert(m_file.begin() + index, entityLine);
	index += 1;
	if (hasChildren)
	{
		for (auto& c : entity->getComponent<CChildren>().children)
		{
			auto& child = m_entityManager.getEntity(c.first, c.second);
			std::string newLine = "child " + child->tag() + " ";
			entityToFile(newLine, index, child);
		}
	}
}

void Scene_Debug::entityToFile(std::string& entityLine, int& index, const std::shared_ptr<Entity>& entity)
{
	if (entity->hasComponent<CLightSource>())
	{
		lightSourceToFile(entityLine, index, entity);
		return;
	}
	if (entity->hasComponent<CTransform>())
	{
		auto& eT = entity->getComponent<CTransform>();
		entityLine += pixelToGrid(entity, m_gridSize).toString() + " ";
		entityLine += std::to_string(eT.speed) + " " + std::to_string(eT.maxSpeed);
	}
	else entityLine += " 0 0 0 0";
	if (entity->hasComponent<CSize>())
	{
		auto& eS = entity->getComponent<CSize>();
		entityLine += " " + eS.size.toString();
	}
	else entityLine += " 0 0";
	if (entity->hasComponent<CBoundingbox>())
	{
		auto& eB = entity->getComponent<CBoundingbox>();
		entityLine += " " + eB.size.toString();
	}
	else entityLine += " 0 0";
	if (entity->hasComponent<CAnimation>())
	{
		auto& eA = entity->getComponent<CAnimation>();
		entityLine += " " + eA.animation.getName();
	}
	else entityLine += " None";
	if (entity->hasComponent<CLayer>())
	{
		auto& eL = entity->getComponent<CLayer>();
		entityLine += " " + std::to_string(eL.layer);
	}
	else entityLine += " 0";
	bool hasChildren = false;
	if (entity->hasComponent<CChildren>())
	{
		auto& eC = entity->getComponent<CChildren>();
		entityLine += " " + std::to_string(eC.children.size());
		hasChildren = true;
	}
	else entityLine += " 0";
	//std::cout << index << " " << m_file.size() << std::endl;
	m_file.insert(m_file.begin() + index, entityLine);
	index += 1;
	if (hasChildren)
	{
		for (auto& c : entity->getComponent<CChildren>().children)
		{
			auto& child = m_entityManager.getEntity(c.first, c.second);
			std::string newLine = "child " + child->tag() + " ";
			entityToFile(newLine, index, child);
		}
	}
}
void Scene_Debug::writeEntityToFile(const std::shared_ptr<Entity>& entity)
{
	for (int i = 0; i < m_file.size(); i++)
	{
		auto& line = m_file[i];
		std::istringstream iss(line);
		std::string _, name, list;
		iss >> _ >> name >> list;
		if (entity->tag() == name && list == "list")
		{
			if (entity->hasComponent<CParent>()) continue;
			std::string entityLine = entity->tag() + " ";
			int index = i + 1;
			entityToFile(entityLine, index, entity);
			break;
		}
	}
}

void Scene_Debug::writeMazeToFIle()
{
	for (int i = 0; i < m_file.size(); i++)
	{
		auto& line = m_file[i];
		std::istringstream iss(line);
		std::string name;
		iss >> name;
		if (name == "MazeBegin")
		{
			int j = i + 1;
			for (int row = 0; row < m_grid.size().y; row++)
			{
				std::vector<char> charVector;
				for (int col = 0; col < m_grid.size().x; col++)
				{
					if (m_grid.get(col, row).color.equals(cellConfig["wall"]))
					{
						charVector.push_back('H');
					}
					else charVector.push_back('o');
				}
				std::string mazeRowString(charVector.begin(), charVector.end());
				m_file.insert(m_file.begin() + j, mazeRowString);
				j++;
			}
			break;
		}
	}
}

void Scene_Debug::sortEntities()
{
	auto compare = [](const std::shared_ptr<Entity>& a, const std::shared_ptr<Entity>& b) -> bool{
		bool aHasLayer = a->hasComponent<CLayer>();
		bool bHasLayer = b->hasComponent<CLayer>();

		if (aHasLayer && bHasLayer) {
			auto& aL = a->getComponent<CLayer>();
			auto& bL = b->getComponent<CLayer>();
			return aL.layer > bL.layer;
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

void Scene_Debug::changeParent(const std::shared_ptr<Entity>& entity, const std::shared_ptr<Entity>& parent)
{
	if (parent->id() == entity->id() && parent->tag() == entity->tag()) return;
	if (entity->hasComponent<CChildren>()) {
		for (auto& c : entity->getComponent<CChildren>().children)
		{
			if (c.first == parent->id() && c.second == parent->tag()) return;
		}
	}
	makeIndependent(entity);
	auto& parentTrans = parent->getComponent<CTransform>();
	Vec2 distance = parentTrans.pos - entity->getComponent<CTransform>().pos;
	entity->addComponent<CParent>(parent->id(), parent->tag(), distance, parent->getComponent<CSize>().size, parentTrans.angle);
	if (!parent->hasComponent<CChildren>()) { parent->addComponent<CChildren>(); }
	parent->getComponent<CChildren>().children.push_back({ entity->id(), entity->tag() });
}

void Scene_Debug::makeIndependent(const std::shared_ptr<Entity>& entity)
{
	if (entity->hasComponent<CParent>()) 
	{
		auto& eParent = entity->getComponent<CParent>();
		auto& eParentEntity = m_entityManager.getEntity(eParent.id, eParent.tag);
		auto& ePPChildren = eParentEntity->getComponent<CChildren>().children;
		auto it = std::find(ePPChildren.begin(), ePPChildren.end(), std::make_pair(entity->id(), entity->tag()));
		if (it != ePPChildren.end()) {
			ePPChildren.erase(it);
		}
		if (ePPChildren.empty()) eParentEntity->removeComponent<CChildren>();
		entity->removeComponent<CParent>();
	}
}

void Scene_Debug::deleteEntity(const std::shared_ptr<Entity>& entity)
{
	makeIndependent(entity);
	if (entity->hasComponent<CChildren>())
	{
		auto& eChildren = entity->getComponent<CChildren>();
		for (int i = 0; i < eChildren.children.size(); i++)
		{
			auto& c = eChildren.children[i];
			if(m_parentEntity != nullptr && m_parentEntity->id() == c.first && m_parentEntity->tag() == c.second)
			{
				m_parentEntity = nullptr;
			}
			auto& cEntity = m_entityManager.getEntity(c.first, c.second);
			deleteEntity(cEntity);
			i--;
		}
	}
	m_entityManager.destroyEntity(entity);
}