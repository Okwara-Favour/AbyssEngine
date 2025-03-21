#pragma once
#include <sol/sol.hpp>
#include <unordered_map>
#include <unordered_set>
#include <filesystem>
#include "Scriptable.hpp"

namespace fs = std::filesystem;

class Editor;
class Entity;
class Inspector;

class ScriptManager
{
	friend Editor;
	friend Inspector;
	const std::string DIR = "Scripts";
	fs::path currentDirectory;
	std::string readFile(const std::string& path);
	bool addScript(const std::string& name, const std::string& filename);
	std::map<size_t, std::vector<Scriptable>> allSOL;
	std::unordered_map<size_t, std::unordered_map<std::string, sol::environment>> allEntityEnvironment;
	std::unordered_map<std::string, sol::function> allCompiledScripts;
	void mapEnvironmentVariables(std::unordered_map<std::string, std::any>& toPush, sol::environment& env, const std::string& envName);
	void pushEnvironmentVariables(Scriptable& script);
	void popEnvironmentVariables(Scriptable& script);
public:
	sol::state lua;
	std::vector<std::string> environmentNames;
	std::unordered_map<std::string, std::string> scriptsMap;
	std::unordered_map<std::string, fs::path> scriptsDirectoryMap;
	ScriptManager();
	std::string removeLuaExt(const std::string& filename);
	void ProcessScript(const std::string& name, const std::string& filename, std::unordered_set<std::string> processedScript = {});
	void LoadScript(Editor& editor, const std::string& filename, const fs::path& directory);
	bool hasScript(const std::string& name);
	bool hasCompiledScript(const std::string& name);
	void ExecuteScript(const std::string& name);
	void ResetScript(Editor& editor, const std::string& filename, const fs::path& directory);
	void CompileEntityEnvironment(Editor& editor, std::shared_ptr<Entity>& entity);
	void ExecuteEntityScripts(Editor& editor, std::shared_ptr<Entity>& entity);
	void UpdateSOL();
	void ResolveMissingSharedSOL(Editor& editor);
	void ResetClass();
	void Close();
};

