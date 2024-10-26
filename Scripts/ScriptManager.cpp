#include "ScriptManager.h"
#include <fstream>
#include <sstream>
#include <regex>
#include "../Editor.h"

ScriptManager::ScriptManager()
{
	lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::coroutine,
		sol::lib::string, sol::lib::table, sol::lib::math,
		sol::lib::io, sol::lib::os, sol::lib::debug, sol::lib::utf8);

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
		allEnvironment[name] = tempEnv;
		auto result = lua.safe_script(scriptsMap[name], allEnvironment[name]);
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
			auto it = std::find_if(s.second.begin(), s.second.end(), [&name](const Scriptable& sc) {
				return sc.name == name;
				});
			if (it != s.second.end())
			{
				auto& value = *it;
				value.instantiated = std::make_shared<bool>(false);
				(*value.hasMapped) = false;
				if(value.scriptClass) value.scriptClass.reset();
				if(value.instance) value.instance.reset();
				auto entity = editor.entityManager.safeGetEntity(s.first);
				if(entity) entity->m_scriptables[name].variableMap.clear();
			}
		}
		(envIter->second).reset();
		allEnvironment.erase(name);
		lua.collect_garbage();
	}


	LoadScript(editor, filename, directory);
}

void ScriptManager::CompileEntityEnvironment(Editor& editor, std::shared_ptr<Entity>& entity)
{
	for (auto& s : entity->m_scriptables)
	{
		if (!(*s.second.hasMapped))
		{
			auto envIter = allEnvironment.find(s.second.name);
			if (envIter == allEnvironment.end()) {
				std::cout << "Warning: script environment not available for " << s.second.name << std::endl;
				continue;
			}
			auto& env = envIter->second;
			//std::cout << "Mapping" << std::endl;
			mapEnvironmentVariables(s.second.variableMap, env);
			(*s.second.hasMapped) = true;
			Scriptable sc;
			sc.name = s.second.name;
			sc.instantiated = s.second.instantiated;
			sc.scriptClass = s.second.scriptClass;
			sc.instance = s.second.instance;
			sc.destroy = s.second.destroy;
			sc.hasMapped = s.second.hasMapped;
			allSOL[entity->id()].push_back(sc);
		}
	}
}

void ScriptManager::ExecuteEntityScripts(Editor& editor, std::shared_ptr<Entity>& entity)
{
	if (!editor.gameMode) return;

	for (auto& s : entity->m_scriptables)
	{	
		if (!(*s.second.instantiated) ||
			!(s.second.scriptClass)
			)
		{
			if(s.second.scriptClass) (s.second.scriptClass).reset();
			if(s.second.instance) (s.second.instance).reset();
			//editor.ConsoleText(entity->getComponent<CName>().name + " " + s.second.name + " " + std::to_string((*s.second.destroy)));
			auto envIter = allEnvironment.find(s.second.name);
			if (envIter == allEnvironment.end()) {
				std::cout << "Warning: script environment not available for " << s.second.name << std::endl;
				continue;
			}
			// Access environment
			auto& env = envIter->second;
			std::unordered_map<std::string, std::any> variableMap;
			std::unordered_map<std::string, std::any> tempVariableMap;
			pushEnvironmentVariables(s.second.variableMap, tempVariableMap, env);

			sol::table tempScriptClass = env[s.second.name];

			if (!tempScriptClass.valid()) {
				editor.ConsoleText("Error: script class not found for " + s.second.name);
				popEnvironmentVariables(tempVariableMap, env);
				continue;
			}
			s.second.scriptClass = tempScriptClass;
			sol::function startFunc = (s.second.scriptClass)["Start"];
			if (!startFunc.valid()) {
				popEnvironmentVariables(tempVariableMap, env);
				continue;
			}
			try
			{
				(s.second.scriptClass)["entity"] = entity;
				sol::table Start = startFunc(s.second.scriptClass);
				s.second.instance = Start;
				s.second.instantiated = std::make_shared<bool>(true);
				//mapLuaToAny(Start, s.second);
				std::string name = s.second.name;
				if (allSOL.find(entity->id()) == allSOL.end())
				{
					Scriptable value;
					value.name = s.second.name;
					value.instantiated = s.second.instantiated;
					value.scriptClass = s.second.scriptClass;
					value.instance = s.second.instance;
					value.destroy = s.second.destroy;
					value.hasMapped = s.second.hasMapped;
					allSOL[entity->id()].push_back(value);
				}
				else
				{
					auto it = std::find_if(allSOL[entity->id()].begin(), allSOL[entity->id()].end(), [&name](const Scriptable& sc) {
						return sc.name == name;
						});

					if (it != allSOL[entity->id()].end())
					{
						auto& value = *it;
						value.name = s.second.name;
						value.instantiated = s.second.instantiated;
						value.scriptClass = s.second.scriptClass;
						value.instance = s.second.instance;
						value.destroy = s.second.destroy;
						value.hasMapped = s.second.hasMapped;
					}
				}
			}
			catch (const sol::error& e) {
				editor.ConsoleText(std::string(e.what()));
				popEnvironmentVariables(tempVariableMap, env);
				continue;
			}
			popEnvironmentVariables(tempVariableMap, env);
		}

		//mapAnyToLua(editor, s.second);
		std::unordered_map<std::string, std::any> tempVariableMap;
		pushEnvironmentVariables(s.second.variableMap, tempVariableMap, (allEnvironment[s.second.name]));
		sol::function updateFunc = (s.second.scriptClass)["Update"];
		if (updateFunc.valid()) {
			try {
				(s.second.instance)["entity"] = entity;
				sol::table Update = updateFunc(s.second.instance);
				//mapLuaToAny(Update, s.second);
				(s.second.instance) = Update;
			}
			catch (const sol::error& e) {
				editor.ConsoleText("Lua error in Update: " + std::string(e.what()));
			}
		}
		editor.physicsManager.OnLuaCollisionEnter(editor, s.second, entity);
		editor.physicsManager.OnLuaCollisionExit(editor, s.second, entity);
		editor.physicsManager.OnLuaCollision(editor, s.second, entity);

		popEnvironmentVariables(tempVariableMap, (allEnvironment[s.second.name]));
	}
}

