#pragma once
#include <SFML/Graphics.hpp>
#include <typeinfo>
#include "Vec2.h"

struct SimpleGradient
{
	sf::Glsl::Vec2 windowSize = sf::Glsl::Vec2(800,600);
	sf::Glsl::Vec2 u_resolution = sf::Glsl::Vec2(800,600);
	sf::Glsl::Vec2 lightPosition = sf::Glsl::Vec2();
	sf::Glsl::Vec3 lightcolorstart = sf::Glsl::Vec3(1.0,1.0,1.0);
	sf::Glsl::Vec3 lightcolorend = sf::Glsl::Vec3(0.0, 0.0, 0.0);
	float lightRadius = 0.5;
	SimpleGradient(const Vec2& ws = Vec2(800, 600), const Vec2& ur = Vec2(800, 600), const Vec2& lp = Vec2(),
		const sf::Color& lcs = sf::Color::White, const sf::Color& lce = sf::Color::Black,
		const float lr = 0.5)
		:windowSize(sf::Glsl::Vec2(ws.x, ws.y)), u_resolution(sf::Glsl::Vec2(ur.x, ur.y))
		,lightPosition(sf::Glsl::Vec2(lp.x, lp.y))
		,lightcolorstart(sf::Glsl::Vec3(lcs.r/255.0f, lcs.g/255.0f, lcs.b/255.0f))
		,lightcolorend(sf::Glsl::Vec3(lce.r / 255.0f, lce.g / 255.0f, lce.b / 255.0f)) 
	{}
};

class GradientMaker
{
	sf::Vector2f m_size;
	std::shared_ptr<sf::Shader> m_shader;
	std::string m_shaderName;
	SimpleGradient simpleGradient;
public:
	GradientMaker(const sf::Vector2f& size, std::shared_ptr<sf::Shader>& shader, const std::string& shaderName);
	template <class T>
	T& getGradient() {
		if constexpr (std::is_same<T, SimpleGradient>::value)
		{
			return simpleGradient;
		}
		std::string typeName = typeid(T).name();
		std::string errorString = "Unsupported type " + typeName + " passed to getGradient()";
		throw std::logic_error(errorString);
	}
	void makeGradient(const std::string& gradientName = "NewGradient");
};

