#include "Editor.h"

void Command::Save(EntityManager& em, std::shared_ptr<Entity>& e)
{
	EMHistoryRecover.clear();
	size_t maxUndoDepth = 20;
	if (EMHistory.size() >= maxUndoDepth)
	{
		EMHistory.erase(EMHistory.begin());
	}
	EMHistory.push_back({ em, (e) ? std::make_shared<Entity>(*e) : nullptr });
}
void Command::Undo(EntityManager& em, std::shared_ptr<Entity>& e)
{
	if (!EMHistory.empty())
	{
		check = true;
		EM = EMHistory.back();
		EMHistoryRecover.push_back({ em, (e) ? std::make_shared<Entity>(*e) : nullptr });
		e = EM.second;
		EMHistory.pop_back();
	}
}

void Command::Redo(EntityManager& em, std::shared_ptr<Entity>& e)
{
	if (!EMHistoryRecover.empty())
	{
		check = true;
		EM = EMHistoryRecover.back();
		EMHistory.push_back({ em, (e) ? std::make_shared<Entity>(*e) : nullptr });
		e = EM.second;
		EMHistoryRecover.pop_back();
	}
}

void Command::Execute(EntityManager& em, std::shared_ptr<Entity>& e)
{
	if (check)
	{
		em = EM.first;
		e = EM.second;
		check = false;
	}
}

void Command::Clear()
{
	EMHistory.clear();
	EMHistoryRecover.clear();
	EM.first = EntityManager();
	EM.second = nullptr;
	check = false;
}




Editor::Editor()
{
	window.create(sf::VideoMode(1366, 768), "Abyss Engine");
	window.setFramerateLimit(60);
	isFullScreen = false;
	ImGui::SFML::Init(window);
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable docking
	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

	engineTabs["EngineSettings"] = std::make_unique<EngineSettings>();
	engineTabs["Inspector"] = std::make_unique<Inspector>();
	engineTabs["Files"] = std::make_unique<Files>();
	engineTabs["Console"] = std::make_unique<Console>();
	engineTabs["Hierarchy"] = std::make_unique<Hierarchy>();
	engineTabs["Display"] = std::make_unique<Display>();
	engineTabs["RenderModifier"] = std::make_unique<RenderModifier>();
	Init();
	command.Save(entityManager, selectedEntity);
}
void Editor::Init()
{
	for (auto& tab : engineTabs)
	{
		tab.second->Init(*this);
	}
}
void Editor::ProcessEvent(sf::Event& event) { ImGui::SFML::ProcessEvent(event); }
void Editor::Render() { ImGui::SFML::Render(window); }
void Editor::CloseTabs() {	ImGui::SFML::Shutdown(); }
void Editor::MainPage()
{
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
	ImGui::Begin("Main DockSpace", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoInputs
		| ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus);
	ImGuiID dockspaceID = ImGui::GetID("MainDockSpace");
	ImGui::DockSpace(dockspaceID);
	ImGui::End();
}
void Editor::Update()
{
	ImGui::SFML::Update(window, deltaClock.restart());
	MainPage();
	command.Execute(entityManager, selectedEntity);
	entityManager.update();
	updateAnimation();
	try
	{
		for (auto& tab : engineTabs)
		{
			tab.second->Update(*this);
		}
	}
	catch (const std::exception& e) {
		HandleError(e.what());
	}
}


void Editor::Save()
{
	command.Save(entityManager, selectedEntity);
	std::cout << "Save occured" << std::endl;
}

void Editor::Undo()
{
	command.Undo(entityManager, selectedEntity);
}

void Editor::Redo()
{
	command.Redo(entityManager, selectedEntity);
}

void Editor::Run()
{
	//SetUnhandledExceptionFilter(ExceptionFilter);
	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			ImGui::SFML::ProcessEvent(event);
			if (event.type == sf::Event::Closed || HasClosed()) {
				window.close();
			}
		}
		if (isFullScreen && !fullScreen)
		{
			window.create(sf::VideoMode(1366, 768), "Abyss Engine");
			isFullScreen = false;
		}

		if (!isFullScreen && fullScreen)
		{
			window.create(sf::VideoMode(1366, 768), "Abyss Engine", sf::Style::Fullscreen);
			isFullScreen = true;
		}
		// Render
		LoadScene();
		Update();
		window.clear();
		Render();
		window.display();
	}
}

