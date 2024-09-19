#pragma once
#include "AbstractEngineTab.hpp"

namespace fs = std::experimental::filesystem;
class Files : public AbstractEngineTab
{
	sf::Texture texture;
	const std::size_t maxSizeBytes = 5 * 1024 * 1024;
	const std::string DIR = "Assets";
	std::string directory = "Assets";
	std::string selected = "";
	fs::path texturePathForAnimation;
	bool popUp = false;
	fs::path currentFile;
	void MakeAnimation(Editor& editor);
	void OpenDirectory(Editor& editor);
	void OpenFile(const fs::path& path);
	void OpenImage(const fs::path& path);
	void RunDesiredApplication(const fs::path& path);
	static void runCommand(const std::string& command);
public:
	void Init(Editor& editor) override;
	void Update(Editor& editor) override;
};

