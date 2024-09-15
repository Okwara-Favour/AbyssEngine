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
