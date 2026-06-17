#include "../include/termite/rendering/canvas.hpp"
#include "../include/termite/rendering/terminal.hpp"

#include <iostream>


namespace tmt
{
    // == Internal UTF-8 splitting ==

    /// @brief Split a UTF-8 string into individual codepoints
    /// @param str The UTF-8 encoded string to split
    /// @return A vector of UTF8Char, one per codepoint
    static std::vector<UTF8Char> split_utf8(const std::string_view str)
    {
        std::vector<UTF8Char> result;
        size_t i = 0;

        while (i < str.size())
        {
            const UTF8Char cp(str.substr(i));
            result.push_back(cp);
            i += cp.length();
        }

        return result;
    }

    /// @brief Copy a rectangular region from one cell buffer to another
    /// @param size_src The dimensions of the source buffer
    /// @param size_dst The dimensions of the destination buffer
    /// @param src The source buffer
    /// @param dst The destination buffer
    void copy_matrix(Vec2i size_src, Vec2i size_dst, const std::vector<Cell>& src, std::vector<Cell>& dst)
    {
        for (int y = 0; y < size_src.y; ++y)
            for (int x = 0; x < size_src.x; ++x)
                if (x < size_dst.x && y < size_dst.y)
                    dst[y*size_dst.x + x] = src[y*size_src.x + x];
    }

    // == Canvas ==

    Canvas::Canvas(const int width, const int height) :
        m_size({ width, height }),
        m_front(width*height),
        m_back (width*height),
        m_needs_full_render(true)
    {}

    Canvas::Canvas(const Vec2i size) :
        m_size(size),
        m_front(size.x*size.y),
        m_back (size.x*size.y),
        m_needs_full_render(true)
    {}


    void Canvas::resize(const Vec2i new_size)
    {
        std::vector<Cell> new_front(new_size.x*new_size.y);
        std::vector<Cell> new_back (new_size.x*new_size.y);

        copy_matrix(m_size, new_size, m_front, new_front);
        copy_matrix(m_size, new_size, m_back, new_back);

        m_size = new_size;
        m_front = std::move(new_front);
        m_back  = std::move(new_back);

        m_needs_full_render = true;
    }

    Canvas::Canvas() : Canvas(terminal::get_size()) {}

    int Canvas::index(const Vec2i position) const noexcept
    {
        if (!this->in_bounds(position))
            return -1;
        return position.y*m_size.x + position.x;
    }

    bool Canvas::in_bounds(const Vec2i position) const noexcept
    {
        return position.x >= 0 && position.x < m_size.x &&
               position.y >= 0 && position.y < m_size.y;
    }

    // == Drawing ==

    void Canvas::set_cell(const size_t index, const Cell& cell) { m_back[index] = cell; }

    void Canvas::set_cell(const Vec2i position, const Cell& cell)
    {
        if (const int idx = this->index(position); idx != -1)
            m_back[idx] = cell;
    }

    void Canvas::set_char(const size_t index, const UTF8Char& chr) { m_back[index].chr = chr; }

    void Canvas::set_char(Vec2i position, const UTF8Char& chr)
    {
        if (const int idx = this->index(position); idx != -1)
            m_back[idx].chr = chr;
    }


    void Canvas::draw_text(
        Vec2i& cursor, const std::string_view text,
        const bool wrap_text,
        const std::optional<Color>& foreground,
        const std::optional<Color>& background)
    {
        if (cursor.x < 0 || cursor.y < 0)
            return;
        
        for (
            const auto codepoints = split_utf8(text);
            const auto& codepoint : codepoints
        ) {
            // Cursor exceed the canvas's width
            if (cursor.x >= m_size.x)
            {
                if (wrap_text) { cursor.x = 0; cursor.y++; }
                else break;
            }

            // Cursor exceed the canvas's height
            if (cursor.y > m_size.y)
                break;

            auto& cell = m_back[cursor.y*m_size.x + cursor.x++];

            cell.chr = codepoint;
            if (foreground.has_value())
                cell.foregr = foreground.value();
            if (background.has_value())
                cell.backgr = background.value();
        }
    }

    void Canvas::draw_text(
        const Vec2i& position, const std::string_view text,
        const bool wrap_text,
        const std::optional<Color>& foreground,
        const std::optional<Color>& background
    ) {
        auto dummy = position;
        this->draw_text(dummy, text, wrap_text, foreground, background);
    }

    void Canvas::draw_rect(Vec2i& cursor, const Vec2i size, const Cell& cell)
    {
        if (cursor.x < 0 || cursor.y < 0)
            return;

        auto end_x = std::min(cursor.x + size.x, m_size.x);
        auto end_y = std::min(cursor.y + size.y, m_size.y);

        for (int y = cursor.y; y < end_y; ++y)
            for (int x = cursor.x; x < end_x; ++x)
                m_back[y*m_size.x + x] = cell;
            
        cursor += size;
    }

