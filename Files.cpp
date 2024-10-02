#include "Files.h"
#include "Editor.h"
#include <iostream>
#include <fstream>
#include <thread>

void Files::Init(Editor& editor)
{
	editor.currentDirectory /= DIR;
	if (!fs::exists(editor.currentDirectory) || !fs::is_directory(editor.currentDirectory)) {
		editor.HandleError("SEVERE WARNING: 'Assets' directory not found!");
		return;
	}

	LoadScripts(editor);

	auto entity = editor.entityManager.addEntity("STest");
	entity->addComponent<CName>("TestSc1");
	entity->addComponent<CTransform>();
	entity->addComponent<CSize>();
	entity->addComponent<CRectangleShape>();
	entity->addScriptable(Scriptable("Example"));
	editor.scriptManager.ExecuteEntityScripts(editor, entity);

	auto entity2 = editor.entityManager.addEntity("STest");
	entity2->addComponent<CName>("TestSc2");
	entity2->addComponent<CTransform>();
	entity2->addComponent<CSize>();
	entity2->addComponent<CRectangleShape>();
	entity2->addScriptable(Scriptable("Example"));
	editor.scriptManager.ExecuteEntityScripts(editor, entity2);
}
void Files::Update(Editor& editor)
{
	ImGui::Begin("Explorer", nullptr, ImGuiWindowFlags_NoCollapse);
	ImGui::BeginTabBar("ExplorerTabBar");
	if (ImGui::BeginTabItem("Files")) {
		ImGui::Text("Content of Directory: %s", editor.currentDirectory.string().c_str());

		// Iterate over directory contents
		OpenDirectory(editor);
		ImGui::EndTabItem();
	}
	ImGui::EndTabBar();
	ImGui::End();
	MakeAnimation(editor);
}


void Files::OpenDirectory(Editor& editor)
{
	if (directory != DIR)
	{
		if (ImGui::Selectable("..", ".." == selected, ImGuiSelectableFlags_AllowDoubleClick))
		{
			selected = "";
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
			{
				editor.currentDirectory = editor.currentDirectory.parent_path();
				directory = editor.currentDirectory.filename().string();
			}
		}
	}

	for (const auto& entry : fs::directory_iterator(editor.currentDirectory)) {
		const std::string filename = entry.path().filename().string();

		if (fs::is_directory(entry)) {
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.6f, 1.0f, 1.0f)); // Blue color for directories
		}

		if (ImGui::Selectable(filename.c_str(), filename == selected, ImGuiSelectableFlags_AllowDoubleClick))
		{
			selected = filename;
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
			{
				if (fs::is_directory(entry)) {
					directory = filename;
					editor.currentDirectory = entry;
				}
			}

			if (fs::is_regular_file(entry))
			{
				auto extension = entry.path().extension().string();
				if (extension == ".png" || extension == ".PNG")
				{
					if (!texture.loadFromFile(entry.path().string()))
					{
						
					}
				}
			}
		}

		if (fs::is_directory(entry)) {
			ImGui::PopStyleColor();
		}

		if (selected == filename)
		{
			if (fs::is_regular_file(entry))
			{
				auto extension = entry.path().extension().string();
				if (extension == ".png" || extension == ".PNG") {
					OpenImage(entry);
				}
				else if (extension == ".txt" || extension == ".h" || extension == ".hpp" || extension == ".cpp") {
					OpenFile(entry);
				}
				else 
				{
					
				}
			}
		}
	}
}

void Files::OpenFile(const fs::path& path)
{
	std::ifstream file(path);

	ImGui::Begin("File View");
	if (!file.is_open()) {
		ImGui::Text("Unable to open file.");
		ImGui::End();
		return;
	}
	if (ImGui::Button("Open"))
	{
		RunDesiredApplication(path);
	}

	if (fs::file_size(path) > maxSizeBytes) {
		ImGui::Text("File is too large to display.");
		ImGui::End();
		return;
	}
	std::string line;
	while (std::getline(file, line)) {
		ImGui::TextWrapped("%s", line.c_str());
	}
	ImGui::End();
}

void Files::OpenImage(const fs::path& path)
{
	ImGui::Begin("Image View");

	if (ImGui::Button("Open"))
	{
		RunDesiredApplication(path);
	}
	ImGui::SameLine();
	if (ImGui::Button("Make Animation"))
	{
		texturePathForAnimation = path;
		popUp = true;
	}

	auto texSize = texture.getSize();
	std::string textureSize = std::to_string(texSize.x) + " X " + std::to_string(texSize.y);
	ImGui::Text((path.filename().string() + " " + textureSize).c_str());
	ImVec2 contentMin = ImGui::GetWindowContentRegionMin();
	ImVec2 contentMax = ImGui::GetWindowContentRegionMax();
	ImVec2 contentSize = ImVec2(contentMax.x - contentMin.x, contentMax.y - contentMin.y);

	ImGui::Image((void*)(intptr_t)texture.getNativeHandle(),
		ImVec2(contentSize.x, contentSize.y));

	ImGui::End();
}

void Files::runCommand(const std::string& command) {
	std::system(command.c_str());
}

void Files::RunDesiredApplication(const fs::path& path)
{
#ifdef _WIN32
		std::string command = "start \"\" \"" + path.string() + "\"";
#elif __APPLE__
		std::string command = "open \"" + path.string() + "\""; // opens in default application on macOS
#elif __linux__
		std::string command = "xdg-open \"" + path.string() + "\""; // opens in default application on Linux
#endif
		std::thread commandThread(runCommand, command);
		commandThread.detach();
}

void Files::MakeAnimation(Editor& editor)
{
	if (!popUp) return;
	ImGui::Begin("AnimationSettings", nullptr, ImGuiWindowFlags_NoDocking);
	static char animationName[128] = "";
	static int frameCount = 1;
	static int speed = 0;

	ImGui::InputText("Animation Name", animationName, sizeof(animationName));
	ImGui::InputInt("Frame Count", &frameCount);
	ImGui::InputInt("Frame Duration", &speed);

	if (ImGui::Button("Save")) {
		sf::Texture texture;
		texture.loadFromFile(texturePathForAnimation.string());
		if (editor.textureMap.find(texturePathForAnimation.filename().string()) == editor.textureMap.end())
		{
			editor.textureMap[texturePathForAnimation.filename().string()] = texture;
			editor.texturePathMap[texturePathForAnimation.filename().string()] = texturePathForAnimation.string();
		}
		Animation anim(std::string(animationName), texturePathForAnimation.filename().string(), editor.textureMap[texturePathForAnimation.filename().string()], frameCount, speed);
		editor.addAnimation(anim);
		popUp = false;
	}
	ImGui::SameLine();
	if (ImGui::Button("Cancel")) {
		popUp = false;
	}
	ImGui::End();
}

void Files::LoadScripts(Editor& editor)
{
	editor.scriptManager.lua.new_usertype<Editor>("Editor",
		"HandleError", &Editor::HandleError, // Bind the HandleError method
		"entityManager", &Editor::entityManager
	);

	// Bind the editor instance to Lua
	editor.scriptManager.lua["editor"] = &editor; // Set 'editor' as a global in Lua
	// Create the command that calls HandleError
	editor.scriptManager.LoadScript("Example.lua", editor.currentDirectory);
		// Execute the Lua command
}