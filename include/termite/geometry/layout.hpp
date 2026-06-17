#pragma once

#include "rect.hpp"

/// @brief Provide method for @c Rect splitting
/// @details `@c Rect cut_*(@c Rect&, @c int)` methods are provided
/// to cut the given @c Rect& and return the sliced piece\n
/// Other padding and centering method are provided as well
namespace tmt::layout
{
    /// @brief Cut `amount` pixels from the top of `rect`
    inline Rect cut_top(Rect& rect, const int amount)
    {
        const auto result = Rect{ rect.x, rect.y, rect.width, amount };
        rect.y      += amount;
        rect.height -= amount;
        return result;
    }

    /// @brief Cut `amount` pixels from the bottom of `rect`
    inline Rect cut_bottom(Rect& rect, const int amount) {
        rect.height -= amount;
        return { rect.x, rect.y + rect.height, rect.width, amount };
    }

    /// @brief Cut `amount` pixels from the left of `rect`
    inline Rect cut_left(Rect& rect, const int amount) {
        const auto result = Rect{ rect.x, rect.y, amount, rect.height };
        rect.x     += amount;
        rect.width -= amount;
        return result;
    }

    /// @brief Cut `amount` pixels from the right of `rect`
    inline Rect cut_right(Rect& rect, const int amount) {
        rect.width -= amount;
        return { rect.x + rect.width, rect.y, amount, rect.height };
    }

    /// @brief Shrink `rect` by `amount` on all four sides
    inline Rect pad(const Rect& rect, const int amount) {
        return {
            rect.x + amount,
            rect.y + amount,
            rect.width  - amount*2,
            rect.height - amount*2
        };
    }

    /// @brief Shrink `rect` by independent horizontal and vertical amounts
    inline Rect pad(const Rect& rect, const int h, const int v) {
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
} // namespace tmt::layout