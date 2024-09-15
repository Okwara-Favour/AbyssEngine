#include "Animation.h"

Animation::Animation()
{}

Animation::Animation(const std::string& name, const sf::Texture& t)
	:m_name(name)
	,m_sprite(t)
{
	m_size = Vec2((float)t.getSize().x, (float)t.getSize().y);
	m_sprite.setOrigin(m_size.x / 2.0f, m_size.y / 2.0f);
	m_sprite.setTextureRect(sf::IntRect(0, 0, m_size.x, m_size.y));
}

Animation::Animation(const std::string& name, const sf::Texture& t, const Vec2& resize)
	:m_name(name)
	,m_sprite(t)
	,m_size(resize)
{
	m_sprite.setScale(resize.x / (float)t.getSize().x, resize.y / (float)t.getSize().y);
	m_sprite.setOrigin(m_size.x / 2.0f, m_size.y / 2.0f);
	m_sprite.setTextureRect(sf::IntRect(0, 0, m_size.x, m_size.y));
}

Animation::Animation(const std::string& name, const sf::Texture& t, size_t frameCount, size_t speed)
	:m_name(name)
	,m_sprite(t)
	,m_frameCount(frameCount)
	,m_speed(speed)
{
	m_size = Vec2((float)t.getSize().x / frameCount, (float)t.getSize().y);
	m_sprite.setOrigin(m_size.x / 2.0f, m_size.y / 2.0f);
	m_sprite.setTextureRect(sf::IntRect(0, 0, m_size.x, m_size.y));
}

Animation::Animation(const std::string& name, const sf::Texture& t, const Vec2& resize, size_t frameCount, size_t speed)
	: m_name(name)
	, m_sprite(t)
	, m_size(Vec2(resize.x / frameCount, resize.y))
	, m_frameCount(frameCount)
	, m_speed(speed)
{
	m_sprite.setScale(resize.x / (float)t.getSize().x, resize.y / (float)t.getSize().y);
	m_sprite.setOrigin(m_size.x / 2.0f, m_size.y / 2.0f);
	m_sprite.setTextureRect(sf::IntRect(0, 0, m_size.x, m_size.y));
}

Animation::Animation(const Animation& anim, const Vec2& resize)
	:m_name(anim.m_name)
	,m_sprite(anim.m_sprite)
	,m_currentFrame(anim.m_currentFrame)
	,m_frameCount(anim.m_frameCount)
	,m_speed(anim.m_speed)
{
	auto t = m_sprite.getTexture();
	sf::Vector2u prevSize = t->getSize();
	m_size = Vec2(prevSize.x / anim.m_frameCount, prevSize.y);
	m_sprite.setScale((resize.x * anim.m_frameCount) / prevSize.x, resize.y / prevSize.y);
	m_sprite.setTextureRect(sf::IntRect(0, 0, m_size.x, m_size.y));
}

Animation::Animation(const Animation& anim)
	:m_name(anim.m_name)
	,m_sprite(anim.m_sprite)
	,m_currentFrame(anim.m_currentFrame)
	,m_frameCount(anim.m_frameCount)
	,m_speed(anim.m_speed)
	,m_size(anim.m_size)
{}
void Animation::update()
{
	m_currentFrame++;
	if (m_speed > 0)
	{
		size_t animFrame = (m_currentFrame / m_speed) % m_frameCount;
		m_sprite.setTextureRect(sf::IntRect(animFrame * m_size.x, 0, m_size.x, m_size.y));
	}
}
bool Animation::hasEnded() const
{
	if (m_speed == 0)
	{
		return true;
	}
	return ((m_currentFrame / m_speed) % m_frameCount) == (m_frameCount - 1);
}
const std::string& Animation::getName() const
{
	return m_name;
}
const Vec2& Animation::getSize() const
{
	return m_size;
}
const size_t Animation::getFrameCount() const
{
	return m_frameCount;
}

const size_t Animation::getSpeed() const
{
	return m_speed;
}

const size_t Animation::getFrame() const
{
	return m_currentFrame;
}
sf::Sprite& Animation::getSprite()
{
	return m_sprite;
}