#include "Files.h"
#include "Editor.h"
#include <iostream>
#include <fstream>
#include <thread>

void Files::Init(Editor& editor)
{
	editor.currentDirectory /= DIR;
	if (!fs::exists(editor.currentDirectory) || !fs::is_directory(editor.currentDirectory)) {
		editor.ConsoleText("SEVERE WARNING: 'Assets' directory not found!");
		return;
	}

	//LoadScripts(editor);

	auto entity = editor.entityManager.addEntity("MainCamera");
	entity->addComponent<CName>("Camera");
	Vec2 CamScale(editor.WinSize().x / 1280.0f, editor.WinSize().y / 720.0f);
	entity->addComponent<CTransform>(Vec2(editor.WinSize().x/2, editor.WinSize().y/2), CamScale);
	entity->addComponent<CSize>();
	entity->addComponent<CCamera>();
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
				else if (extension == ".lua" || extension == ".txt" || extension == ".h" || extension == ".hpp" || extension == ".cpp") {
					OpenFile(editor, entry);
				}
				else 
				{
					
				}
			}
		}
	}
}

void Files::OpenFile(Editor& editor, const fs::path& path)
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
	if (path.extension().string() == ".lua" && ImGui::Button("Add Script"))
	{
		LoadScripts(editor, path.filename().string(), editor.currentDirectory);
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

void Files::LoadScripts(Editor& editor, const std::string& filename, const fs::path& directory)
{
	if (editor.scriptManager.hasScript(editor.scriptManager.removeLuaExt(filename)))
	{
		editor.ConsoleText("Script: " + filename + " already loaded and processed");
		editor.ConsoleText("Use scripts settings for changes");
		return;
	}
	editor.scriptManager.LoadScript(editor, filename, directory);
}