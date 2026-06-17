#pragma once

#include "../geometry/vec2.hpp"

#include <string>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <termios.h>
    #include <sys/ioctl.h>
#endif

namespace tmt::terminal
{
    /// @brief Enter raw mode, saving the original terminal state
    /// @note Must call @c restore() before exit
    void set_raw_mode();

    /// @brief Restore the terminal to its original state
    void restore();

    /// @brief Get the current terminal size
    /// @return A @c Vec2i where @c x is width and @c y is height
    [[nodiscard]] Vec2i get_size();

    // == Escape codes ==

    /// @brief Get the string that move the cursor to a certain position
    /// @note Zero-indexed
    [[nodiscard]] std::string move_cursor_str(int x, int y);
    /// @copybrief move_cursor_str
    [[nodiscard]] std::string move_cursor_str(Vec2i position);

    /// @brief Get the string that hide the cursor
    static constexpr std::string_view hide_cursor_str("\033[?25l");

    /// @brief Get the string that show the cursor
    static constexpr std::string_view show_cursor_str("\033[?25h");

    /// @brief Get the string that clear screen and move cursor to (0, 0)
    static constexpr std::string_view clear_str("\033[2J\033[H");

    /// @brief Get the string that erase the line the cursor is on
    static constexpr std::string_view clear_line_str("\033[2K");

    /// @brief Get the string that goes to the new line
    static constexpr std::string_view new_line_str = "\r\n";


    // == Terminal methods ==

    /// @brief Move the cursor to a certain position
    /// @note Zero-indexed
    void move_cursor(int x, int y);
    /// @copybrief move_cursor
    void move_cursor(Vec2i position);

    /// @brief Goes to the next line
    void new_line();

    /// @brief Hide cursor
    void hide_cursor();

    /// @brief Show cursor
    void show_cursor();

    /// @brief Clear screen and move cursor to (0, 0)
    void clear();

    /// @brief Erase the line the cursor is on
    void clear_line();

    /// @brief Flush the console
    void flush();


    // == Signal ==

    /// @brief Initialize signal handlers for window resize and process termination
    /// @note Do not call manually
    void init_signals();

    /// @brief True if a terminal resize event was recieved
    bool has_resize();

    /// @brief True if a terminal quit event (e.g. Ctrl+C) was recieved
    bool has_quit();

    /// @brief Clear the terminal resize flag, should be called after handling a resize event
    void clear_resize_flag();

    /// @brief Clear the terminal quit flag, should be called after handling a quit event
    void clear_quit_flag();


} // namespace tmt::terminal