void ScriptManager::UpdateSOL()
{
	std::vector<size_t> toRemove = {};
	for (auto& s : allSOL)
	{
		for (auto iter = s.second.begin(); iter != s.second.end(); )
		{
			if (iter->destroy && (*iter->destroy))
			{
				(*iter->destroy) = false;
				//std::cout << "Removing sol reference" << iter->name << std::endl;
				if(iter->scriptClass) (iter->scriptClass).reset();
				if(iter->instance) (iter->instance).reset();
				iter = s.second.erase(iter);
			}
			else
			{
				++iter; // Move to the next entry
			}
		}

		if (s.second.empty())
		{
			toRemove.push_back(s.first);
			lua.collect_garbage();
		}
	}
	for (auto& t : toRemove)
	{
		allSOL.erase(t);
	}
}

void ScriptManager::ResetClass()
{
	for (auto& s : allSOL)
	{
		for (auto& sc : s.second)
		{
			if (sc.scriptClass) (sc.scriptClass).reset();
			if (sc.instance) (sc.instance).reset();
		}
	}
	allSOL.clear();
	lua.collect_garbage();
}

void ScriptManager::Close()
{
	for (auto& s : allSOL)
	{
		for (auto& sc : s.second)
		{
			if(sc.scriptClass) (sc.scriptClass).reset();
			if(sc.instance) (sc.instance).reset();
		}
	}
	for (auto& e : allEnvironment)
	{
		if(e.second) e.second.reset();
	}
	allSOL.clear();
	allEnvironment.clear();

	scriptsMap.clear();
	scriptsDirectoryMap.clear();
	environmentNames.clear();
	currentDirectory.clear();

	lua.collect_garbage();
	lua["collectgarbage"]("collect");
}

void ScriptManager::ResolveMissingSharedSOL(Editor& editor)
{
	for (auto& entity : editor.entityManager.getEntities())
	{
		for (auto& s : entity->m_scriptables)
		{
			auto iter = allSOL.find(entity->id());
			if (iter != allSOL.end())
			{
				auto& value = *iter;
				std::string& name = s.second.name;
				auto it = std::find_if(value.second.begin(), value .second.end(), [&name](const Scriptable& sc) {
					return sc.name == name;
					});
				if (it == value.second.end())
				{
					Scriptable sc;
					sc.name = s.second.name;
					sc.instantiated = s.second.instantiated;
					sc.scriptClass = s.second.scriptClass;
					sc.instance = s.second.instance;
					s.second.destroy = std::make_shared<bool>(false);
					sc.destroy = s.second.destroy;
					sc.hasMapped = s.second.hasMapped;
					allSOL[entity->id()].push_back(sc);
				}
			}
			else if (iter == allSOL.end())
			{
				Scriptable sc;
				sc.name = s.second.name;
				sc.instantiated = s.second.instantiated;
				sc.scriptClass = s.second.scriptClass;
				sc.instance = s.second.instance;
				s.second.destroy = std::make_shared<bool>(false);
				sc.destroy = s.second.destroy;
				sc.hasMapped = s.second.hasMapped;
				allSOL[entity->id()].push_back(sc);
			}
		}
	}
}