void Editor::CloseEditor() { close = true; }
void Editor::ToggleFullScreen() { fullScreen = !fullScreen; }
const bool Editor::HasClosed() const { return close; }
const bool Editor::FullScreen() const { return fullScreen; }

bool Editor::isMouseInTab()
{
	auto windowPos = ImGui::GetWindowPos();
	auto contentMin = ImGui::GetWindowContentRegionMin();
	auto contentMax = ImGui::GetWindowContentRegionMax();
	ImVec2 mousePos = ImGui::GetMousePos();
	ImVec2 windowContentMin = ImVec2(windowPos.x + contentMin.x, windowPos.y + contentMin.y);
	ImVec2 windowContentMax = ImVec2(windowPos.x + contentMax.x, windowPos.y + contentMax.y);
	return (mousePos.x >= windowContentMin.x && mousePos.x <= windowContentMax.x &&
		mousePos.y >= windowContentMin.y && mousePos.y <= windowContentMax.y);
}

void Editor::HandleError(const std::string& error)
{
	engineTabs["Console"]->HandleError(error);
}

void Editor::addAnimation(const Animation& anim)
{
	anim_toAdd.push_back(anim);
}
void Editor::removeAnimation(const Animation& anim)
{
	anim_toRemove.push_back(anim);
}
void Editor::updateAnimation()
{
	for (auto& a : anim_toAdd)
	{
		if (animationMap.find(a.getName()) == animationMap.end())
		{
			animationMap[a.getName()] = a;
		}
	}

	for (auto& a : anim_toRemove)
	{
		if (animationMap.find(a.getName()) != animationMap.end())
		{
			animationMap.erase(a.getName());
		}
	}
	anim_toAdd.clear();
	anim_toRemove.clear();
}

void Editor::SaveScene()
{
	nlohmann::json SceneCollection;

	std::vector<nlohmann::json> entityRecord;
	std::vector<nlohmann::json> textureRecord;
	std::vector<nlohmann::json> animationRecord;
	std::vector<nlohmann::json> globalRecord;

	nlohmann::json globalField;
	globalField["DURATION"] = duration;
	globalField["TRANSLATE_FACTOR"] = translateFactor;
	globalField["SCALE_FACTOR"] = scaleFactor;
	globalField["ROTATE_FACTOR"] = rotateFactor;
	globalRecord.push_back(globalField);

	SceneCollection["GLOBAL"] += globalRecord;

	for (auto& t : texturePathMap)
	{
		nlohmann::json textureField;
		textureField["NAME"] = t.first;
		textureField["PATH"] = t.second;
		textureRecord.push_back(textureField);
	}

	SceneCollection["TEXTURE"] += textureRecord;

	for (auto& a : animationMap)
	{
		nlohmann::json animationField;
		animationField["NAME"] = a.first;
		animationField["TEXTURE"] = a.second.getTexName();
		animationField["FRAMECOUNT"] = a.second.getFrameCount();
		animationField["SPEED"] = a.second.getSpeed();
		animationRecord.push_back(animationField);
	}

	SceneCollection["ANIMATION"] += animationRecord;

	for (auto& e : entityManager.getEntities())
	{
		nlohmann::json entityField;
		entityField["TAG"] = e->tag();
		if (e->hasComponent<CTransform>())
		{
			auto& trans = e->getComponent<CTransform>();
			entityField["PX"] = trans.pos.x;
			entityField["PY"] = trans.pos.y;
			entityField["SX"] = trans.scale.x;
			entityField["SY"] = trans.scale.y;
			entityField["R"] = trans.angle;
		}
		if (e->hasComponent<CName>())
		{
			entityField["NAME"] = e->getComponent<CName>().name;
		}
		else {
			entityField["NAME"] = "Unnamed";
		}
		if (e->hasComponent<CRectangleShape>())
		{
			entityField["RENDER"] = "Rectangle";
		}
		else if (e->hasComponent<CCircleShape>())
		{
			entityField["RENDER"] = "Circle";
		}
		else if (e->hasComponent<CAnimation>())
		{
			entityField["RENDER"] = e->getComponent<CAnimation>().animation.getName();
		}
		else {
			entityField["RENDER"] = "None";
		}
		entityRecord.push_back(entityField);
	}
	SceneCollection["ENTITY"] += entityRecord;
	std::cout << SceneCollection.dump(4) << std::endl;

	std::ofstream outFile("Scene.json");
	if (outFile.is_open())
	{
		outFile << SceneCollection.dump(4); // Pretty print with 4 spaces
		outFile.close();
	}
	else
	{
		HandleError("Failed to open file for writing");
	}
}

