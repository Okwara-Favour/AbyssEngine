#pragma once
#include <sol/sol.hpp>
#include <unordered_map>
#include <unordered_set>
#include <filesystem>

namespace fs = std::filesystem;

class Editor;
class Entity;
class Scriptable;

class ScriptManager
{
	friend Editor;
	const std::string DIR = "Scripts";
	fs::path currentDirectory;
	std::string readFile(const std::string& path);
	bool addScript(const std::string& name, const std::string& filename);
	std::string removeLuaExt(const std::string& filename);
	std::vector<Scriptable> allSOL;
	std::unordered_map<std::string, std::shared_ptr<sol::environment>> allEnvironment;
	void mapLuaToAny(Scriptable& script);
	void mapAnyToLua(Scriptable& script);
public:
	sol::state lua;
	std::unordered_map<std::string, std::string> scriptsMap;
	std::unordered_map<std::string, fs::path> scriptsPathMap;
	ScriptManager();
	void ProcessScript(const std::string& name, const std::string& filename, std::unordered_set<std::string> processedScript = {});
	void LoadScript(const std::string& filename, const fs::path& directory);
	bool hasScript(const std::string& name);
	bool hasEnvironment(const std::string& name);
	void ExecuteScript(const std::string& name);
	void ResetScript(const std::string& filename, const fs::path& directory);
	void ExecuteEntityScripts(Editor& editor, std::shared_ptr<Entity>& entity);
	void UpdateSOL();
	void Close();
};

