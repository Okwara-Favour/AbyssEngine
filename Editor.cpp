#include "Editor.h"

Editor::Editor() {}
Editor::Editor(sf::RenderWindow& w) :window(&w)
{
	ImGui::SFML::Init(*window);
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable docking
	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	engineTabs.push_back(std::make_unique<EngineSettings>());
	engineTabs.push_back(std::make_unique<Inspector>());
	engineTabs.push_back(std::make_unique<Files>());
	engineTabs.push_back(std::make_unique<Console>());
	engineTabs.push_back(std::make_unique<Hierarchy>());
	engineTabs.push_back(std::make_unique<Display>());
	Init();
	command.Save(entityManager);
}
void Editor::Init()
{
	for (auto& tab : engineTabs)
	{
		tab->Init(*this);
	}
}
void Editor::ProcessEvent(sf::Event& event) { ImGui::SFML::ProcessEvent(event); }
void Editor::Render() { ImGui::SFML::Render(*window); }
void Editor::Close() { ImGui::SFML::Shutdown(); }
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
	ImGui::SFML::Update(*window, deltaClock.restart());
	MainPage();
	command.Execute(entityManager);
	entityManager.update();
	for (auto& tab : engineTabs)
	{
		tab->Update(*this);
	}
}