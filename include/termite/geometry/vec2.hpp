#pragma once

namespace tmt
{
    /// @brief A 2-dimensional vector
    template <typename T>
    struct Vec2
    {
        T x, y;

        constexpr Vec2() : x(0), y(0) {}
        constexpr Vec2(const T& x, const T& y) : x(x), y(y) {}

        /// @brief Add two @c Vec2 element-wise
        constexpr Vec2 operator+(const Vec2& other) const { return { this->x + other.x, this->y + other.y }; }

        /// @brief Subtract two @c Vec2 element-wise
        constexpr Vec2 operator-(const Vec2& other) const { return { this->x - other.x, this->y - other.y }; }

        /// @brief Scale the @c Vec2 by a scalar
        constexpr Vec2 operator*(const T& scalar) const { return { this->x*scalar, this->y*scalar }; }

        /// @brief Scale the @c Vec2 down by a scalar
        constexpr Vec2 operator/(const T& scalar) const { return { this->x/scalar, this->y/scalar }; }

        /// @brief Add another @c Vec2 to this one element-wise
        constexpr Vec2& operator+=(const Vec2& other) { this->x += other.x; this->y += other.y; return *this; }

        /// @brief Subtract another @c Vec2 from this one element-wise
        constexpr Vec2& operator-=(const Vec2& other) { this->x -= other.x; this->y -= other.y; return *this; }

        /// @brief Scale the @c Vec2 by a scalar
        constexpr Vec2 operator*=(const T& scalar) const { this->x *= scalar; this->y *= scalar; return *this;  }

        /// @brief Scale the @c Vec2 down by a scalar
        constexpr Vec2 operator/=(const T& scalar) const { this->x /= scalar; this->y /= scalar; return *this; }

        /// @brief Exact equality of dimensions
        constexpr bool operator==(const Vec2& other) const { return this->x == other.x && this->y == other.y; }

        /// @brief Inequality
        constexpr bool operator!=(const Vec2& other) const { return this->x != other.x || this->y != other.y;}

        /// @brief Convert to another @c Vec2 type
        /// @tparam Tp The target type
        template <typename Tp>
        constexpr Vec2<Tp> as() const { return { static_cast<Tp>(x), static_cast<Tp>(y) }; }
    };


    /// @brief Scale the @c Vec2 by a scalar
    template <typename T>
    constexpr Vec2<T> operator*(const T& scalar, const Vec2<T>& vec) { return { vec.x*scalar, vec.y*scalar }; }


    /// @brief Compute the dot product of two @c Vec2
    template <typename T>
    constexpr T dot(const Vec2<T>& a, const Vec2<T>& b) { return a.x*b.x + a.y*b.y; }

    /// @brief A 2-dimensional vector of type @c int
    using Vec2i = Vec2<int>;

    /// @brief A 2-dimensional vector of type @c float
    using Vec2f = Vec2<float>;
} // namespace tmt
