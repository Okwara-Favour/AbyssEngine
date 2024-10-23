#pragma once
#include <iostream>
#include <map>
#include <sol/sol.hpp>
#include <any>

class Scriptable
{
public:
	std::string name;
	sol::table scriptClass;
	sol::table instance;
	std::shared_ptr<bool> instantiated = std::make_shared<bool>(false);
	std::shared_ptr<bool> destroy = std::make_shared<bool>(false);
	std::shared_ptr<bool> hasMapped = std::make_shared<bool>(false);
	std::unordered_map<std::string, std::any> variableMap = {};
	Scriptable(const std::string& n = "") : name(n) {}
};