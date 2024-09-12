#pragma once
#include "AbstractEngineTab.hpp"
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;
class Files : public AbstractEngineTab
{
	const std::string DIR = "Assets";
	std::string directory = "Assets";
	std::string selected = "";
	fs::path currentDirectory = fs::current_path();
	void OpenDirectory(Editor& editor);
public:
	void Init(Editor& editor) override;
	void Update(Editor& editor) override;
};

