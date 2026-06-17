#pragma once

#include <string>

namespace tmt
{
    struct Color
    {
        unsigned char r, g, b;

        /// @brief Get the escape code for this @c Color as a foreground color
        [[nodiscard]] std::string fore_code() const
        {
            return "\033[38;2;" +
                std::to_string(r) + ";" +
                std::to_string(g) + ";" +
                std::to_string(b) + "m";
        }

        /// @brief Get the escape code for this @c Color as a background color
        [[nodiscard]] std::string back_code() const
        {
            return "\033[48;2;" +
                std::to_string(r) + ";" +
                std::to_string(g) + ";" +
                std::to_string(b) + "m";
        }

        /// @brief Exact equality of channels
        [[nodiscard]] constexpr bool operator==(const Color& other) const
        {
            return this->r == other.r && this->g == other.g && this->b == other.b;
        }

        /// @brief Inequality
        [[nodiscard]] constexpr bool operator!=(const Color& other) const
        {
            return this->r != other.r || this->g != other.g || this->b != other.b;
        }
    };

    /// @brief Predefined colors
    namespace color
    {
        constexpr Color WHITE      = {255, 255, 255};
        constexpr Color BLACK      = {0,   0,   0  };
        constexpr Color GRAY       = {100, 100, 100};
        constexpr Color DARK_GRAY  = {30,  30,  30 };

        constexpr Color RED        = {250, 54,  47 };
        constexpr Color GREEN      = {40,  117, 39 };
        constexpr Color BLUE       = {47,  98,  250};
        constexpr Color YELLOW     = {186, 166, 54 };
        constexpr Color CYAN       = {54,  182, 186};
        constexpr Color MAGENTA    = {128, 56,  124};

        constexpr Color ORANGE     = {232, 134, 30 };
        constexpr Color PURPLE     = {81,  37,  122};
        constexpr Color PINK       = {255, 59,  127};
        constexpr Color LIGHT_BLUE = {93,  193, 240};
    } // namespace color
} // namespace tmt