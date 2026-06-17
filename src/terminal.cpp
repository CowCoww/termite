#include "../include/termite/rendering/terminal.hpp"
#include "../include/termite/geometry/vec2.hpp"

#include <iostream>
#include <csignal>

#ifdef _WIN32
    static DWORD s_original_in;
    static DWORD s_original_out;
    static tmt::Vec2i s_original_size;
#else
    #include <unistd.h>
    static ::termios s_original;
#endif

// == Signal flags ==
static volatile std::sig_atomic_t s_resize_flag = 0;
static volatile std::sig_atomic_t s_interrupt_flag = 0;

#ifdef _WIN32
    static BOOL WINAPI on_ctrl_event(const DWORD event)
    {
        if (event == CTRL_C_EVENT || event == CTRL_CLOSE_EVENT)
        {
            s_interrupt_flag = 1;
            return TRUE;
        }
        return FALSE;
    }
#else
    static void on_sigwinch(int) { s_resize_flag = 1; }
    static void on_sigint(int)   { s_interrupt_flag = 1; }
    static void on_sigterm(int)  { s_interrupt_flag = 1; }
#endif






namespace tmt::terminal
{

    static constexpr std::string_view enter_alt_screen_str = "\033[?1049h";
    static constexpr std::string_view leave_alt_screen_str  = "\033[?1049l";

    void enter_alt_screen() { std::cout << enter_alt_screen_str; }
    void leave_alt_screen() { std::cout << leave_alt_screen_str; }


    void set_raw_mode()
    {
        enter_alt_screen();
#ifdef _WIN32
        const HANDLE h_in = GetStdHandle(STD_INPUT_HANDLE);
        const HANDLE h_out = GetStdHandle(STD_OUTPUT_HANDLE);

        GetConsoleMode(h_in, &s_original_in);
        GetConsoleMode(h_out, &s_original_out);

        // Raw input, no line buffering, no echo, no processed input
        SetConsoleMode(h_in, ENABLE_VIRTUAL_TERMINAL_INPUT);

        // Enable ANSI escape processing on output side
        SetConsoleMode(h_out, s_original_out | ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN);

        // Set UTF-8 input and output
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);

#else
        ::tcgetattr(STDIN_FILENO, &s_original);
        ::termios raw = s_original;

        // - Input flags -
        // IXON   : disable Ctrl+S / Ctrl+Q software flow control
        // ICRNL  : disable carriage return to newline translation
        // BRKINT : disable break condition sending SIGINT
        // INPCK  : disable parity checking (legacy, rarely relevant)
        // ISTRIP : disable stripping of 8th bit (legacy)
        raw.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP);

        // - Output flags -
        // OPOST : disable all output processing (notably \n → \r\n translation)
        raw.c_oflag &= ~(OPOST);

        // - Control flags -
        // CS8 : set character size to 8 bits per byte
        raw.c_cflag |= (CS8);

        // - Local flags -
        // ECHO   : disable echoing typed characters back to the terminal
        // ICANON : disable canonical mode, read byte by byte instead of line by line
        // ISIG   : disable Ctrl+C / Ctrl+Z signal generation
        // IEXTEN : disable Ctrl+V implementation-defined extensions
        raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);

        // VMIN  = 0 : read() returns immediately even if no bytes available
        // VTIME = 1 :read() waits up to 100ms before returning empty
        // together this gives a non-blocking read with a natural ~100ms heartbeat
        // which is exactly the cadence the app loop needs for onUpdate()
        raw.c_cc[VMIN]  = 0;
        raw.c_cc[VTIME] = 1;

        ::tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
#endif
    }

    void restore()
    {
        leave_alt_screen();
#ifdef _WIN32
        SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE),  s_original_in);
        SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), s_original_out);
#else
        ::tcsetattr(STDIN_FILENO, TCSAFLUSH, &s_original);
#endif
    }

    [[nodiscard]] Vec2i get_size()
    {
#ifdef _WIN32
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
        return {
            csbi.srWindow.Right  - csbi.srWindow.Left + 1,
            csbi.srWindow.Bottom - csbi.srWindow.Top  + 1
        };
#else
        ::winsize ws;
        ::ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
        return { ws.ws_col, ws.ws_row };
#endif
    }


    [[nodiscard]] std::string move_cursor_str(const int x, const int y)
    {
        return std::string("\033[") + std::to_string(y + 1) + ";" + std::to_string(x + 1) + "H";
    }
    [[nodiscard]] std::string move_cursor_str(const Vec2i position)
    {
        return move_cursor_str(position.x, position.y);
    }

    void move_cursor(const int x, const int y)
    {
        std::cout << move_cursor_str(x, y);
    }
    void move_cursor(const Vec2i position)
    {
        std::cout << move_cursor_str(position.x, position.y);
    }
    void new_line()    { std::cout << new_line_str; }
    void hide_cursor() { std::cout << hide_cursor_str; }
    void show_cursor() { std::cout << show_cursor_str; }
    void clear()       { std::cout << clear_str; };
    void clear_line()  { std::cout << clear_line_str; };
    void flush()       { std::cout.flush(); };

    // == Signal ==
    void init_signals()
    {
#ifdef _WIN32
        SetConsoleCtrlHandler(on_ctrl_event, TRUE);
        s_last_size = get_size();
#else
        std::signal(SIGWINCH, on_sigwinch);
        std::signal(SIGINT,   on_sigint);
        std::signal(SIGTERM,  on_sigterm);
#endif
    }

    bool has_resize()
    {
#ifdef _WIN32
        const Vec2i current = get_size();
        if (current != s_last_size)
        {
            s_last_size = current;
            s_resize_flag = 1;
        }
#endif
        return s_resize_flag == 1;
    }

    bool has_quit() { return s_interrupt_flag == 1; }
    void clear_resize_flag() { s_resize_flag    = 0; }
    void clear_quit_flag()   { s_interrupt_flag = 0; }
}