    void Canvas::draw_rect(const Vec2i& position, const Vec2i size, const Cell& cell)
    {
        auto dummy = position;
        this->draw_rect(dummy, size, cell);
    }

    void Canvas::draw_rect(
        Vec2i& cursor, const Vec2i size,
        const std::optional<Color>& foreground,
        const std::optional<Color>& background
    ) {
        if (cursor.x < 0 || cursor.y < 0)
            return;

        auto end_x = std::min(cursor.x + size.x, m_size.x);
        auto end_y = std::min(cursor.y + size.y, m_size.y);

        for (int y = cursor.y; y < end_y; ++y)
            for (int x = cursor.x; x < end_x; ++x)
            {
                auto& cell = m_back[y*m_size.x + x];
                if (foreground.has_value())
                    cell.foregr = foreground.value();
                if (background.has_value())
                    cell.backgr = background.value();
            }
        
        cursor += size;
    }

    void Canvas::draw_rect(
        const Vec2i& position, const Vec2i size,
        const std::optional<Color>& foreground,
        const std::optional<Color>& background
    ) {
        auto dummy = position;
        this->draw_rect(dummy, size, foreground, background);
    }


    void Canvas::draw_rect(const Rect& rect, const Cell& cell)
    {
        auto dummy = Vec2i{ rect.x, rect.y };
        this->draw_rect(dummy, {rect.width, rect.height}, cell);
    }

    void Canvas::draw_rect(
        const Rect& rect,
        const std::optional<Color>& foreground,
        const std::optional<Color>& background
    ) {
        auto dummy = Vec2i{ rect.x, rect.y };
        this->draw_rect(dummy, {rect.width, rect.height}, foreground, background);
    }

    /*
    void Canvas::draw_border(const Vec2i pos, const Vec2i size,
                             const Color& fg, const Color& bg)
    {
        const int x0 = pos.x;
        const int y0 = pos.y;
        const int x1 = pos.x + size.x - 1;
        const int y1 = pos.y + size.y - 1;

        // corners
        set_cell({ x0, y0 }, Cell("┌", fg, bg));
        set_cell({ x1, y0 }, Cell("┐", fg, bg));
        set_cell({ x0, y1 }, Cell("└", fg, bg));
        set_cell({ x1, y1 }, Cell("┘", fg, bg));

        // horizontal edges
        for (int x = x0 + 1; x < x1; x++)
        {
            set_cell({ x, y0 }, Cell("─", fg, bg));
            set_cell({ x, y1 }, Cell("─", fg, bg));
        }

        // vertical edges
        for (int y = y0 + 1; y < y1; y++)
        {
            set_cell({ x0, y }, Cell("│", fg, bg));
            set_cell({ x1, y }, Cell("│", fg, bg));
        }
    }
*/
    void Canvas::fill(const Cell& cell) { std::fill(m_back.begin(), m_back.end(), cell); }

    void Canvas::clear      (const Cell& cell) { std::fill(m_back.begin(), m_back.end(), cell); }

    void Canvas::clear(
        const std::optional<Color>& foreground,
        const std::optional<Color>& background
    )
    {
        if (!foreground.has_value() && !background.has_value())
            return;

        for (auto& cell : m_back)
        {
            if (foreground.has_value())
                cell.foregr = foreground.value();
            if (background.has_value())
                cell.backgr = background.value();
        }
    }

    // == Rendering ==

    std::string Canvas::render_full_str() const
    {
        std::string output;
        output.reserve(m_size.x*m_size.y*16); // rough estimate per cell

        output += terminal::clear_str;

        for (int y = 0; y < m_size.y; ++y)
        {
            for (int x = 0; x < m_size.x; ++x)
                output += m_back[y*m_size.x + x].escape_code();

                output += terminal::new_line_str;
        }

        return output;
    }

    std::string Canvas::render_diff_str() const
    {
        std::string output;
        output.reserve(m_size.x*m_size.y*16); // rough estimate per cell

        for (int y = 0; y < m_size.y; ++y)
            for (int x = 0; x < m_size.x; ++x)
            {
                const int idx = y*m_size.x + x;

                // Skip cells that haven't changed
                if (m_back[idx] == m_front[idx])
                    continue;

                // Move cursor to this cell and emit its escape codes
                output += terminal::move_cursor_str(x, y) + m_back[idx].escape_code();
            }

        return output;
    }


    void Canvas::flush()
    {
        std::cout << (m_needs_full_render? render_full_str() : render_diff_str());
        m_needs_full_render = false;
        m_front = m_back;
        // std::fill(m_back.begin(), m_back.end(), Cell());
        terminal::flush();

        // std::cout << render_full_str();
        // m_front = m_back;
        // terminal::flush();
    }

} // namespace tmt