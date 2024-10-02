#include "ScriptManager.h"
#include <fstream>
#include <sstream>
#include <regex>
#include "Editor.h"

ScriptManager::ScriptManager()
{
	lua.open_libraries(sol::lib::base, sol::lib::package);

	lua["require"] = [this](const std::string& name) {
		auto it = scriptsMap.find(name); // Adjust if needed
		if (it != scriptsMap.end()) {
			sol::load_result loadResult = lua.load(it->second);
			if (!loadResult.valid()) {
				throw std::runtime_error("Error loading module: " + name);
			}
			// Execute the loaded script and return its result (should be a table)
			return loadResult();
		}
		throw std::runtime_error("Module not found: " + name);
		};
}

std::string ScriptManager::readFile(const std::string& path) {
	std::ifstream file(path);
	if (!file.is_open()) {
		throw std::runtime_error("Could not open file: " + path);
	}
	return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

bool ScriptManager::addScript(const std::string& name, const std::string& filename)
{
	fs::path path = currentDirectory / filename;

	if (fs::is_regular_file(path) && path.extension().string() == ".lua")
	{
		scriptsMap[name] = readFile(path.string());
		scriptsPathMap[name] = path;
		return true;
	}
	return false;
}

void ScriptManager::ProcessScript(const std::string& name, const std::string& filename, std::unordered_set<std::string> processedScript)
{
	if (processedScript.find(name) != processedScript.end())
	{
		return;
	}
	processedScript.insert(name);

	//std::cout << "processing" << std::endl;

	if (addScript(name, filename))
	{
		std::regex requireRegex(R"(require\s*\(\s*["']([^"']+)["']\s*\))");
		std::smatch match;
		std::stringstream ss(scriptsMap[name]); // Create a stringstream from the script content

		std::string line;
		while (std::getline(ss, line)) // Read line by line
		{
			// Check for require matches in the current line
			if (std::regex_search(line, match, requireRegex)) {
				std::string moduleName = removeLuaExt(match[1].str()); // Extract the module name
				ProcessScript(moduleName, (moduleName + ".lua"), processedScript); // Recursively process the required script
			}
		}
	}
}

void ScriptManager::LoadScript(const std::string& filename, const fs::path& directory)
{
	currentDirectory = directory;
	currentDirectory /= DIR;
	if (!fs::exists(currentDirectory) || !fs::is_directory(currentDirectory)) {
		throw std::runtime_error("SEVERE WARNING: 'Scripts' directory not found!");
	}
	
	std::string name = removeLuaExt(filename);

	ProcessScript(name, filename);

	if (hasScript(name))
	{
		sol::environment tempEnv(lua, sol::create, lua.globals());
		allEnvironment[name] = std::make_shared<sol::environment>(tempEnv);
		auto result = lua.safe_script(scriptsMap[name], *allEnvironment[name]);
		if (!result.valid()) {
			sol::error err = result; // Capture the error
			std::cout << "Error during script processing" << std::endl;
		}
	}
}

bool ScriptManager::hasScript(const std::string& name)
{
	return scriptsPathMap.find(name) != scriptsPathMap.end();
}

std::string ScriptManager::removeLuaExt(const std::string& filename)
{
	std::size_t dotPos = filename.rfind('.');

	// Check if it's ".lua" and the dot exists
	if (dotPos != std::string::npos) {
		if (filename.substr(dotPos) == ".lua") return filename.substr(0, dotPos);
		else throw std::runtime_error("Unknown extension detected");
	}
	return filename;
}

void ScriptManager::ExecuteScript(const std::string& name)
{
	if (!hasScript(name)) return;

	sol::environment temp_env(lua, sol::create, lua.globals());

	auto result = lua.safe_script(scriptsMap[name], temp_env);
	if (!result.valid()) {
		// An error occurred; handle it
		sol::error err = result; // Capture the error
		std::cout << err.what() << std::endl; // Pass the error message to the editor's HandleError
	}
}

void ScriptManager::ResetScript(const std::string& filename, const fs::path& directory)
{
	std::string name = removeLuaExt(filename);

	if (!hasScript(name)) return;

	auto envIter = allEnvironment.find(name);
	if (envIter != allEnvironment.end()) {
		for (auto& s : allSOL)
		{
			if (s.name == name)
			{
				s.instantiated = std::make_shared<bool>(false);
				s.scriptClass.reset();
				s.instance.reset();
			}
		}
		auto& env = *envIter->second;
		env.reset();

		lua.collect_garbage();
	}
	LoadScript(filename, directory);
}

void ScriptManager::ExecuteEntityScripts(Editor& editor, std::shared_ptr<Entity>& entity)
{
	for (auto& s : entity->m_scriptables)
	{	
		if (!(*s.second.instantiated) ||
			!(*s.second.scriptClass)
			)
		{
			editor.HandleError(entity->getComponent<CName>().name + " " + s.second.name + " " + std::to_string((*s.second.destroy)));
			auto envIter = allEnvironment.find(s.second.name);
			if (envIter == allEnvironment.end()) {
				std::cout << "Warning: script environment not available for " << s.second.name << std::endl;
				continue;
			}

			// Access environment
			auto& env = *envIter->second;
			sol::table tempScriptClass = env[s.second.name];
			if (!tempScriptClass.valid()) {
				editor.HandleError("Error: script class not found for " + s.second.name);
				continue;
			}
			s.second.scriptClass = std::make_shared<sol::table>(tempScriptClass);
			sol::function startFunc = (*s.second.scriptClass)["Start"];
			if (!startFunc.valid()) {
				continue;
			}
			try
			{
				sol::table Start = startFunc(*s.second.scriptClass, entity);
				s.second.instance = std::make_shared<sol::table>(Start);
				s.second.instantiated = std::make_shared<bool>(true);

				Scriptable sc;
				sc.name = s.second.name;
				sc.instantiated = s.second.instantiated;
				sc.scriptClass = s.second.scriptClass;
				sc.instance = s.second.instance;
				sc.destroy = s.second.destroy;
				allSOL.push_back(sc);
			}
			catch (const sol::error& e) {
				editor.HandleError(std::string(e.what()));
				continue;
			}
		}

		if (!editor.gameMode) continue;

		sol::function updateFunc = (*s.second.scriptClass)["Update"];
		if (updateFunc.valid()) {
			try {
				sol::protected_function Update = updateFunc(*s.second.instance, entity);
			}
			catch (const sol::error& e) {
				editor.HandleError("Lua error in Update: " + std::string(e.what()));
			}
		}
	}
}

void ScriptManager::UpdateSOL()
{
	for (auto it = allSOL.begin(); it != allSOL.end(); )
	{
		//std::cout << (it->destroy) << std::endl;
		if (it->destroy && (*it->destroy))
		{
			(*it->destroy) = false;
			std::cout << "Removing sol reference" << it->name << std::endl;
			(*it->scriptClass).reset();
			(*it->instance).reset();
			it = allSOL.erase(it);
			lua.collect_garbage();
		}
		else
		{
			++it; // Move to the next entry
		}
	}
}

void ScriptManager::Close()
{
	for (auto& s : allSOL)
	{
		(*s.scriptClass).reset();
		(*s.instance).reset();
	}
	for (auto& e : allEnvironment)
	{
		(*e.second).reset();
	}
	allSOL.clear();
	allEnvironment.clear();
	lua.collect_garbage();
}