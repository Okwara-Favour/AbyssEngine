#include "Vec2.h"
#include <iostream>


Vec2::Vec2() {}
Vec2::Vec2(float xin, float yin)
    :x(xin)
    , y(yin)
{
}

Vec2& Vec2::operator = (const Vec2& rhs)
{
    x = rhs.x;
    y = rhs.y;
    return *this;
}

Vec2& Vec2::operator = (float num)
{
    x = num;
    y = num;
    return *this;
}

Vec2 Vec2::operator + (const Vec2& rhs) const
{
    return Vec2(x + rhs.x, y + rhs.y);
}
Vec2 Vec2::operator + (float num) const
{
    return Vec2(x + num, y + num);
}

Vec2 Vec2::operator - (const Vec2& rhs) const
{
    return Vec2(x - rhs.x, y - rhs.y);
}
Vec2 Vec2::operator - (float num) const
{
    return Vec2(x - num, y - num);
}

Vec2 Vec2::operator * (float num) const
{
    return Vec2(x * num, y * num);
}

float Vec2::operator * (const Vec2& rhs) const
{
    return (this->x * rhs.y) - (this->y * rhs.x);
}

Vec2 Vec2::operator / (float num) const
{
    if (num != 0)  return Vec2(x / num, y / num);
    else return Vec2();
}

Vec2 Vec2::operator / (const Vec2& rhs) const
{
    if (rhs.x == 0 && rhs.y == 0)  return Vec2(0, 0);
    if (rhs.x == 0 && rhs.y != 0) return Vec2(0, y / rhs.y);
    if (rhs.x != 0 && rhs.y == 0)  return Vec2(x / rhs.x, 0);
    else return Vec2(x/rhs.x, y/rhs.y);
}

void Vec2::operator += (const Vec2& rhs)
{
    x += rhs.x;
    y += rhs.y;
}

void Vec2::operator += (float num)
{
    x += num;
    y += num;
}

void Vec2::operator -= (const Vec2& rhs)
{
    x -= rhs.x;
    y -= rhs.y;
}

void Vec2::operator -= (float num)
{
    x -= num;
    y -= num;
}

void Vec2::operator *= (float num)
{
    x *= num;
    y *= num;
}

void Vec2::operator *= (const Vec2& rhs)
{
    x *= rhs.x;
    y *= rhs.y;
}

void Vec2::operator /= (float num)
{
    if (num != 0)
    {
        x /= num;
        y /= num;
    }
    else 
    {
        x = 0;
        y = 0;
    }
    
}

bool Vec2::operator != (const Vec2& rhs)
{
    return (x != rhs.x) || (y != rhs.y);
}

bool Vec2::operator == (const Vec2& rhs)
{
    return (x == rhs.x) && (y == rhs.y);
}

Vec2 Vec2::abs()
{
    return Vec2(std::abs(x), std::abs(y));
}

Vec2& Vec2::add(const Vec2& v)
{
    x += v.x;
    y += v.y;
    return *this;
}

Vec2& Vec2::add(float num)
{
    x += num;
    y += num;
    return *this;
}

Vec2& Vec2::scale(float s)
{
    x *= s;
    y *= s;
    return *this;
}

Vec2 Vec2::multiply(const Vec2& v)
{
    return Vec2(x * v.x, y * v.y);
}

Vec2& Vec2::rotate(float deg)
{
    float rad = deg * 3.14159265358979323846 / 180.0;

    // Perform rotation using trigonometric functions
    float new_x = x * cos(rad) - y * sin(rad);
    float new_y = x * sin(rad) + y * cos(rad);

    // Update the vector components
    x = new_x;
    y = new_y;

    return *this;
}

float Vec2::dist(const Vec2& v) const
{
    return sqrtf((v.x - x) * (v.x - x) + (v.y - y) * (v.y - y));
}

float Vec2::angleFromPoint(const Vec2& p)
{
    Vec2 direction = p - *this;
    double angleRadians = std::atan2(direction.y, direction.x);

    // Ensure the angle is positive (between 0 and 2*pi)
    if (angleRadians < 0.0) {
        angleRadians += 2.0 * pi;
    }

    return angleRadians;
};
double Vec2::angleBetweenTwoPoints(const Vec2& p1, const Vec2& p2)
{
    Vec2 direction = p2 - p1;
    double angleRadians = std::atan2(direction.y, direction.x);

    // Ensure the angle is positive (between 0 and 2*pi)
    if (angleRadians < 0.0) {
        angleRadians += 2.0 * pi;
    }

    return angleRadians;
};

double Vec2::length()
{
    return sqrtf((x * x) + (y * y));
}

void Vec2::normalize()
{
    double len = length();
    if (len != 0)
    {
        x = x / len;
        y = y / len;
    }
}

Vec2 Vec2::speedVector(const Vec2& p, float S)
{
    Vec2 Direc = p - *this;
    Direc.normalize();
    return Direc.scale(S);
}

void Vec2::setVec2(float nx, float ny)
{
    x = nx;
    y = ny;
}

std::string Vec2::toString()
{
    return std::to_string(x) + " " + std::to_string(y);
}

Vec2& Vec2::makeWholeNumber()
{
    x = (int)x;
    y = (int)y;
    return *this;
}