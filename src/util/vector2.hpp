#pragma once

#include <cmath>

/**
 * @brief 二维向量类，用于表示和操作2D空间中的向量
 */
class Vector2
{
public:
    double x = 0.0;    // x坐标分量
    double y = 0.0;    // y坐标分量

public:
    // 默认构造函数
    Vector2() = default;

    /**
     * @brief 带参数构造函数
     * @param x x坐标分量
     * @param y y坐标分量
     */
    Vector2(double x, double y) : x(x), y(y) {}

    // 默认析构函数
    ~Vector2() = default;

    /**
     * @brief 向量加法运算符重载
     * @param other 要相加的向量
     * @return 两个向量相加的结果
     */
    Vector2 operator+(const Vector2& other) const
    {
        return Vector2(x + other.x, y + other.y);
    }

    /**
     * @brief 向量加法赋值运算符重载
     * @param other 要相加的向量
     */
    void operator+=(const Vector2& other)
    {
        x += other.x;
        y += other.y;
    }

    /**
     * @brief 向量减法运算符重载
     * @param other 要相减的向量
     * @return 两个向量相减的结果
     */
    Vector2 operator-(const Vector2& other) const
    {
        return Vector2(x - other.x, y - other.y);
    }

    /**
     * @brief 向量减法赋值运算符重载
     * @param other 要相减的向量
     */
    void operator-=(const Vector2& other)
    {
        x -= other.x;
        y -= other.y;
    }

    /**
     * @brief 向量点乘运算符重载
     * @param other 要点乘的向量
     * @return 两个向量的点积
     */
    double operator*(const Vector2& other) const
    {
        return x * other.x + y * other.y;
    }

    /**
     * @brief 向量数乘运算符重载
     * @param scalar 标量因子
     * @return 向量与标量相乘的结果
     */
    Vector2 operator*(double scalar) const
    {
        return Vector2(x * scalar, y * scalar);
    }

    /**
     * @brief 向量数乘赋值运算符重载
     * @param scalar 标量因子
     */
    void operator*=(double scalar)
    {
        x *= scalar;
        y *= scalar;
    }

    /**
     * @brief 向量相等比较运算符重载
     * @param other 要比较的向量
     * @return 两个向量是否相等
     */
    bool operator==(const Vector2& other) const
    {
        return x == other.x && y == other.y;
    }

    /**
     * @brief 向量大于比较运算符重载（基于向量长度）
     * @param other 要比较的向量
     * @return 当前向量长度是否大于other向量长度
     */
    bool operator>(const Vector2& other) const
    {
        return length() > other.length();
    }

    /**
     * @brief 向量小于比较运算符重载（基于向量长度）
     * @param other 要比较的向量
     * @return 当前向量长度是否小于other向量长度
     */
    bool operator<(const Vector2& other) const
    {
        return length() < other.length();
    }

    /**
     * @brief 计算向量的长度（模）
     * @return 向量的长度
     */
    double length() const
    {
        return sqrt(x * x + y * y);
    }

    /**
     * @brief 获取向量的单位向量
     * @return 与原向量方向相同的单位向量，如果原向量为零向量则返回零向量
     */
    Vector2 normalize() const
    {
        double length = this->length();
        if (length == 0)
            return Vector2(0, 0);

        return Vector2(x / length, y / length);
    }

    /**
     * @brief 判断向量是否近似为零向量
     * @return 如果向量长度小于0.00001则返回true
     */
    bool approxZero() const
    {
        return length() < 0.00001;
    }
};
