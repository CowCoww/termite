#pragma once

#include "color.hpp"
#include "../char/utf8char.hpp"

namespace tmt
{
    struct Cell
    {
        UTF8Char chr; /// UTF-8 encoded character to display

        Color foregr = color::WHITE; /// Foreground color
        Color backgr = color::BLACK; /// Background color

        constexpr Cell() = default;

        explicit  constexpr Cell(
            const UTF8Char& character,
            const Color& foreground = color::WHITE,
            const Color& background = color::BLACK
        ) :
            chr(character),
            foregr(foreground),
            backgr(background)
        {}

        explicit constexpr Cell(
            const char character,
            const Color& foreground = color::WHITE,
            const Color& background = color::BLACK
        ) :
            chr(character),
            foregr(foreground),
            backgr(background)
        {}

        explicit constexpr Cell(
            const std::string_view character,
            const Color& foreground = color::WHITE,
            const Color& background = color::BLACK
        ) :
            chr(character),
            foregr(foreground),
            backgr(background)
        {}

        /// @brief Get the escape code representation for this cell
        [[nodiscard]] std::string escape_code() const
        {
            return foregr.fore_code() + backgr.back_code() + chr.to_string() + "\033[0m";
        }

        /// @brief Equality
        [[nodiscard]] bool operator==(const Cell& other) const
        {
            return
                this->chr == other.chr &&
                this->foregr == other.foregr &&
                this->backgr == other.backgr;
        }

        /// @brief Inequality
        [[nodiscard]] bool operator!=(const Cell& other) const
        {
            return !(*this == other);
        }

        /// @brief Copy assignment
        Cell& operator=(const Cell& other)
        {
            if (this == &other)
                return *this;

            this->chr = other.chr;
            this->foregr = other.foregr;
            this->backgr = other.backgr;
            return *this;
        }
    };
} // namespace tmt