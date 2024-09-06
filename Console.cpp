#include "Console.h"

void Console::Init(Editor& editor)
{

}
void Console::Update(Editor& editor)
{
	ImGui::Begin("Explorer", nullptr, ImGuiWindowFlags_NoCollapse);
	ImGui::BeginTabBar("ExplorerTabBar");
	if (ImGui::BeginTabItem("Console")) {
		ImGui::Text("Content of Tab Console");
		ImGui::EndTabItem();
	}
	ImGui::EndTabBar();
	ImGui::End();
}