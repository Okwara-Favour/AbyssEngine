#include "Editor.h"

void Command::Save(EntityManager& em, std::shared_ptr<Entity>& e)
{
	EntityManager temp;
	em.copyTo(temp);
	EMHistoryRecover.clear();
	size_t maxUndoDepth = 20;
	if (EMHistory.size() >= maxUndoDepth)
	{
		EMHistory.erase(EMHistory.begin());
	}
	EMHistory.push_back({ temp, (e) ? std::make_shared<Entity>(*e) : nullptr });
}
void Command::Undo(EntityManager& em, std::shared_ptr<Entity>& e)
{
	if (!EMHistory.empty())
	{
		EntityManager temp;
		em.copyTo(temp);
		check = true;
		EMHistory.back().first.copyTo(EM.first);
		EM.second = EMHistory.back().second;
		EMHistoryRecover.push_back({ temp, (e) ? std::make_shared<Entity>(*e) : nullptr });
		//e = EM.second;
		EMHistory.pop_back();
	}
}

void Command::Redo(EntityManager& em, std::shared_ptr<Entity>& e)
{
	if (!EMHistoryRecover.empty())
	{
		EntityManager temp;
		em.copyTo(temp);
		check = true;
		EMHistoryRecover.back().first.copyTo(EM.first);
		EM.second = EMHistoryRecover.back().second;
		EMHistory.push_back({ temp, (e) ? std::make_shared<Entity>(*e) : nullptr });
		//e = EM.second;
		EMHistoryRecover.pop_back();
	}
}

