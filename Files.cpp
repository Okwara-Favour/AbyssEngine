#include "Files.h"

void Files::Init(Editor& editor)
{
	for (const auto& entry : fs::directory_iterator(currentDirectory)) {
		const std::string filename = entry.path().filename().string();
		if (filename == DIR)
		{
			currentDirectory = entry.path();
			break;
		}
	}
}
void Files::Update(Editor& editor)
{
	ImGui::Begin("Explorer", nullptr, ImGuiWindowFlags_NoCollapse);
	ImGui::BeginTabBar("ExplorerTabBar");
	if (ImGui::BeginTabItem("Files")) {
		ImGui::Text("Content of Tab Files");
		ImGui::Text("Content of Directory: %s", currentDirectory.string().c_str());

		// Iterate over directory contents
		OpenDirectory(editor);
		ImGui::EndTabItem();
	}
	ImGui::EndTabBar();
	ImGui::End();
}


void Files::OpenDirectory(Editor& editor)
{
	if (directory != DIR)
	{
		if (ImGui::Selectable("...", "..." == selected, ImGuiSelectableFlags_AllowDoubleClick))
		{
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
			{
				currentDirectory = currentDirectory.parent_path();
				directory = currentDirectory.filename().string();
			}
		}
	}

	for (const auto& entry : fs::directory_iterator(currentDirectory)) {
		const std::string filename = entry.path().filename().string();

		if (ImGui::Selectable(filename.c_str(), filename == selected, ImGuiSelectableFlags_AllowDoubleClick))
		{
			selected = filename;
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
			{
				if (fs::is_directory(entry)) {
					directory = filename;
					currentDirectory = entry;
				}
			}
		}
	}
}
