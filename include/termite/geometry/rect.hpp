#pragma once

#include "vec2.hpp"

namespace tmt
{
    /// @brief Represent a rectangle
    /// @details Hold 4 integers, where @c x and @c y are coordinates
    /// on screen, @c width and @c height are dimensions of the rectangle
    struct Rect
    {
        int x, y, width, height;
        
        /// @brief Zero-initialize all fields
        constexpr Rect() : x(0), y(0), width(0), height(0) {}

        constexpr Rect(const int x, const int y, const int width, const int height) :
            x(x), y(y), width(width), height(height)
        {}

        constexpr Rect(const Vec2i position, const Vec2i size) :
            x(position.x), y(position.y),
            width(size.x), height(size.y)
        {}
        
        /// @brief Set the @c x and @c y coordinate of this @c Rect
        constexpr void set_position(const int _x, const int _y) { this->x = _x; this->y = _y; }
        /// @copybrief set_position
        constexpr void set_position(const Vec2i position) { this->x = position.x; this->y = position.y; }
        
        /// @brief Set the @c width and @c height of this Rect
        constexpr void set_size(const int _width, const int _height) { this->width = _width; this->height = _height; }
        /// @copybrief set_size
        constexpr void set_size(const Vec2i size) { this->width = size.x; this->height = size.y; }

        /// @brief Get the position of this @c Rect
        [[nodiscard]] constexpr Vec2i position() const { return Vec2i{ x, y }; }
        
        /// @brief Get the top-left corner of this @c Rect
        [[nodiscard]] constexpr Vec2i top_left() const { return Vec2i{ x, y }; }

        /// @brief Get the top-left corner of this @c Rect
        [[nodiscard]] constexpr Vec2i bottom_right() const { return Vec2i{ x + width, y + height }; }

        /// @brief Get the size of this @c Rect
        [[nodiscard]] constexpr Vec2i size() const { return Vec2i{ width, height }; }

        /// @brief True if @c Vec2i is inside or on the perimeter of this @c Rect
        [[nodiscard]] constexpr bool contains(const Vec2i point) const
        {
            return
                (this->x <= point.x) &&
                (this->y <= point.y) &&
                (this->x + this->width >= point.x) &&
                (this->y + this->height >= point.y);
        }
        
        /// @brief True if this @c Rect contains another @c Rect
        /// @note The edges touching would still evaluate to true
        [[nodiscard]] constexpr bool contains(const Rect rect) const
        {
            return
                (this->x <= rect.x) &&
                (this->y <= rect.y) &&
                (this->x + this->width >= rect.x + rect.width) &&
                (this->y + this->height >= rect.y + rect.height);
        }
        
        /// @brief True if two @c Rect objects have an intersection
        /// @details @p A and @p B collide if @c A contains the top-left
        /// of @c B but not the bottom-right and vice versa
        static constexpr bool collided(const Rect& A, const Rect& B)
        {
            return A.contains(B.top_left()) ^ A.contains(B.bottom_right());
        }
    };
}