void ScriptManager::mapEnvironmentVariables(std::unordered_map<std::string, std::any>& toPush,	sol::environment& env)
{
	for (const auto& pair : env)
	{
		std::string varName = pair.first.as<std::string>();
		sol::object varValue = pair.second;
		//std::cout << "Mapping" << " " << varName << std::endl;
		// Store the original value in toRestore before overwriting
		if (varValue.valid())
		{
			if (varValue.is<int>()) {
				toPush[varName] = varValue.as<int>();
			}
			else if (varValue.is<unsigned int>()) {
				toPush[varName] = varValue.as<unsigned int>();
			}
			else if (varValue.is<float>()) {
				toPush[varName] = varValue.as<float>();
			}
			else if (varValue.is<double>()) {
				toPush[varName] = varValue.as<double>();
			}
			else if (varValue.is<std::string>()) {
				toPush[varName] = varValue.as<std::string>();
			}
			else if (varValue.is<std::shared_ptr<Entity>>()) {
				toPush[varName] = varValue.as<std::shared_ptr<Entity>>();
			}
			else if (varValue.is<Vec2>()) {
				toPush[varName] = varValue.as<Vec2>();
			}
		}
	}
}

void ScriptManager::pushEnvironmentVariables(std::unordered_map<std::string, std::any>& toPush,
	std::unordered_map<std::string, std::any>& toRestore,
	sol::environment& env)
{
	for (const auto& pair : toPush)
	{
		const std::string& varName = pair.first;
		const std::any& varValue = pair.second;
		// Save the original value from the Lua environment to restore later
		sol::object originalValue = env[varName];
		if (originalValue.valid())
		{
			// Store the original Lua variable in toRestore before overwriting
			if (originalValue.is<int>()) {
				toRestore[varName] = originalValue.as<int>();
			}
			else if (originalValue.is<unsigned int>()) {
				toRestore[varName] = originalValue.as<unsigned int>();
			}
			else if (originalValue.is<float>()) {
				toRestore[varName] = originalValue.as<float>();
			}
			else if (originalValue.is<double>()) {
				toRestore[varName] = originalValue.as<double>();
			}
			else if (originalValue.is<std::string>()) {
				toRestore[varName] = originalValue.as<std::string>();
			}
			else if (originalValue.is<std::shared_ptr<Entity>>()) {
				toRestore[varName] = originalValue.as<std::shared_ptr<Entity>>();
			}
			else if (originalValue.is<Vec2>()) {
				toRestore[varName] = originalValue.as<Vec2>();
			}
		}

		// Now push the new value from toPush into the Lua environment
		if (varValue.type() == typeid(int)) {
			env[varName] = std::any_cast<int>(varValue);
		}
		else if (varValue.type() == typeid(unsigned int)) {
			env[varName] = std::any_cast<unsigned int>(varValue);
		}
		else if (varValue.type() == typeid(float)) {
			env[varName] = std::any_cast<float>(varValue);
		}
		else if (varValue.type() == typeid(double)) {
			env[varName] = std::any_cast<double>(varValue);
		}
		else if (varValue.type() == typeid(std::string)) {
			env[varName] = std::any_cast<std::string>(varValue);
		}
		else if (varValue.type() == typeid(std::shared_ptr<Entity>)) {
			env[varName] = std::any_cast<std::shared_ptr<Entity>>(varValue);
		}
		else if (varValue.type() == typeid(Vec2)) {
			env[varName] = std::any_cast<Vec2>(varValue);
		}
	}
}

void ScriptManager::popEnvironmentVariables(std::unordered_map<std::string, std::any>& toRestore, sol::environment& env)
{
	for (const auto& pair : toRestore)
	{
		const std::string& varName = pair.first;
		const std::any& varValue = pair.second;

		// Restore the original value from toRestore back into the Lua environment
		if (varValue.type() == typeid(int)) {
			env[varName] = std::any_cast<int>(varValue);
		}
		else if (varValue.type() == typeid(unsigned int)) {
			env[varName] = std::any_cast<unsigned int>(varValue);
		}
		else if (varValue.type() == typeid(float)) {
			env[varName] = std::any_cast<float>(varValue);
		}
		else if (varValue.type() == typeid(double)) {
			env[varName] = std::any_cast<double>(varValue);
		}
		else if (varValue.type() == typeid(std::string)) {
			env[varName] = std::any_cast<std::string>(varValue);
		}
		else if (varValue.type() == typeid(std::shared_ptr<Entity>)) {
			env[varName] = std::any_cast<std::shared_ptr<Entity>>(varValue);
		}
		else if (varValue.type() == typeid(Vec2)) {
			env[varName] = std::any_cast<Vec2>(varValue);
		}
	}
	// Clear the toRestore map once everything is restored
	toRestore.clear();
}