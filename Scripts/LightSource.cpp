#include "LightSource.h"
#include <iostream>

LightSource::LightSource(const Vec2& lpos, const Vec2& wSize, const float e,
    const sf::Texture& t,
    const sf::Color& color,
    const int n, const float sAngle, const float eAngle)
    :lightSource(lpos), canvasSize(Vec2(wSize.x, wSize.x)), color(color), extent(e), lines(n)
    , startAngle(fmodf(sAngle, 360.0f)), endAngle(fmodf(eAngle, 360.0f)), windowSize(wSize)
    , texture(t)
{
    defaultStartAngle = startAngle, defaultEndAngle = endAngle;
    makeCircle();
    makeLinesegments();
    makePolygons();
}

void LightSource::makeCircle()
{
    circle.setPosition(lightSource.x, lightSource.y);
    circle.setRadius(extent);
    circle.setOrigin(extent, extent);
    circle.setFillColor(color);
}

void LightSource::makeLinesegments()
{
    linesegments.clear();
    float diffAngle = (startAngle > endAngle) ? endAngle - (startAngle - 360) : endAngle - startAngle;
    float divAngle = diffAngle / lines;
    //const float epsilon = 1e-6f;  // Adjust epsilon as needed
    float angle = startAngle;
    for (int i = 0; i <= lines; i++)
    {
        angle = fmodf(angle, 360.0f);
        float radians = angle * 3.1415926535 / 180;
        lineSegment temp(Vec2(lightSource.x, lightSource.y), Vec2(lightSource.x, lightSource.y - extent));
        auto& endPos = temp.m_endPos;
        auto& startPos = temp.m_startPos;
        Vec2 delta = endPos - startPos;
        Vec2 rotatedPos;
        rotatedPos.x = delta.x * cos(radians) - delta.y * sin(radians);
        rotatedPos.y = delta.x * sin(radians) + delta.y * cos(radians);
        endPos = startPos + rotatedPos;
        linesegments.push_back(temp);
        angle += divAngle;
    }
}

void LightSource::makePolygons()
{
    int n = linesegments.size() - 1;
    if (n < 0) 
    {
        std::cout << startAngle << " " << endAngle << std::endl;
        return;
    }
    vertices = sf::VertexArray(sf::Triangles, 3 * n);
    auto makePoly = [this](lineSegment& l1, lineSegment& l2, int index)->void {
        vertices[3 * index + 0].position = sf::Vector2f(l1.m_startPos.x, l1.m_startPos.y);
        vertices[3 * index + 1].position = sf::Vector2f(l1.m_endPos.x, l1.m_endPos.y);
        vertices[3 * index + 2].position = sf::Vector2f(l2.m_endPos.x, l2.m_endPos.y);

        vertices[3 * index + 0].color = color;
        vertices[3 * index + 1].color = color;
        vertices[3 * index + 2].color = color;

        auto lightPos = sf::Vector2f(lightSource.x, lightSource.y);
        float angle1 = l1.m_startPos.angleFromPoint(l1.m_endPos);
        float angle2 = l1.m_startPos.angleFromPoint(l2.m_endPos);
        auto textureHalfSize = sf::Vector2f(texture.getSize()) / 2.0f;
        Vec2 endTexPoint1(textureHalfSize.x, textureHalfSize.y);
        Vec2 endTexPoint2(textureHalfSize.x, textureHalfSize.y);

        endTexPoint1 *= Vec2(cos(angle1), sin(angle1));
        endTexPoint2 *= Vec2(cos(angle2), sin(angle2));

        vertices[3 * index + 0].texCoords = textureHalfSize;
        vertices[3 * index + 1].texCoords = textureHalfSize + sf::Vector2f(endTexPoint1.x, endTexPoint1.y);
        vertices[3 * index + 2].texCoords = textureHalfSize + sf::Vector2f(endTexPoint2.x, endTexPoint2.y);
        };

    for (int i = 0; i < n; i++)
    {
        int nextI = i + 1;
        makePoly(linesegments[i], linesegments[nextI], i);
    }
}

