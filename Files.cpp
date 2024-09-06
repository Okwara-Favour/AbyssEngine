#include "Files.h"

void Files::Init(Editor& editor)
{

}
void Files::Update(Editor& editor)
{
	ImGui::Begin("Explorer", nullptr, ImGuiWindowFlags_NoCollapse);
	ImGui::BeginTabBar("ExplorerTabBar");
	if (ImGui::BeginTabItem("Files")) {
		ImGui::Text("Content of Tab Files");
		ImGui::EndTabItem();
	}
	ImGui::EndTabBar();
	ImGui::End();
}
