#pragma once
#include <cmath>
#include <string>

class Vec2
{
public:
    float x = 0, y = 0;
    const double pi = 3.14159265358979323846;

    Vec2();
    Vec2(float xin, float yin);


    Vec2& operator = (const Vec2& rhs);
    Vec2& operator = (float num);
    Vec2 operator + (const Vec2& rhs) const;
    Vec2 operator + (float num) const;
    Vec2 operator - (const Vec2& rhs) const;
    Vec2 operator - (float num) const;
    Vec2 operator * (float num) const;
    float operator * (const Vec2& rhs) const;
    Vec2 operator / (float num) const;
    Vec2 operator / (const Vec2& rhs) const;
    void operator += (const Vec2& rhs);
    void operator += (float num);
    void operator -= (const Vec2& rhs);
    void operator -= (float num);
    void operator *= (float num);
    void operator *= (const Vec2& rhs);
    void operator /= (float num);
    bool operator != (const Vec2& rhs);
    bool operator == (const Vec2& rhs);
    Vec2 abs();
    Vec2& add(const Vec2& v);
    Vec2& add(float num);
    Vec2& scale(float s);
    Vec2& rotate(float deg);
    Vec2 multiply(const Vec2& v);
    float dist(const Vec2& v) const;
    float angleFromPoint(const Vec2& p);
    double angleBetweenTwoPoints(const Vec2& p1, const Vec2& p2);
    double length();
    void normalize();
    Vec2 speedVector(const Vec2& p, float S);
    void setVec2(float nx, float ny);
    Vec2& makeWholeNumber();
    std::string toString();
};