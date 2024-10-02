#pragma once
#include <iostream>
#include <map>
#include <sol/sol.hpp>
#include <any>

class Scriptable
{
public:
	std::string name;
	std::shared_ptr<sol::table> scriptClass;
	std::shared_ptr<sol::table> instance;
	std::shared_ptr<bool> instantiated = std::make_shared<bool>(false);
	std::shared_ptr<bool> destroy = std::make_shared<bool>(false);
	Scriptable(const std::string& n = "") : name(n) {}
};