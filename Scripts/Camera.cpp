#include "Camera.h"

Camera::Camera(const Vec2& pos, const Vec2& size)
	:m_view(sf::FloatRect(pos.x, pos.y, size.x, size.y))
    ,m_rectangle(sf::Vector2f(size.x, size.y))
{
    m_rectangle.setOrigin(size.x / 2.0f, size.y / 2.0f);
    m_rectangle.setPosition(pos.x, pos.y);
    m_rectangle.setFillColor(sf::Color::Transparent);
    m_rectangle.setOutlineThickness(10);
    m_rectangle.setOutlineColor(sf::Color::White);
}


void Camera::followEntity(const Vec2& pos)
{
    auto& size = m_view.getSize();
    float centerX = std::max(size.x / 2.0f, pos.x);
    float centerY = std::max(size.y / 2.0f, pos.y);
    m_view.setCenter(centerX, centerY);
    m_rectangle.setPosition(centerX, centerY);
}

void Camera::followEntity(const Vec2& pos, const Vec2& minBounds, const Vec2& maxBounds)
{
    auto& size = m_view.getSize();
    float centerX = std::max(size.x / 2.0f, pos.x);
    float centerY = std::max(size.y / 2.0f, pos.y);
    centerX = std::max(minBounds.x + size.x / 2.0f, std::min(centerX, maxBounds.x - size.x / 2.0f));
    centerY = std::max(minBounds.y + size.y / 2.0f, std::min(centerY, maxBounds.y - size.y / 2.0f));
    m_view.setCenter(centerX, centerY);
    m_rectangle.setPosition(centerX, centerY);
}

const bool Camera::outOfCameraBounds(const Vec2& pos) const
{
    auto& center = m_view.getCenter();
    auto& size = m_view.getSize();
    float left = center.x - size.x / 2;
    float right = center.x + size.x / 2;
    float top = center.y - size.y / 2;
    float bottom = center.y + size.y / 2;

    return pos.x < left || pos.x > right || pos.y < top || pos.y > bottom;
}


Vec2 Camera::getSize() 
{ 
    auto& size = m_view.getSize();
    return Vec2(size.x, size.y);
}

const Vec2& Camera::getScale()
{
    return m_scale;
}

Vec2 Camera::getPosition()
{
    auto& pos = m_view.getCenter();
    return Vec2(pos.x, pos.y);
}
float Camera::getAngle()
{
    return m_view.getRotation();
}

const sf::Color& Camera::getColor()
{
    return m_color;
}

void Camera::setPosition(float x, float y)
{
    m_view.setCenter(x, y);
    m_rectangle.setPosition(x, y);
}
void Camera::setScale(float x, float y)
{
    m_rectangle.setScale(x, y);
    m_scale = Vec2(x, y);
    auto size = m_rectangle.getGlobalBounds().getSize();
    m_view.setSize(size.x, size.y);
}
void Camera::setSize(float x, float y)
{
    m_view.setSize(x, y);
    m_rectangle.setSize(sf::Vector2f(x, y));
    m_rectangle.setOrigin(x / 2.0f, y / 2.0f);
}
void Camera::setPosition(const Vec2& p)
{
    m_view.setCenter(p.x, p.y);
    m_rectangle.setPosition(p.x, p.y);
}
void Camera::setScale(const Vec2& s)
{
    m_rectangle.setScale(s.x, s.y);
    m_scale = s;
    auto size = m_rectangle.getGlobalBounds().getSize();
    m_view.setSize(size.x, size.y);
    
}
void Camera::setSize(const Vec2& s)
{
    m_view.setSize(s.x, s.y);
    m_rectangle.setSize(sf::Vector2f(s.x, s.y));
    m_rectangle.setOrigin(s.x / 2.0f, s.y / 2.0f);
}

void Camera::setColor(int r, int g, int b, int a)
{
    m_color = sf::Color(r, g, b, a);
}

void Camera::setAngle(float angle)
{
    m_view.setRotation(angle);
    m_rectangle.setRotation(angle);
}

void Camera::apply(sf::RenderTexture& texture) const
{
    texture.setView(m_view);
}
void Camera::apply(sf::RenderWindow& window, const Vec2& pos, const Vec2& size)
{
    m_view.setViewport(sf::FloatRect(pos.x, pos.y, size.x, size.y));
    window.setView(m_view);
}

void Camera::Lua(sol::state& lua)
{
    lua.new_usertype<Camera>("Camera",
        sol::constructors<Camera(), Camera(const Vec2&, const Vec2&)>(),

        "getSize", &Camera::getSize,
        "getScale", &Camera::getScale,
        "getPosition", &Camera::getPosition,
        "getColor", &Camera::getColor,
        "getAngle", &Camera::getAngle,

        "setPosition", sol::overload(
            static_cast<void(Camera::*)(float, float)>(&Camera::setPosition),
            static_cast<void(Camera::*)(const Vec2&)>(&Camera::setPosition)
        ),
        "setScale", sol::overload(
            static_cast<void(Camera::*)(float, float)>(&Camera::setScale),
            static_cast<void(Camera::*)(const Vec2&)>(&Camera::setScale)
        ),
        "setSize", sol::overload(
            static_cast<void(Camera::*)(float, float)>(&Camera::setSize),
            static_cast<void(Camera::*)(const Vec2&)>(&Camera::setSize)
        ),
        "setAngle", &Camera::setAngle,
        "setColor", &Camera::setColor,

        "outOfCameraBounds", &Camera::outOfCameraBounds,
        "followEntity", sol::overload(
            static_cast<void(Camera::*)(const Vec2&)>(&Camera::followEntity),
            static_cast<void(Camera::*)(const Vec2&, const Vec2&, const Vec2&)>(&Camera::followEntity)
        )
    );
}