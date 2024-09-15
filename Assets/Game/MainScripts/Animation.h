#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Vec2.h"


class Animation
{
	sf::Sprite m_sprite;
	size_t m_frameCount = 1;
	size_t m_currentFrame = 0;
	size_t m_speed = 0;
	Vec2 m_size = { 1,1 };
	std::string m_name = "none";
	std::string m_texName = "none";

public:
	Animation();
	Animation(const std::string& name, const sf::Texture& t);
	Animation(const std::string& name, const sf::Texture& t, const Vec2& resize);
	Animation(const std::string& name, const sf::Texture& t, size_t frameCount, size_t speed);
	Animation(const std::string& name, const sf::Texture& t, const Vec2& resize, size_t frameCount, size_t speed);
	Animation(const Animation& anim, const Vec2& resize);
	Animation(const Animation& anim);
	void update();
	bool hasEnded() const;
	const std::string& getName() const;
	const Vec2& getSize() const;
	const size_t getFrame() const;
	const size_t getFrameCount() const;
	const size_t getSpeed() const;
	sf::Sprite& getSprite();
};

