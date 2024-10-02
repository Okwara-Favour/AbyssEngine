#pragma once
#include <iostream>
#include <map>
#include <any>

class Inspector;

class Scriptable
{
	friend Inspector;
	std::map<std::string, std::shared_ptr<std::any>> variableMap;
	std::string script;
protected:
	template<typename T>
	void registerVariable(const std::string& varName, T& varValue) {
		auto sharedValue = std::make_shared<std::any>(std::ref(varValue));
		if (variableMap.find(varName) == variableMap.end()) variableMap[varName] = sharedValue;
		else throw std::runtime_error("Key already registered: " + varName);
	}
public:
	std::string name;
	//Scriptable(const std::string& n)
};