void LightSource::setTexture(const sf::Texture& t)
{
    texture = t;
}

void LightSource::setLightSource(const Vec2& lpos)
{
    lightSource = lpos;
}

void LightSource::setExtent(float e)
{
    extent = e;
}
void LightSource::setLines(int n)
{
    lines = n;
}

void LightSource::setDefaultAngles(float sAngle, float eAngle)
{
    defaultStartAngle = fmodf(sAngle, 360.0f), defaultEndAngle = fmodf(eAngle, 360.0f);
}

void LightSource::setAngles(float sAngle, float eAngle)
{
    startAngle = fmodf(sAngle, 360.0f), endAngle = fmodf(eAngle, 360.0f);
    defaultStartAngle = startAngle, defaultEndAngle = endAngle;
}

void LightSource::setAngle(float angle)
{
    angle = fmodf(angle, 360.0f);
    float angleDiff = (defaultEndAngle - defaultStartAngle) / 2.0f;
    float minAng = angle - angleDiff;
    float maxAng = angle + angleDiff;
    startAngle = fmodf(minAng, 360.0f);
    endAngle = fmodf(maxAng, 360.0f);

    //std::cout << startAngle << " " << endAngle << " " << minAng << " " << maxAng << " " << angleDiff << std::endl;
}

void LightSource::reMake()
{
    makeCircle();
    makeLinesegments();
    makePolygons();
}


void LightSource::drawPolygons(sf::RenderWindow& window, const bool withTexture) const
{
    if (withTexture && useTexture) window.draw(vertices, &texture);
    else window.draw(vertices);
}

void LightSource::drawCircle(sf::RenderWindow& window, const bool withTexture) const
{
    if (withTexture && useTexture) window.draw(circle, &texture);
    else window.draw(circle);
}

void LightSource::cutLineOnIntersect(lineSegment& pointer, Intersect& intersect, Vec2& minIntersect)
{
    if ((pointer.m_endPos.x <= pointer.m_startPos.x && intersect.m_pos.x >= minIntersect.x) ||
        (pointer.m_endPos.x >= pointer.m_startPos.x && intersect.m_pos.x <= minIntersect.x))
    {
        minIntersect.x = intersect.m_pos.x;
    }

    if ((pointer.m_endPos.y <= pointer.m_startPos.y && intersect.m_pos.y >= minIntersect.y) ||
        (pointer.m_endPos.y >= pointer.m_startPos.y && intersect.m_pos.y <= minIntersect.y))
    {
        minIntersect.y = intersect.m_pos.y;
    }
}

void LightSource::rectIntersect(const Vec2& pos, const Vec2& size)
{
    auto rectangleToLineSegments = [](const Vec2& topLeft, const Vec2& bottomRight) -> std::vector<lineSegment> {
        std::vector<lineSegment> segments;
        segments.push_back(lineSegment(topLeft, Vec2(bottomRight.x, topLeft.y))); // Top
        segments.push_back(lineSegment(Vec2(bottomRight.x, topLeft.y), bottomRight)); // Right
        segments.push_back(lineSegment(bottomRight, Vec2(topLeft.x, bottomRight.y))); // Bottom
        segments.push_back(lineSegment(Vec2(topLeft.x, bottomRight.y), topLeft)); // Left
        return segments;
        };

    std::vector<lineSegment> other = rectangleToLineSegments(pos - size / 2, pos + size / 2);

    for (int i = 0; i < linesegments.size(); i++)
    {
        Vec2 minIntersect = linesegments[i].m_endPos;
        for (int j = 0; j < other.size(); j++)
        {
            Intersect intersect = linesegments[i].intersect(other[j]);
            if (intersect.m_result)
            {
                cutLineOnIntersect(linesegments[i], intersect, minIntersect);
            }
        }
        linesegments[i].m_endPos = minIntersect;
    }

    makePolygons();
}