void Command::Execute(EntityManager& em, std::shared_ptr<Entity>& e)
{
	if (check)
	{
		EM.first.copyTo(em);
		e = (EM.second) ? em.getEntity(EM.second->id()) : nullptr;
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
	engineTabs["RenderModifier"] = std::make_unique<RenderModifier>();
	engineTabs["Display"] = std::make_unique<Display>();

	scriptManager.lua.set_function("ConsoleText", &Editor::ConsoleText, this);
	scriptManager.lua.set_function("GetPrefabByName", &EntityManager::getEntityName, &prefabManager);
	scriptManager.lua["EManager"] = &entityManager;
	scriptManager.lua["Listener"] = &eventListener;

	Vec2::Lua(scriptManager.lua);
	CTransform::Lua(scriptManager.lua);
	CName::Lua(scriptManager.lua);
	CSize::Lua(scriptManager.lua);
	CBoxRender::Lua(scriptManager.lua);
	CCircleRender::Lua(scriptManager.lua);
	Entity::Lua(scriptManager.lua);
	EntityManager::Lua(scriptManager.lua);
	EventListener::Lua(scriptManager.lua);
	Camera::Lua(scriptManager.lua);
	CCircleCollider::Lua(scriptManager.lua);
	CBoxCollider::Lua(scriptManager.lua);
	CCamera::Lua(scriptManager.lua);
	LuaBindSFML(scriptManager.lua);
	Init();
	command.Save(entityManager, selectedEntity);

	LoadPrefabData();
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
void Editor::CloseTabs() 
{
	ImGui::SFML::Shutdown();
	scriptManager.Close();
}
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
	prefabManager.update();
	scriptManager.UpdateSOL();
	updateAnimation();
	try
	{
		for (auto& tab : engineTabs)
		{
			if(tab.first != "Display") tab.second->Update(*this);
		}
		engineTabs["Display"]->Update(*this);
	}
	catch (const std::exception& e) {
		ConsoleText(e.what());
	}
}


void Editor::Save()
{
	command.Save(entityManager, selectedEntity);
	//std::cout << "Save occured" << std::endl;
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
			window.setFramerateLimit(60);
			isFullScreen = false;
		}

		if (!isFullScreen && fullScreen)
		{
			window.create(sf::VideoMode(1366, 768), "Abyss Engine", sf::Style::Fullscreen);
			window.setFramerateLimit(60);
			isFullScreen = true;
		}
		// Render
		LoadScene();
		//window.clear();
		Update();
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

void Editor::ConsoleText(const std::string& error)
{
	engineTabs["Console"]->ConsoleText(error);
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

void Editor::AddChildEntitiesToSceneFile(nlohmann::json& dict, const std::shared_ptr<Entity> parent)
{
	auto& children = parent->getComponent<CChildren>().children;
	
	for (auto& child : children)
	{
		auto& e = entityManager.getEntity(child.first);
		nlohmann::json entityField;
		entityField["ID"] = e->id();
		entityField["TAG"] = e->tag();
		entityField["PREFAB"] = { e->prefabData().first, e->prefabData().second };
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
		if (e->hasComponent<CBoxRender>())
		{
			auto& boxRender = e->getComponent<CBoxRender>();
			entityField["RENDER"] = "Rectangle";
			entityField["FILLCOLOR"] = { boxRender.fillColor.r, boxRender.fillColor.g, boxRender.fillColor.b, boxRender.fillColor.a };
			entityField["OUTLINECOLOR"] = { boxRender.outlineColor.r, boxRender.outlineColor.g, boxRender.outlineColor.b, boxRender.outlineColor.a };
			entityField["THICKNESS"] = boxRender.outlineThickness;
		}
		else if (e->hasComponent<CCircleRender>())
		{
			auto& circleRender = e->getComponent<CCircleRender>();
			entityField["RENDER"] = "Circle";
			entityField["POINTCOUNT"] = circleRender.pointCount;
			entityField["FILLCOLOR"] = { circleRender.fillColor.r, circleRender.fillColor.g, circleRender.fillColor.b, circleRender.fillColor.a };
			entityField["OUTLINECOLOR"] = { circleRender.outlineColor.r, circleRender.outlineColor.g, circleRender.outlineColor.b, circleRender.outlineColor.a };
			entityField["THICKNESS"] = circleRender.outlineThickness;
		}
		else if (e->hasComponent<CAnimation>())
		{
			auto& animComponent = e->getComponent<CAnimation>();
			entityField["RENDER"] = animComponent.animation.getName();
			entityField["REPEAT"] = animComponent.repeat;
			entityField["FILLCOLOR"] = { animComponent.fillColor.r, animComponent.fillColor.g, animComponent.fillColor.b, animComponent.fillColor.a };
		}
		else {
			entityField["RENDER"] = "None";
		}
		if (e->hasComponent<CCamera>())
		{
			auto& cam = e->getComponent<CCamera>().camera;
			entityField["CAMERACOLOR"] = { cam.getColor().r, cam.getColor().g, cam.getColor().b, cam.getColor().a };
		}
		if (e->hasComponent<CBoxCollider>())
		{
			auto& boxOff = e->getComponent<CBoxCollider>().offset;
			entityField["BOXCOLLIDER"] = { boxOff.x, boxOff.y };
		}
		if (e->hasComponent<CCircleCollider>())
		{
			auto& circleOff = e->getComponent<CCircleCollider>().offset;
			entityField["CIRCLECOLLIDER"] = circleOff;
		}
		if (e->hasAnyScriptable())
		{
			nlohmann::json scriptablesArray = nlohmann::json::array();

			for (auto& s : e->getScriptables())
			{
				scriptablesArray.push_back(s.first);
			}

			entityField["SCRIPTABLES"] = scriptablesArray;
		}
		if (e->hasComponent<CChildren>())
		{
			entityField["HASCHILDREN"] = true;
			AddChildEntitiesToSceneFile(entityField, e);
		}
		else
		{
			entityField["HASCHILDREN"] = false;
		}
		dict["ENTITY"] += entityField;
	}
}

void Editor::LoadChildEntitiesFromSceneFile(const nlohmann::json& dict, const std::shared_ptr<Entity> parent)
{
	for (const auto& entityField : dict)
	{
		size_t id = entityField["ID"].get<size_t>();
		auto entity = entityManager.addEntity(entityField["TAG"].get<std::string>(), id);

		if (entityField.contains("PREFAB"))
		{
			auto prefabData = entityField["PREFAB"];
			entity->m_prefabID = { prefabData[0].get<bool>(), prefabData[1].get<size_t>() };
		}
		entity->addComponent<CName>(entityField["NAME"].get<std::string>());
		Vec2 pos(entityField["PX"].get<float>(), entityField["PY"].get<float>());
		Vec2 scale(entityField["SX"].get<float>(), entityField["SY"].get<float>());
		float angle = entityField["R"].get<float>();
		entity->addComponent<CTransform>(pos, scale, angle);
		entity->addComponent<CSize>();
		std::string renderName = entityField["RENDER"].get<std::string>();
		if (renderName == "Rectangle")
		{
			auto& BoxRender = entity->addComponent<CBoxRender>();
			BoxRender.outlineThickness = entityField["THICKNESS"].get<float>();
			auto fillColorArray = entityField["FILLCOLOR"];
			BoxRender.fillColor = sf::Color(
				fillColorArray[0].get<uint8_t>(),
				fillColorArray[1].get<uint8_t>(),
				fillColorArray[2].get<uint8_t>(),
				fillColorArray[3].get<uint8_t>()
			);

			// Load outline color for Rectangle
			auto outlineColorArray = entityField["OUTLINECOLOR"];
			BoxRender.outlineColor = sf::Color(
				outlineColorArray[0].get<uint8_t>(),
				outlineColorArray[1].get<uint8_t>(),
				outlineColorArray[2].get<uint8_t>(),
				outlineColorArray[3].get<uint8_t>()
			);
		}
		if (renderName == "Circle")
		{
			auto& CircleRender = entity->addComponent<CCircleRender>();
			CircleRender.outlineThickness = entityField["THICKNESS"].get<float>();
			CircleRender.pointCount = entityField["POINTCOUNT"].get<size_t>();
			auto fillColorArray = entityField["FILLCOLOR"];
			CircleRender.fillColor = sf::Color(
				fillColorArray[0].get<uint8_t>(),
				fillColorArray[1].get<uint8_t>(),
				fillColorArray[2].get<uint8_t>(),
				fillColorArray[3].get<uint8_t>()
			);

			// Load outline color for Circle
			auto outlineColorArray = entityField["OUTLINECOLOR"];
			CircleRender.outlineColor = sf::Color(
				outlineColorArray[0].get<uint8_t>(),
				outlineColorArray[1].get<uint8_t>(),
				outlineColorArray[2].get<uint8_t>(),
				outlineColorArray[3].get<uint8_t>()
			);
		}
		if (renderName != "Circle" && renderName != "Rectangle")
		{
			auto& Anim = entity->addComponent<CAnimation>(animationMap[renderName]);
			Anim.repeat = entityField["REPEAT"].get<bool>();
			auto fillColorArray = entityField["FILLCOLOR"];
			Anim.fillColor = sf::Color(
				fillColorArray[0].get<uint8_t>(),
				fillColorArray[1].get<uint8_t>(),
				fillColorArray[2].get<uint8_t>(),
				fillColorArray[3].get<uint8_t>()
			);
		}
		if (entityField.contains("CAMERACOLOR"))
		{
			auto fillColorArray = entityField["CAMERACOLOR"];
			entity->addComponent<CCamera>().camera.setColor(
				fillColorArray[0].get<uint8_t>(),
				fillColorArray[1].get<uint8_t>(),
				fillColorArray[2].get<uint8_t>(),
				fillColorArray[3].get<uint8_t>()
			);
		}
		if (entityField.contains("BOXCOLLIDER"))
		{
			auto boxOff = entityField["BOXCOLLIDER"];
			entity->addComponent<CBoxCollider>();
			entity->getComponent<CBoxCollider>().offset = Vec2(boxOff[0].get<float>(), boxOff[1].get<float>());
		}
		if (entityField.contains("CIRCLECOLLIDER"))
		{
			auto circleOff = entityField["CIRCLECOLLIDER"];
			entity->addComponent<CCircleCollider>();
			entity->getComponent<CCircleCollider>().offset = circleOff.get<float>();
		}
		if (entityField.contains("SCRIPTABLES"))
		{
			for (const auto& scriptName : entityField["SCRIPTABLES"])
			{
				std::string script = scriptName.get<std::string>();
				entity->addScriptable(script);
			}
		}
		if (entityField["HASCHILDREN"].get<bool>())
		{
			entity->addComponent<CChildren>();
			LoadChildEntitiesFromSceneFile(entityField["ENTITY"], entity);
		}
		auto& parentTrans = parent->getComponent<CTransform>();
		entity->addComponent<CParent>(parent->id(), parent->tag(), parentTrans.pos, parentTrans.scale, parentTrans.angle);
		parent->getComponent<CChildren>().children.push_back({ entity->id(), entity->tag() });
	}
}

void Editor::SaveScene()
{
	nlohmann::json SceneCollection;

	nlohmann::json globalField;
	globalField["TOTAL"] = entityManager.m_totalEntities;
	SceneCollection["STATISTICS"] += globalField;

	for (auto& e : entityManager.getEntities())
	{
		if (e->hasComponent<CParent>()) continue;
		nlohmann::json entityField;
		entityField["ID"] = e->id();
		entityField["TAG"] = e->tag();
		entityField["PREFAB"] = { e->prefabData().first, e->prefabData().second };
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
		if (e->hasComponent<CBoxRender>())
		{
			auto& boxRender = e->getComponent<CBoxRender>();
			entityField["RENDER"] = "Rectangle";
			entityField["FILLCOLOR"] = { boxRender.fillColor.r, boxRender.fillColor.g, boxRender.fillColor.b, boxRender.fillColor.a };
			entityField["OUTLINECOLOR"] = { boxRender.outlineColor.r, boxRender.outlineColor.g, boxRender.outlineColor.b, boxRender.outlineColor.a };
			entityField["THICKNESS"] = boxRender.outlineThickness;
		}
		else if (e->hasComponent<CCircleRender>())
		{
			auto& circleRender = e->getComponent<CCircleRender>();
			entityField["RENDER"] = "Circle";
			entityField["POINTCOUNT"] = circleRender.pointCount;
			entityField["FILLCOLOR"] = { circleRender.fillColor.r, circleRender.fillColor.g, circleRender.fillColor.b, circleRender.fillColor.a };
			entityField["OUTLINECOLOR"] = { circleRender.outlineColor.r, circleRender.outlineColor.g, circleRender.outlineColor.b, circleRender.outlineColor.a };
			entityField["THICKNESS"] = circleRender.outlineThickness;
		}
		else if (e->hasComponent<CAnimation>())
		{
			auto& animComponent = e->getComponent<CAnimation>();
			entityField["RENDER"] = animComponent.animation.getName();
			entityField["REPEAT"] = animComponent.repeat;
			entityField["FILLCOLOR"] = { animComponent.fillColor.r, animComponent.fillColor.g, animComponent.fillColor.b, animComponent.fillColor.a };
		}
		else {
			entityField["RENDER"] = "None";
		}
		if (e->hasComponent<CCamera>())
		{
			auto& cam = e->getComponent<CCamera>().camera;
			entityField["CAMERACOLOR"] = { cam.getColor().r, cam.getColor().g, cam.getColor().b, cam.getColor().a };
		}
		if (e->hasComponent<CBoxCollider>())
		{
			auto& boxOff = e->getComponent<CBoxCollider>().offset;
			entityField["BOXCOLLIDER"] = { boxOff.x, boxOff.y };
		}
		if (e->hasComponent<CCircleCollider>())
		{
			auto& circleOff = e->getComponent<CCircleCollider>().offset;
			entityField["CIRCLECOLLIDER"] = circleOff;
		}
		if (e->hasAnyScriptable())
		{
			nlohmann::json scriptablesArray = nlohmann::json::array();

			for (auto& s : e->getScriptables())
			{
				scriptablesArray.push_back(s.first);
			}

			entityField["SCRIPTABLES"] = scriptablesArray;
		}

		if (e->hasComponent<CChildren>())
		{
			entityField["HASCHILDREN"] = true;
			AddChildEntitiesToSceneFile(entityField, e);
		}
		else
		{
			entityField["HASCHILDREN"] = false;
		}
		SceneCollection["ENTITY"] += entityField;
	}
	
	std::ofstream outFile(saveFile);
	if (outFile.is_open())
	{
		outFile << SceneCollection.dump(4); // Pretty print with 4 spaces
		outFile.close();
	}
	else
	{
		ConsoleText("Failed to open file for writing");
	}
}

void Editor::LoadScene()
{
	if (!load) return;

	nlohmann::json SceneCollection;

	// Open and read the JSON file
	std::ifstream inFile(saveFile);
	if (inFile.is_open())
	{
		inFile >> SceneCollection;
		inFile.close();
	}
	else
	{
		ConsoleText("Failed to open file for reading");
		return;
	}

	command.Clear();
	selectedEntity = nullptr;

	scriptManager.ResetClass();
	entityManager = EntityManager();

	if (SceneCollection.contains("STATISTICS"))
	{
		auto globalRecord = SceneCollection["STATISTICS"];
		if (!globalRecord.empty())
		{
			entityManager.m_totalEntities = globalRecord[0]["TOTAL"].get<size_t>();
		}
	}

	if (SceneCollection.contains("ENTITY"))
	{
		//std::cout << SceneCollection["ENTITY"][0] << std::endl;
		for (const auto& entityField : SceneCollection["ENTITY"])
		{
			size_t id = entityField["ID"].get<size_t>();
			auto entity = entityManager.addEntity(entityField["TAG"].get<std::string>(), id);

			if (entityField.contains("PREFAB"))
			{
				auto prefabData = entityField["PREFAB"];
				entity->m_prefabID = { prefabData[0].get<bool>(), prefabData[1].get<size_t>() };
			}
			entity->addComponent<CName>(entityField["NAME"].get<std::string>());
			Vec2 pos(entityField["PX"].get<float>(), entityField["PY"].get<float>());
			Vec2 scale(entityField["SX"].get<float>(), entityField["SY"].get<float>());
			float angle = entityField["R"].get<float>();
			entity->addComponent<CTransform>(pos, scale, angle);
			entity->addComponent<CSize>();
			std::string renderName = entityField["RENDER"].get<std::string>();
			if (renderName == "Rectangle") 
			{
				auto& BoxRender = entity->addComponent<CBoxRender>();
				BoxRender.outlineThickness = entityField["THICKNESS"].get<float>();
				auto fillColorArray = entityField["FILLCOLOR"];
				BoxRender.fillColor = sf::Color(
					fillColorArray[0].get<uint8_t>(),
					fillColorArray[1].get<uint8_t>(),
					fillColorArray[2].get<uint8_t>(),
					fillColorArray[3].get<uint8_t>()
				);

				// Load outline color for Rectangle
				auto outlineColorArray = entityField["OUTLINECOLOR"];
				BoxRender.outlineColor = sf::Color(
					outlineColorArray[0].get<uint8_t>(),
					outlineColorArray[1].get<uint8_t>(),
					outlineColorArray[2].get<uint8_t>(),
					outlineColorArray[3].get<uint8_t>()
				);
			}
			if (renderName == "Circle")
			{
				auto& CircleRender = entity->addComponent<CCircleRender>();
				CircleRender.outlineThickness = entityField["THICKNESS"].get<float>();
				CircleRender.pointCount = entityField["POINTCOUNT"].get<size_t>();
				auto fillColorArray = entityField["FILLCOLOR"];
				CircleRender.fillColor = sf::Color(
					fillColorArray[0].get<uint8_t>(),
					fillColorArray[1].get<uint8_t>(),
					fillColorArray[2].get<uint8_t>(),
					fillColorArray[3].get<uint8_t>()
				);

				// Load outline color for Circle
				auto outlineColorArray = entityField["OUTLINECOLOR"];
				CircleRender.outlineColor = sf::Color(
					outlineColorArray[0].get<uint8_t>(),
					outlineColorArray[1].get<uint8_t>(),
					outlineColorArray[2].get<uint8_t>(),
					outlineColorArray[3].get<uint8_t>()
				);
			}
			if (renderName != "Circle" && renderName != "Rectangle") 
			{
				if (renderName != "None")
				{
					auto& Anim = entity->addComponent<CAnimation>(animationMap[renderName]);
					Anim.repeat = entityField["REPEAT"].get<bool>();
					auto fillColorArray = entityField["FILLCOLOR"];
					Anim.fillColor = sf::Color(
						fillColorArray[0].get<uint8_t>(),
						fillColorArray[1].get<uint8_t>(),
						fillColorArray[2].get<uint8_t>(),
						fillColorArray[3].get<uint8_t>()
					);
				}
			}
			if (entityField.contains("CAMERACOLOR"))
			{
				auto fillColorArray = entityField["CAMERACOLOR"];
				entity->addComponent<CCamera>().camera.setColor(
					fillColorArray[0].get<uint8_t>(),
					fillColorArray[1].get<uint8_t>(),
					fillColorArray[2].get<uint8_t>(),
					fillColorArray[3].get<uint8_t>()
				);
			}
			if (entityField.contains("BOXCOLLIDER"))
			{
				auto boxOff = entityField["BOXCOLLIDER"];
				entity->addComponent<CBoxCollider>();
				entity->getComponent<CBoxCollider>().offset = Vec2(boxOff[0].get<float>(), boxOff[1].get<float>());
			}
			if (entityField.contains("CIRCLECOLLIDER"))
			{
				auto circleOff = entityField["CIRCLECOLLIDER"];
				entity->addComponent<CCircleCollider>();
				entity->getComponent<CCircleCollider>().offset = circleOff.get<float>();
			}
			if (entityField.contains("SCRIPTABLES"))
			{
				for (const auto& scriptName : entityField["SCRIPTABLES"])
				{
					std::string script = scriptName.get<std::string>();
					entity->addScriptable(script);
				}
			}
			if (entityField["HASCHILDREN"].get<bool>())
			{
				entity->addComponent<CChildren>();
				LoadChildEntitiesFromSceneFile(entityField["ENTITY"], entity);
			}
		}
	}
	load = false;
}

void Editor::SaveChildPrefabData(nlohmann::json& dict, const std::shared_ptr<Entity> parent)
{
	auto& children = parent->getComponent<CChildren>().childEntities;
	for (auto& child : children)
	{
		auto childEntity = std::any_cast<std::shared_ptr<Entity>>(child.second);
		auto& e = childEntity;
		nlohmann::json entityField;
		entityField["ID"] = e->id();
		entityField["TAG"] = e->tag();
		entityField["PREFAB"] = { e->prefabData().first, e->prefabData().second };
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
		if (e->hasComponent<CBoxRender>())
		{
			auto& boxRender = e->getComponent<CBoxRender>();
			entityField["RENDER"] = "Rectangle";
			entityField["FILLCOLOR"] = { boxRender.fillColor.r, boxRender.fillColor.g, boxRender.fillColor.b, boxRender.fillColor.a };
			entityField["OUTLINECOLOR"] = { boxRender.outlineColor.r, boxRender.outlineColor.g, boxRender.outlineColor.b, boxRender.outlineColor.a };
			entityField["THICKNESS"] = boxRender.outlineThickness;
		}
		else if (e->hasComponent<CCircleRender>())
		{
			auto& circleRender = e->getComponent<CCircleRender>();
			entityField["RENDER"] = "Circle";
			entityField["POINTCOUNT"] = circleRender.pointCount;
			entityField["FILLCOLOR"] = { circleRender.fillColor.r, circleRender.fillColor.g, circleRender.fillColor.b, circleRender.fillColor.a };
			entityField["OUTLINECOLOR"] = { circleRender.outlineColor.r, circleRender.outlineColor.g, circleRender.outlineColor.b, circleRender.outlineColor.a };
			entityField["THICKNESS"] = circleRender.outlineThickness;
		}
		else if (e->hasComponent<CAnimation>())
		{
			auto& animComponent = e->getComponent<CAnimation>();
			entityField["RENDER"] = animComponent.animation.getName();
			entityField["REPEAT"] = animComponent.repeat;
			entityField["FILLCOLOR"] = { animComponent.fillColor.r, animComponent.fillColor.g, animComponent.fillColor.b, animComponent.fillColor.a };
		}
		else {
			entityField["RENDER"] = "None";
		}
		if (e->hasComponent<CCamera>())
		{
			auto& cam = e->getComponent<CCamera>().camera;
			entityField["CAMERACOLOR"] = { cam.getColor().r, cam.getColor().g, cam.getColor().b, cam.getColor().a };
		}
		if (e->hasComponent<CBoxCollider>())
		{
			auto& boxOff = e->getComponent<CBoxCollider>().offset;
			entityField["BOXCOLLIDER"] = { boxOff.x, boxOff.y };
		}
		if (e->hasComponent<CCircleCollider>())
		{
			auto& circleOff = e->getComponent<CCircleCollider>().offset;
			entityField["CIRCLECOLLIDER"] = circleOff;
		}
		if (e->hasAnyScriptable())
		{
			nlohmann::json scriptablesArray = nlohmann::json::array();

			for (auto& s : e->getScriptables())
			{
				scriptablesArray.push_back(s.first);
			}

			entityField["SCRIPTABLES"] = scriptablesArray;
		}
		if (e->hasComponent<CChildren>())
		{
			entityField["HASCHILDREN"] = true;
			
			SaveChildPrefabData(entityField, e);
		}
		else
		{
			entityField["HASCHILDREN"] = false;
		}
		dict["ENTITY"] += entityField;
	}
}
void Editor::LoadChildPrefabData(const nlohmann::json& dict, const std::shared_ptr<Entity> parent)
{
	for (const auto& entityField : dict)
	{
		size_t id = entityField["ID"].get<size_t>();
		auto entity = prefabManager.createEntity(entityField["TAG"].get<std::string>(), id);

		if (entityField.contains("PREFAB"))
		{
			auto prefabData = entityField["PREFAB"];
			entity->m_prefabID = { prefabData[0].get<bool>(), prefabData[1].get<size_t>() };
		}
		entity->addComponent<CName>(entityField["NAME"].get<std::string>());
		Vec2 pos(entityField["PX"].get<float>(), entityField["PY"].get<float>());
		Vec2 scale(entityField["SX"].get<float>(), entityField["SY"].get<float>());
		float angle = entityField["R"].get<float>();
		entity->addComponent<CTransform>(pos, scale, angle);
		entity->addComponent<CSize>();
		std::string renderName = entityField["RENDER"].get<std::string>();
		if (renderName == "Rectangle")
		{
			auto& BoxRender = entity->addComponent<CBoxRender>();
			BoxRender.outlineThickness = entityField["THICKNESS"].get<float>();
			auto fillColorArray = entityField["FILLCOLOR"];
			BoxRender.fillColor = sf::Color(
				fillColorArray[0].get<uint8_t>(),
				fillColorArray[1].get<uint8_t>(),
				fillColorArray[2].get<uint8_t>(),
				fillColorArray[3].get<uint8_t>()
			);

			// Load outline color for Rectangle
			auto outlineColorArray = entityField["OUTLINECOLOR"];
			BoxRender.outlineColor = sf::Color(
				outlineColorArray[0].get<uint8_t>(),
				outlineColorArray[1].get<uint8_t>(),
				outlineColorArray[2].get<uint8_t>(),
				outlineColorArray[3].get<uint8_t>()
			);
		}
		if (renderName == "Circle")
		{
			auto& CircleRender = entity->addComponent<CCircleRender>();
			CircleRender.outlineThickness = entityField["THICKNESS"].get<float>();
			CircleRender.pointCount = entityField["POINTCOUNT"].get<size_t>();
			auto fillColorArray = entityField["FILLCOLOR"];
			CircleRender.fillColor = sf::Color(
				fillColorArray[0].get<uint8_t>(),
				fillColorArray[1].get<uint8_t>(),
				fillColorArray[2].get<uint8_t>(),
				fillColorArray[3].get<uint8_t>()
			);

			// Load outline color for Circle
			auto outlineColorArray = entityField["OUTLINECOLOR"];
			CircleRender.outlineColor = sf::Color(
				outlineColorArray[0].get<uint8_t>(),
				outlineColorArray[1].get<uint8_t>(),
				outlineColorArray[2].get<uint8_t>(),
				outlineColorArray[3].get<uint8_t>()
			);
		}
		if (renderName != "Circle" && renderName != "Rectangle")
		{
			auto& Anim = entity->addComponent<CAnimation>(animationMap[renderName]);
			Anim.repeat = entityField["REPEAT"].get<bool>();
			auto fillColorArray = entityField["FILLCOLOR"];
			Anim.fillColor = sf::Color(
				fillColorArray[0].get<uint8_t>(),
				fillColorArray[1].get<uint8_t>(),
				fillColorArray[2].get<uint8_t>(),
				fillColorArray[3].get<uint8_t>()
			);
		}
		if (entityField.contains("CAMERACOLOR"))
		{
			auto fillColorArray = entityField["CAMERACOLOR"];
			entity->addComponent<CCamera>().camera.setColor(
				fillColorArray[0].get<uint8_t>(),
				fillColorArray[1].get<uint8_t>(),
				fillColorArray[2].get<uint8_t>(),
				fillColorArray[3].get<uint8_t>()
			);
		}
		if (entityField.contains("BOXCOLLIDER"))
		{
			auto boxOff = entityField["BOXCOLLIDER"];
			entity->addComponent<CBoxCollider>();
			entity->getComponent<CBoxCollider>().offset = Vec2(boxOff[0].get<float>(), boxOff[1].get<float>());
		}
		if (entityField.contains("CIRCLECOLLIDER"))
		{
			auto circleOff = entityField["CIRCLECOLLIDER"];
			entity->addComponent<CCircleCollider>();
			entity->getComponent<CCircleCollider>().offset = circleOff.get<float>();
		}
		if (entityField.contains("SCRIPTABLES"))
		{
			for (const auto& scriptName : entityField["SCRIPTABLES"])
			{
				std::string script = scriptName.get<std::string>();
				entity->addScriptable(script);
			}
		}
		if (entityField["HASCHILDREN"].get<bool>())
		{
			entity->addComponent<CChildren>();
			LoadChildPrefabData(entityField["ENTITY"], entity);
		}
		auto& parentTrans = parent->getComponent<CTransform>();
		entity->addComponent<CParent>(parent->id(), parent->tag(), parentTrans.pos, parentTrans.scale, parentTrans.angle);
		parent->getComponent<CChildren>().children.push_back({ entity->id(), entity->tag() });
		parent->getComponent<CChildren>().childEntities[entity->id()] = entity;
	}
}
void Editor::SavePrefabData()
{
	nlohmann::json SceneCollection;

	nlohmann::json globalField;
	globalField["TOTAL"] = prefabManager.m_totalEntities;
	globalField["DURATION"] = duration;
	globalField["TRANSLATE_FACTOR"] = translateFactor;
	globalField["SCALE_FACTOR"] = scaleFactor;
	globalField["ROTATE_FACTOR"] = rotateFactor;

	SceneCollection["STATISTICS"] += globalField;

	for (auto& t : texturePathMap)
	{
		nlohmann::json textureField;
		textureField["NAME"] = t.first;
		textureField["PATH"] = t.second;
		SceneCollection["TEXTURE"] += textureField;
	}

	for (auto& a : animationMap)
	{
		nlohmann::json animationField;
		animationField["NAME"] = a.first;
		animationField["TEXTURE"] = a.second.getTexName();
		animationField["FRAMECOUNT"] = a.second.getFrameCount();
		animationField["SPEED"] = a.second.getSpeed();
		SceneCollection["ANIMATION"] += animationField;
	}

	for (auto& s : scriptManager.scriptsDirectoryMap)
	{
		if (scriptManager.hasEnvironment(s.first))
		{
			nlohmann::json scriptField;
			scriptField["NAME"] = s.first + ".lua";
			scriptField["DIRECTORY"] = s.second.string();
			SceneCollection["SCRIPT"] += scriptField;
		}
	}

	for (auto& e : prefabManager.getEntities())
	{
		if (e->hasComponent<CParent>()) continue;
		nlohmann::json entityField;
		entityField["ID"] = e->id();
		entityField["TAG"] = e->tag();
		entityField["PREFAB"] = { e->prefabData().first, e->prefabData().second };
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
		if (e->hasComponent<CBoxRender>())
		{
			auto& boxRender = e->getComponent<CBoxRender>();
			entityField["RENDER"] = "Rectangle";
			entityField["FILLCOLOR"] = { boxRender.fillColor.r, boxRender.fillColor.g, boxRender.fillColor.b, boxRender.fillColor.a };
			entityField["OUTLINECOLOR"] = { boxRender.outlineColor.r, boxRender.outlineColor.g, boxRender.outlineColor.b, boxRender.outlineColor.a };
			entityField["THICKNESS"] = boxRender.outlineThickness;
		}
		else if (e->hasComponent<CCircleRender>())
		{
			auto& circleRender = e->getComponent<CCircleRender>();
			entityField["RENDER"] = "Circle";
			entityField["POINTCOUNT"] = circleRender.pointCount;
			entityField["FILLCOLOR"] = { circleRender.fillColor.r, circleRender.fillColor.g, circleRender.fillColor.b, circleRender.fillColor.a };
			entityField["OUTLINECOLOR"] = { circleRender.outlineColor.r, circleRender.outlineColor.g, circleRender.outlineColor.b, circleRender.outlineColor.a };
			entityField["THICKNESS"] = circleRender.outlineThickness;
		}
		else if (e->hasComponent<CAnimation>())
		{
			auto& animComponent = e->getComponent<CAnimation>();
			entityField["RENDER"] = animComponent.animation.getName();
			entityField["REPEAT"] = animComponent.repeat;
			entityField["FILLCOLOR"] = { animComponent.fillColor.r, animComponent.fillColor.g, animComponent.fillColor.b, animComponent.fillColor.a };
		}
		else {
			entityField["RENDER"] = "None";
		}
		if (e->hasComponent<CCamera>())
		{
			auto& cam = e->getComponent<CCamera>().camera;
			entityField["CAMERACOLOR"] = { cam.getColor().r, cam.getColor().g, cam.getColor().b, cam.getColor().a };
		}
		if (e->hasComponent<CBoxCollider>())
		{
			auto& boxOff = e->getComponent<CBoxCollider>().offset;
			entityField["BOXCOLLIDER"] = { boxOff.x, boxOff.y };
		}
		if (e->hasComponent<CCircleCollider>())
		{
			auto& circleOff = e->getComponent<CCircleCollider>().offset;
			entityField["CIRCLECOLLIDER"] = circleOff;
		}
		if (e->hasAnyScriptable())
		{
			nlohmann::json scriptablesArray = nlohmann::json::array();

			for (auto& s : e->getScriptables())
			{
				scriptablesArray.push_back(s.first);
			}

			entityField["SCRIPTABLES"] = scriptablesArray;
		}

		if (e->hasComponent<CChildren>())
		{
			entityField["HASCHILDREN"] = true;
			SaveChildPrefabData(entityField, e);
		}
		else
		{
			entityField["HASCHILDREN"] = false;
		}
		SceneCollection["ENTITY"] += entityField;
	}
	
	std::ofstream outFile("Prefab.json");
	if (outFile.is_open())
	{
		outFile << SceneCollection.dump(4); // Pretty print with 4 spaces
		outFile.close();
	}
	else
	{
		ConsoleText("Failed to open file for writing");
	}
}
void Editor::LoadPrefabData()
{
	nlohmann::json SceneCollection;

	// Open and read the JSON file
	std::ifstream inFile("Prefab.json");
	if (inFile.is_open())
	{
		inFile >> SceneCollection;
		inFile.close();
	}
	else
	{
		ConsoleText("Failed to open file for reading");
		return;
	}

	if (SceneCollection.contains("STATISTICS"))
	{
		auto globalRecord = SceneCollection["STATISTICS"];
		if (!globalRecord.empty())
		{
			prefabManager.m_totalEntities = globalRecord[0]["TOTAL"].get<size_t>();
			duration = globalRecord[0]["DURATION"].get<float>();
			translateFactor = globalRecord[0]["TRANSLATE_FACTOR"].get<float>();
			scaleFactor = globalRecord[0]["SCALE_FACTOR"].get<float>();
			rotateFactor = globalRecord[0]["ROTATE_FACTOR"].get<float>();
		}
	}
	
	if (SceneCollection.contains("TEXTURE"))
	{
		//std::cout << SceneCollection["TEXTURE"][0] << std::endl;
		for (const auto& textureField : SceneCollection["TEXTURE"])
		{
			std::string name = textureField["NAME"].get<std::string>();
			std::string path = textureField["PATH"].get<std::string>();
			texturePathMap[name] = path;
			sf::Texture texture;
			texture.loadFromFile(path);
			textureMap[name] = texture; // Load or process texture as needed
		}
	}

	if (SceneCollection.contains("ANIMATION"))
	{
		//std::cout << SceneCollection["ANIMATION"][0] << std::endl;
		for (const auto& animationField : SceneCollection["ANIMATION"])
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

	if (SceneCollection.contains("SCRIPT"))
	{
		for (const auto& scriptField : SceneCollection["SCRIPT"])
		{
			std::string filename = scriptField["NAME"].get<std::string>();
			std::string directoryName = scriptField["DIRECTORY"].get<std::string>();
			fs::path directory(directoryName);
			scriptManager.LoadScript(*this, filename, directory);
		}
	}

	if (SceneCollection.contains("ENTITY"))
	{
		for (const auto& entityField : SceneCollection["ENTITY"])
		{
			size_t id = entityField["ID"].get<size_t>();
			auto entity = prefabManager.addEntity(entityField["TAG"].get<std::string>(), id);
			entity->setPrefab(true);

			if (entityField.contains("PREFAB"))
			{
				auto prefabData = entityField["PREFAB"];
				entity->m_prefabID = { prefabData[0].get<bool>(), prefabData[1].get<size_t>() };
			}
			entity->addComponent<CName>(entityField["NAME"].get<std::string>());
			Vec2 pos(entityField["PX"].get<float>(), entityField["PY"].get<float>());
			Vec2 scale(entityField["SX"].get<float>(), entityField["SY"].get<float>());
			float angle = entityField["R"].get<float>();
			entity->addComponent<CTransform>(pos, scale, angle);
			entity->addComponent<CSize>();
			std::string renderName = entityField["RENDER"].get<std::string>();
			if (renderName == "Rectangle")
			{
				auto& BoxRender = entity->addComponent<CBoxRender>();
				BoxRender.outlineThickness = entityField["THICKNESS"].get<float>();
				auto fillColorArray = entityField["FILLCOLOR"];
				BoxRender.fillColor = sf::Color(
					fillColorArray[0].get<uint8_t>(),
					fillColorArray[1].get<uint8_t>(),
					fillColorArray[2].get<uint8_t>(),
					fillColorArray[3].get<uint8_t>()
				);

				// Load outline color for Rectangle
				auto outlineColorArray = entityField["OUTLINECOLOR"];
				BoxRender.outlineColor = sf::Color(
					outlineColorArray[0].get<uint8_t>(),
					outlineColorArray[1].get<uint8_t>(),
					outlineColorArray[2].get<uint8_t>(),
					outlineColorArray[3].get<uint8_t>()
				);
			}
			if (renderName == "Circle")
			{
				auto& CircleRender = entity->addComponent<CCircleRender>();
				CircleRender.outlineThickness = entityField["THICKNESS"].get<float>();
				CircleRender.pointCount = entityField["POINTCOUNT"].get<size_t>();
				auto fillColorArray = entityField["FILLCOLOR"];
				CircleRender.fillColor = sf::Color(
					fillColorArray[0].get<uint8_t>(),
					fillColorArray[1].get<uint8_t>(),
					fillColorArray[2].get<uint8_t>(),
					fillColorArray[3].get<uint8_t>()
				);

				// Load outline color for Circle
				auto outlineColorArray = entityField["OUTLINECOLOR"];
				CircleRender.outlineColor = sf::Color(
					outlineColorArray[0].get<uint8_t>(),
					outlineColorArray[1].get<uint8_t>(),
					outlineColorArray[2].get<uint8_t>(),
					outlineColorArray[3].get<uint8_t>()
				);
			}
			if (renderName != "Circle" && renderName != "Rectangle")
			{
				if (renderName != "None")
				{
					auto& Anim = entity->addComponent<CAnimation>(animationMap[renderName]);
					Anim.repeat = entityField["REPEAT"].get<bool>();
					auto fillColorArray = entityField["FILLCOLOR"];
					Anim.fillColor = sf::Color(
						fillColorArray[0].get<uint8_t>(),
						fillColorArray[1].get<uint8_t>(),
						fillColorArray[2].get<uint8_t>(),
						fillColorArray[3].get<uint8_t>()
					);
				}
			}
			if (entityField.contains("CAMERACOLOR"))
			{
				auto fillColorArray = entityField["CAMERACOLOR"];
				entity->addComponent<CCamera>().camera.setColor(
					fillColorArray[0].get<uint8_t>(),
					fillColorArray[1].get<uint8_t>(),
					fillColorArray[2].get<uint8_t>(),
					fillColorArray[3].get<uint8_t>()
				);
			}
			if (entityField.contains("BOXCOLLIDER"))
			{
				auto boxOff = entityField["BOXCOLLIDER"];
				entity->addComponent<CBoxCollider>();
				entity->getComponent<CBoxCollider>().offset = Vec2(boxOff[0].get<float>(), boxOff[1].get<float>());
			}
			if (entityField.contains("CIRCLECOLLIDER"))
			{
				auto circleOff = entityField["CIRCLECOLLIDER"];
				entity->addComponent<CCircleCollider>();
				entity->getComponent<CCircleCollider>().offset = circleOff.get<float>();
			}
			if (entityField.contains("SCRIPTABLES"))
			{
				for (const auto& scriptName : entityField["SCRIPTABLES"])
				{
					std::string script = scriptName.get<std::string>();
					entity->addScriptable(script);
				}
			}
			if (entityField["HASCHILDREN"].get<bool>())
			{
				entity->addComponent<CChildren>();
				LoadChildPrefabData(entityField["ENTITY"], entity);
			}
		}
	}
}

void Editor::StartGame()
{
	if (gameMode) return;
	gameMode = true;
	entityManager.copyTo(EMCOPY);
	selectedEntity = nullptr;
	//scriptManager.RecompileClasses(*this);
}

void Editor::QuitGame()
{
	if (!gameMode) return;
	gameMode = false;
	EMCOPY.copyTo(entityManager);
	scriptManager.ResolveMissingSharedSOL(*this);
	selectedEntity = nullptr;
}

void Editor::LuaBindSFML(sol::state& lua)
{
	lua.new_usertype<sf::RectangleShape>("RectangleShape",
		sol::constructors<sf::RectangleShape()>()
		// No members or functions exposed
	);

	lua.new_usertype<sf::CircleShape>("CircleShape",
		sol::constructors<sf::CircleShape()>()
		// No members or functions exposed
	);
	lua.new_usertype<sf::Color>("Color",
		sol::constructors<sf::Color(), sf::Color(sf::Uint8, sf::Uint8, sf::Uint8, sf::Uint8)>(),

		"r", &sf::Color::r,
		"g", &sf::Color::g,
		"b", &sf::Color::b,
		"a", &sf::Color::a,

		// Static predefined colors
		"Black", sol::var(sf::Color::Black),
		"White", sol::var(sf::Color::White),
		"Red", sol::var(sf::Color::Red),
		"Green", sol::var(sf::Color::Green),
		"Blue", sol::var(sf::Color::Blue),
		"Yellow", sol::var(sf::Color::Yellow),
		"Magenta", sol::var(sf::Color::Magenta),
		"Cyan", sol::var(sf::Color::Cyan),
		"Transparent", sol::var(sf::Color::Transparent)
	);
}