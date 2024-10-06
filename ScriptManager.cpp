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
		scriptsDirectoryMap[name] = currentDirectory;
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

void ScriptManager::LoadScript(Editor& editor, const std::string& filename, const fs::path& directory)
{
	currentDirectory = directory;

	std::string name = removeLuaExt(filename);

	ProcessScript(name, filename);

	if (hasScript(name))
	{
		if (std::find(environmentNames.begin(), environmentNames.end(), name) == environmentNames.end())
		{
			environmentNames.push_back(name);
		}
		sol::environment tempEnv(lua, sol::create, lua.globals());
		allEnvironment[name] = std::make_shared<sol::environment>(tempEnv);
		auto result = lua.safe_script(scriptsMap[name], *allEnvironment[name]);
		if (!result.valid()) {
			sol::error err = result; // Capture the error
			editor.ConsoleText("Script: " + name + " environment construction failed");
		}
		else
		{
			editor.ConsoleText("Script: " + name + " environment construction succeeded");
		}
	}
}

bool ScriptManager::hasScript(const std::string& name)
{
	return scriptsDirectoryMap.find(name) != scriptsDirectoryMap.end();
}

bool ScriptManager::hasEnvironment(const std::string& name)
{
	return allEnvironment.find(name) != allEnvironment.end();
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
		std::cout << err.what() << std::endl; // Pass the error message to the editor's ConsoleText
	}
}

void ScriptManager::ResetScript(Editor& editor, const std::string& filename, const fs::path& directory)
{
	std::string name = removeLuaExt(filename);

	if (!hasScript(name)) 
	{
		editor.ConsoleText("Script: " + name + " not found");
		return;
	}
	else if (!hasEnvironment(name))
	{
		editor.ConsoleText("Environment: " + name + " not found, Script is a sub script");
		return;
	}
	auto envIter = allEnvironment.find(name);
	if (envIter != allEnvironment.end()) {
		for (auto& s : allSOL)
		{
			if (s.name == name)
			{
				s.instantiated = std::make_shared<bool>(false);
				(*s.scriptClass).reset();
				(*s.instance).reset();
			}
		}
		(*envIter->second).reset();
		allEnvironment.erase(name);
		lua.collect_garbage();
	}


	LoadScript(editor, filename, directory);
}

