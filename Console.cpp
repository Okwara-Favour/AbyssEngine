#include "Console.h"

void Console::Init(Editor& editor)
{

}
void Console::Update(Editor& editor)
{
	ImGui::Begin("Explorer", nullptr, ImGuiWindowFlags_NoCollapse);
	ImGui::BeginTabBar("ExplorerTabBar");
	if (ImGui::BeginTabItem("Console")) {
		if (ImGui::Button("Clear"))
		{
			errorStack.clear();
		}
		if (errorStack.size() > maxError) errorStack.pop_back();
		ImGui::SameLine();
		if(!errorStack.empty()) ImGui::Text("Lines: %d", errorStack.size());
		for (auto& errorString : errorStack)
		{
			ImGui::TextWrapped(errorString.c_str());
		}
		ImGui::EndTabItem();
	}
	ImGui::EndTabBar();
	ImGui::End();
}

void Console::ConsoleText(const std::string& errorString)
{
	errorStack.push_back(errorString);
}
