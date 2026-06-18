#pragma once

#include "rect.hpp"

/// @brief Provide method for @c Rect splitting
/// @details `@c Rect cut_*(@c Rect&, @c int)` methods are provided
/// to cut the given @c Rect& and return the sliced piece\n
/// Other padding and centering method are provided as well
namespace tmt::layout
{
    /// @brief Align element in layout methods
    enum class Align
    {
        TOP, BOTTOM, /// Vertical alignment
        LEFT, RIGHT, /// Horizontal alignment
        CENTER       /// Both vertical and horizontal
    };

    /// @brief Cut `amount` pixels from the top of `rect`
    inline Rect cut_top(Rect& rect, const int amount)
    {
        const auto result = Rect{ rect.x, rect.y, rect.width, amount };
        rect.y      += amount;
        rect.height -= amount;
        return result;
    }

    /// @brief Cut `amount` pixels from the bottom of `rect`
    inline Rect cut_bottom(Rect& rect, const int amount)
    {
        rect.height -= amount;
        return { rect.x, rect.y + rect.height, rect.width, amount };
    }

    /// @brief Cut `amount` pixels from the left of `rect`
    inline Rect cut_left(Rect& rect, const int amount)
    {
        const auto result = Rect{ rect.x, rect.y, amount, rect.height };
        rect.x     += amount;
        rect.width -= amount;
        return result;
    }

    /// @brief Cut `amount` pixels from the right of `rect`
    inline Rect cut_right(Rect& rect, const int amount)
    {
        rect.width -= amount;
        return { rect.x + rect.width, rect.y, amount, rect.height };
    }

    /// @brief Shrink `rect` by `amount` on all four sides
    inline Rect pad(const Rect& rect, const int amount)
    {
        return {
            rect.x + amount,
            rect.y + amount,
            rect.width  - amount*2,
            rect.height - amount*2
        };
    }

    /// @brief Shrink `rect` by independent horizontal and vertical amounts
    inline Rect pad(const Rect& rect, const int h, const int v)
    {
        return {
            rect.x + h,
            rect.y+ v,
            rect.width  - h*2,
            rect.height - v*2
        };
    }

    /// @brief Place a rectangle of size (width, height) into the middle of the container
    /// @note @c Rect.x and @c Rect.y will get rounded down if the center is between pixels
    inline Rect center_in(const Rect& container, const int width, const int height) {
        return {
            container.x + (container.width  - width)/2,
            container.y + (container.height - height)/2,
            width, height
        };
    }

    /// @brief Place a rectangle of size (size.x, size.y) into the middle of the container
    /// @note @c Rect.x and @c Rect.y will get rounded down if the center is between pixels
    inline Rect center_in(const Rect& container, const Vec2i size) {
        return Rect{ container.top_left() + (container.size() - size)/2, size };
    }

    /// @brief Align a rectangle inside a container
    /// @param container The container containing the rectangle
    /// @param size The size of the rectangle
    /// @param horizontal The horizontal alignment, takes in @c Align::LEFT, @c Align::CENTER or @c Align::RIGHT
    /// @param vertical The vertical alignment, takes in @c Align::TOP, @c Align::CENTER or @c Align::BOTTOM
    /// @note @c Rect.x and @c Rect.y will get rounded down if the true alignment is between pixels
    inline Rect align_rect(
        const Rect& container, const Vec2i size,
        const Align horizontal = Align::CENTER,
        const Align vertical = Align::CENTER
    ) {
        auto result = Rect{ container.top_left(), size };

        switch (horizontal)
        {
        case Align::LEFT:   result.x = container.x; break;
        case Align::CENTER: result.x = container.x + (container.width  - size.x)/2; break;
        case Align::RIGHT:  result.x = container.x + container.width - size.x; break;
        default: break;
        }

        switch (vertical)
        {
        case Align::TOP:    result.y = container.y; break;
        case Align::CENTER: result.y = container.y + (container.height - size.y)/2; break;
        case Align::BOTTOM: result.y = container.y + container.height - size.y; break;
        default: break;
        }

        return result;
    }

    /// @brief Align text inside a container
    /// @param container The container containing the text
    /// @param width The width of the string of text
    /// @param horizontal The horizontal alignment, takes in @c Align::LEFT, @c Align::CENTER or @c Align::RIGHT
    /// @param vertical The vertical alignment, takes in @c Align::TOP, @c Align::CENTER or @c Align::BOTTOM
    /// @returns The coordinate to print the text, rounded down to the nearest integer
    inline Vec2i align_text(
        const Rect& container, const int width,
        const Align horizontal = Align::CENTER,
        const Align vertical = Align::CENTER
    ) {
        auto result = container.top_left();

        switch (horizontal)
        {
        case Align::LEFT:   result.x = container.x; break;
        case Align::CENTER: result.x = container.x + (container.width - width)/2; break;
        case Align::RIGHT:  result.x = container.x + container.width - width; break;
        default: break;
        }

        switch (vertical)
        {
        case Align::TOP:    result.y = container.y; break;
        case Align::CENTER: result.y = container.y + (container.height - 1)/2; break;
        case Align::BOTTOM: result.y = container.y + container.height - 1; break;
        default: break;
        }

        return result;
    }
} // namespace tmt::layout