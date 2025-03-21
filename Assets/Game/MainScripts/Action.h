#pragma once
#include <iostream>
#include "Vec2.h"

class Action
{
	std::string m_name = "none";
	std::string m_type = "none";
	Vec2		m_pos;
public:
	Action();
	Action(const std::string& name, const std::string& type);
	Action(const std::string& name, const std::string& type, const Vec2& pos);
	const std::string& name() const;
	const std::string& type() const;
	const Vec2& pos() const;
};

