#pragma once

#include "cell.hpp"
#include "../geometry/vec2.hpp"
#include "../geometry/rect.hpp"

#include <optional>
#include <vector>

namespace tmt
{
    /// @brief A double-buffered grid of @c Cell objects that render to the terminal
    /// @details The canvas include two grids, one is a front buffer representing
    /// what is currently visible on screen, while the other is what we draw into.
    /// When @c Canvas::flush() is called, only cells that differ from the two buffer
    /// is rendered to stdout, minimizing terminal in-out and flickering.\n
    /// Coordinate system in zero-indexed with x represent width and y height.
    class Canvas
    {
    public:
        /// @brief Construct a @c Canvas with the given width and height
        Canvas(int width, int height);
        /// @copybrief Canvas
        explicit Canvas(Vec2i size);

        /// @brief Construct a @c Canvas using the current terminal dimensions
        Canvas();

        /// @brief Resizes the canvas
        void resize(Vec2i new_size);

        // == Drawing ==

        /// @brief Set a single cell in the back buffer
        /// @param position The position of the target cell
        /// @param cell The reference @c Cell
        void set_cell(Vec2i position, const Cell& cell);

        /// @brief Set a single char in a cell in the back buffer
        /// @param position The position of the target cell
        /// @param chr The character
        void set_char(Vec2i position, const UTF8Char& chr);

        /// @brief Draw text to the back of the buffer
        /// @details Moves the cursor when finish drawing
        /// @param cursor The position of the first character
        /// @param text The string of text to draw
        /// @param wrap_text Wrap text to new line when overflowing
        /// @param foreground Text color
        /// @param background Background color
        void draw_text(Vec2i& cursor, std::string_view text,
            bool wrap_text = false,
            const std::optional<Color>& foreground = std::nullopt,
            const std::optional<Color>& background = std::nullopt
        );

        /// @brief Draw text to the back of the buffer
        /// @param position The position of the first character
        /// @param text The string of text to draw
        /// @param wrap_text Wrap text to new line when overflowing
        /// @param foreground Text color
        /// @param background Background color
        void draw_text(const Vec2i& position, std::string_view text,
            bool wrap_text = false,
            const std::optional<Color>& foreground = std::nullopt,
            const std::optional<Color>& background = std::nullopt
        );

        /// @details Moves the cursor when finish drawing
        /// @param cursor Top left corner of the rectangle
        /// @param size Width @c x and height @c y of the rectangle
        /// @param cell The reference @c Cell
        void draw_rect(Vec2i& cursor, Vec2i size, const Cell& cell);

        /// @brief Fill a rectangular region with a given cell
        /// @param position Top left corner of the rectangle
        /// @param size Width @c x and height @c y of the rectangle
        /// @param cell The reference @c Cell
        void draw_rect(const Vec2i& position, Vec2i size, const Cell& cell);

        /// @brief Fill a rectangular region with colors and avoid modifying text
        /// @param cursor Top left corner of the rectangle
        /// @param size Width @c x and height @c y of the rectangle
        /// @param foreground Text color
        /// @param background Background color
        void draw_rect(
            Vec2i& cursor, Vec2i size,
            const std::optional<Color>& foreground,
            const std::optional<Color>& background
        );

        /// @brief Fill a rectangular region with colors and avoid modifying text
        /// @param position Top left corner of the rectangle
        /// @param size Width @c x and height @c y of the rectangle
        /// @param foreground Text color
        /// @param background Background color
        void draw_rect(
            const Vec2i& position, Vec2i size,
            const std::optional<Color>& foreground,
            const std::optional<Color>& background
        );


        /// @brief Fill a rectangular region with a given cell
        /// @param rect The rectangle to fill
        /// @param cell The reference @c Cell
        void draw_rect(const Rect& rect, const Cell& cell);

        /// @brief Fill a rectangular region with colors and avoid modifying text
        /// @param rect The rectangle to fill
        /// @param foreground Text color
        /// @param background Background color
        void draw_rect(
            const Rect& rect,
            const std::optional<Color>& foreground,
            const std::optional<Color>& background
        );

        // @brief Draw a border around a rectangular region
        // @param position Top left corner of the rectangle
        // @param size Width @c x and height @c y of the rectangle
        // @param border_type Type of the border where:\n
        //  + @c 0: No character\n
        //  + @c 1: Single line border using Unicode box-drawing character\n
        //  + @c 2: Double line border using Unicode box-drawing character\n
        //  + any other value will default to @c 0
        // @param foreground Border color
        // @param background Border background color
        // void draw_border(
        //     Vec2i position, Vec2i size, int border_type = 1,
        //     const std::optional<Color>& foreground = std::nullopt,
        //     const std::optional<Color>& background = std::nullopt
        // );



        /// @brief Fill this @c Canvas with a certain cell
        /// @param cell The reference cell
        /// @note Call this (or @c Canvas::clear) before each
        /// frame draws to ensure no artifact between frames,
        /// the @c Cell::chr is preferably a white-space
        void fill(const Cell& cell);

        /// @brief Clear the back buffer
        /// @details Identical to @c Canvas::fill
        /// @param cell the cell to fill with, default to @c Cell()
        void clear(const Cell& cell = Cell());

        /// @brief Clear the back buffer with foreground and background color
        /// @param foreground Text color
        /// @param background Background color
        void clear(
            const std::optional<Color>& foreground,
            const std::optional<Color>& background
        );

        // == Rendering ==

        /// @brief Flush the back buffer to the terminal
        /// @details
        /// Compares the back buffer against the front buffer cell by cell.
        /// Only cells that changed are written to stdout, batched into a single
        /// write() call to minimize syscall overhead.
        /// After flushing, the front buffer is updated to match the back buffer.
        void flush();

        // == Accessor ==

        /// @brief Get the canvas size
        [[nodiscard]] Vec2i size() const noexcept { return m_size; }

        /// @brief Get the canvas width in cells
        [[nodiscard]] int width()  const noexcept { return m_size.x; }

        /// @brief Get the canvas height in cells
        [[nodiscard]] int height() const noexcept { return m_size.y; }

        /// @brief Force the canvas to re-render the entire back buffer
        void force_full_render() { m_needs_full_render = true; }


    private:
        /// Canvas size
        Vec2i m_size;

        /// Front buffer currently visible on screen
        std::vector<Cell> m_front;

        /// Back buffer being drawn to in this frame
        std::vector<Cell> m_back;

        /// True if the entire back buffer needs to be rendered
        bool m_needs_full_render = true; 

        /// @brief Convert a 2D position to a flat buffer index
        /// @param position The position to convert
        /// @return The flat index, or @c -1 if out of bounds
        [[nodiscard]] int index(Vec2i position) const noexcept;

        /// @brief True if the given position is within the canvas bounds
        [[nodiscard]] bool in_bounds(Vec2i position) const noexcept;

        /// @brief Set a single cell in the back buffer
        /// @param index The position of the target cell
        /// @param cell The reference @c Cell
        void set_cell(size_t index, const Cell& cell);

        /// @brief Set a single char in a cell in the back buffer
        /// @param index The position of the target cell
        /// @param chr The character
        void set_char(size_t index, const UTF8Char& chr);

        // == Rendering ==

        /// @brief Render the entire back buffer into a single string
        /// @details This is used when rendering the first frame or when the screen is resized
        [[nodiscard]] std::string render_full_str() const;

        /// @brief Render only the cells that differ between the back and front buffer into a single string
        /// @details This is used to minimize the amount of data written to stdout during each frame
        [[nodiscard]] std::string render_diff_str() const;
    };
}