void ScriptManager::ExecuteEntityScripts(Editor& editor, std::shared_ptr<Entity>& entity)
{
	for (auto& s : entity->m_scriptables)
	{	
		if (!(*s.second.instantiated) ||
			!(*s.second.scriptClass)
			)
		{
			//editor.ConsoleText(entity->getComponent<CName>().name + " " + s.second.name + " " + std::to_string((*s.second.destroy)));
			auto envIter = allEnvironment.find(s.second.name);
			if (envIter == allEnvironment.end()) {
				std::cout << "Warning: script environment not available for " << s.second.name << std::endl;
				continue;
			}

			// Access environment
			auto& env = *envIter->second;
			sol::table tempScriptClass = env[s.second.name];
			if (!tempScriptClass.valid()) {
				editor.ConsoleText("Error: script class not found for " + s.second.name);
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
				mapLuaToAny(Start, s.second);
				Scriptable sc;
				sc.name = s.second.name;
				sc.instantiated = s.second.instantiated;
				sc.scriptClass = s.second.scriptClass;
				sc.instance = s.second.instance;
				sc.destroy = s.second.destroy;
				allSOL.push_back(sc);
			}
			catch (const sol::error& e) {
				editor.ConsoleText(std::string(e.what()));
				continue;
			}
		}

		if (!editor.gameMode) continue;

		mapAnyToLua(editor, s.second);
		sol::function updateFunc = (*s.second.scriptClass)["Update"];
		if (updateFunc.valid()) {
			try {
				sol::table Update = updateFunc(*s.second.instance, entity);
				mapLuaToAny(Update, s.second);
				(*s.second.instance) = Update;
			}
			catch (const sol::error& e) {
				editor.ConsoleText("Lua error in Update: " + std::string(e.what()));
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

	scriptsMap.clear();
	scriptsDirectoryMap.clear();
	environmentNames.clear();
	currentDirectory.clear();
}

void ScriptManager::mapLuaToAny(const sol::table& tab, Scriptable& script)
{
	for (const auto& pair : tab)
	{
		std::string varName = pair.first.as<std::string>();
		auto& varValue = pair.second;

		//std::cout << varName << std::endl;
		if (varValue.get_type() == sol::type::number)
		{
			if (varValue.is<int>()) {
				script.variableMap[varName] = std::make_any<int>(varValue.as<int>());
			}
			else if (varValue.is<unsigned int>()) {
				script.variableMap[varName] = std::make_any<unsigned int>(varValue.as<unsigned int>());
			}
			else if (varValue.is<float>()) {
				script.variableMap[varName] = std::make_any<float>(varValue.as<float>());
			}
			else if (varValue.is<double>()) {
				script.variableMap[varName] = std::make_any<double>(varValue.as<double>());
			}
			else if (varValue.is<long>()) {
				script.variableMap[varName] = std::make_any<long>(varValue.as<long>());
			}
			else if (varValue.is<unsigned long>()) {
				script.variableMap[varName] = std::make_any<unsigned long>(varValue.as<unsigned long>());
			}
			else if (varValue.is<long long>()) {
				script.variableMap[varName] = std::make_any<long long>(varValue.as<long long>());
			}
			else if (varValue.is<unsigned long long>()) {
				script.variableMap[varName] = std::make_any<unsigned long long>(varValue.as<unsigned long long>());
			}
		}
		else if (varValue.get_type() == sol::type::string) {
			script.variableMap[varName] = std::make_any<std::string>(varValue.as<std::string>());
		}
		else if (varValue.is<std::shared_ptr<Entity>>())
		{
			script.variableMap[varName] = std::make_any<std::shared_ptr<Entity>>(varValue.as<std::shared_ptr<Entity>>());
		}
		else if (varValue.is<Vec2>())
		{
			script.variableMap[varName] = std::make_any<Vec2>(varValue.as<Vec2>());
		}
	}
}


void ScriptManager::mapAnyToLua(Editor& editor, Scriptable& script)
{
	for (const auto& pair : script.variableMap)
	{
		const std::string& varName = pair.first;
		const std::any& varValue = pair.second;
		if (varValue.type() == typeid(int))
		{
			(*script.instance)[varName] = std::any_cast<int>(varValue);
		}
		else if (varValue.type() == typeid(unsigned int))
		{
			(*script.instance)[varName] = std::any_cast<unsigned int>(varValue);
		}
		else if (varValue.type() == typeid(float))
		{
			(*script.instance)[varName] = std::any_cast<float>(varValue);
		}
		else if (varValue.type() == typeid(double))
		{
			(*script.instance)[varName] = std::any_cast<double>(varValue);
		}
		else if (varValue.type() == typeid(long))
		{
			(*script.instance)[varName] = std::any_cast<long>(varValue);
		}
		else if (varValue.type() == typeid(unsigned long))
		{
			(*script.instance)[varName] = std::any_cast<unsigned long>(varValue);
		}
		else if (varValue.type() == typeid(long long))
		{
			(*script.instance)[varName] = std::any_cast<long long>(varValue);
		}
		else if (varValue.type() == typeid(unsigned long long))
		{
			(*script.instance)[varName] = std::any_cast<unsigned long long>(varValue);
		}
		else if (varValue.type() == typeid(std::string))
		{
			(*script.instance)[varName] = std::any_cast<std::string>(varValue);
		}
		else if (varValue.type() == typeid(std::shared_ptr<Entity>))
		{
			auto entityPtr = std::any_cast<std::shared_ptr<Entity>>(varValue);
			if (entityPtr) {
				(*script.instance)[varName] = editor.entityManager.getEntity(entityPtr->id());
			}
		}
		else if (varValue.type() == typeid(Vec2))
		{
			(*script.instance)[varName] = std::any_cast<Vec2>(varValue);
		}
	}
}