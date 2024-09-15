#include "Camera.h"
Camera::Camera() {}
Camera::Camera(const Vec2& winSize, const Vec2& p, const Vec2& s, const Vec2& gs)
	:m_winSize(winSize), m_cellPos(p), m_cellSize(s), m_cameraPos(Vec2(p.x* s.x, p.y* s.y)), m_gridSize(gs)
{}
void Camera::createCellView(const Vec2& pos)
{
    m_cellPos = pos;
    m_cameraPos = Vec2(pos.x * m_cellSize.x, pos.y * m_cellSize.y);
    m_view = sf::View(sf::FloatRect(m_cameraPos.x, m_cameraPos.y, m_cellSize.x, m_cellSize.y));
}

void Camera::createFullView()
{
    m_cameraPos = Vec2(0, 0);
    m_view = sf::View(sf::FloatRect(0, 0, m_winSize.x, m_winSize.y));
}

void Camera::followEntity(const Vec2& pos, const Vec2& size, const Vec2& minBounds, const Vec2& maxBounds)
{
    m_view = sf::View(sf::FloatRect(pos.x, pos.y, size.x, size.y));
    float centerX = std::max(size.x / 2.0f, pos.x);
    float centerY = std::max(size.y / 2.0f, pos.y);
    centerX = std::max(minBounds.x + size.x / 2.0f, std::min(centerX, maxBounds.x - size.x / 2.0f));
    centerY = std::max(minBounds.y + size.y / 2.0f, std::min(centerY, maxBounds.y - size.y / 2.0f));
    m_view.setCenter(centerX, centerY);
}

const Vec2& Camera::cameraCellSize() const
{
    return m_cellSize;
}

const bool Camera::outOfCameraBounds(const Vec2& pos) const
{
    return pos.x < 0 || pos.y < 0 || pos.x >= m_gridSize.x || pos.y >= m_gridSize.y;
}