void Editor::LoadScene()
{
	if (!load) return;

	nlohmann::json SceneCollection;

	// Open and read the JSON file
	std::ifstream inFile("Scene.json");
	if (inFile.is_open())
	{
		inFile >> SceneCollection;
		inFile.close();
	}
	else
	{
		HandleError("Failed to open file for reading");
		return;
	}

	command.Clear();
	selectedEntity = nullptr;
	
	if (SceneCollection.contains("GLOBAL"))
	{
		auto globalRecord = SceneCollection["GLOBAL"];
		//std::cout << globalRecord << std::endl;
		if (!globalRecord.empty())
		{
			duration = globalRecord[0][0]["DURATION"].get<float>();
			translateFactor = globalRecord[0][0]["TRANSLATE_FACTOR"].get<float>();
			scaleFactor = globalRecord[0][0]["SCALE_FACTOR"].get<float>();
			rotateFactor = globalRecord[0][0]["ROTATE_FACTOR"].get<float>();
		}
	}

	// Process TEXTURE
	texturePathMap.clear();
	textureMap.clear();
	if (SceneCollection.contains("TEXTURE"))
	{
		//std::cout << SceneCollection["TEXTURE"][0] << std::endl;
		for (const auto& textureField : SceneCollection["TEXTURE"][0])
		{
			std::string name = textureField["NAME"].get<std::string>();
			std::string path = textureField["PATH"].get<std::string>();
			texturePathMap[name] = path;
			sf::Texture texture;
			texture.loadFromFile(path);
			textureMap[name] = texture; // Load or process texture as needed
		}
	}

	// Process ANIMATION
	animationMap.clear();
	if (SceneCollection.contains("ANIMATION"))
	{
		//std::cout << SceneCollection["ANIMATION"][0] << std::endl;
		for (const auto& animationField : SceneCollection["ANIMATION"][0])
		{
			std::string name = animationField["NAME"].get<std::string>();
			if (name == "default_rectangle" || name == "default_circle")
			{
				animationMap[name] = Animation();
			}
			else
			{
				std::string texName = animationField["TEXTURE"].get<std::string>();
				int framecount = animationField["FRAMECOUNT"].get<int>();
				int speed = animationField["SPEED"].get<int>();
				Animation anim(name, texName, textureMap[texName], framecount, speed);
				animationMap[name] = anim;
			}
		}
	}
	
	entityManager = EntityManager();
	if (SceneCollection.contains("ENTITY"))
	{
		//std::cout << SceneCollection["ENTITY"][0] << std::endl;
		for (const auto& entityField : SceneCollection["ENTITY"][0])
		{
			auto entity = entityManager.addEntity(entityField["TAG"].get<std::string>());
			entity->addComponent<CName>(entityField["NAME"].get<std::string>());
			Vec2 pos(entityField["PX"].get<float>(), entityField["PY"].get<float>());
			Vec2 scale(entityField["SX"].get<float>(), entityField["SY"].get<float>());
			float angle = entityField["R"].get<float>();
			entity->addComponent<CTransform>(pos, scale, angle);
			entity->addComponent<CSize>();
			std::string renderName = entityField["RENDER"].get<std::string>();
			if (renderName == "Rectangle") entity->addComponent<CRectangleShape>();
			if (renderName == "Circle") entity->addComponent<CCircleShape>();
			if (renderName != "Circle" && renderName != "Rectangle") entity->addComponent<CAnimation>(animationMap[renderName]);
		}
	}
	
	load = false;
}