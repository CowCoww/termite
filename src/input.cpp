#include "../include/termite/input.hpp"

#ifdef _WIN32
#else

#include <unistd.h>

namespace tmt
{
    /// @brief Read available bytes from stdin into a buffer
    /// @return Number of bytes read, or 0 if nothing available within VTIME window
    static int read_raw(char* buf, const int buf_size)
    {
        const int n = ::read(STDIN_FILENO, buf, buf_size);
        return n > 0 ? n : 0;
    }

    /// @brief Parse a raw byte buffer into a @c Key value
    /// @details
    /// Terminal keypresses arrive as raw byte sequences defined by the VT100/VT220
    /// standards with structure:\n
    /// - Single byte:\n
    ///     0x00 - 0x1A  : Ctrl+key (ASCII control characters)\n
    ///     0x1B         : Escape, or start of escape sequence\n
    ///     0x20 - 0x7E  : printable ASCII, maps directly to codepoint\n
    ///     0x7F         : Backspace (DEL character)\n
    ///
    /// - Escape sequences (multibyte, always start with 0x1B):\n
    ///     CSI  \033[... : arrows, function keys, navigation keys\n
    ///     SS3  \033O... : F1-F4 on some terminals (notably older xterm)\n
    ///
    /// @param buf The raw byte buffer
    /// @param len Number of bytes in the buffer
    /// @return The corresponding Key value
    static key::Key parse_sequence(const char* buf, const int len)
    {
        if (len == 0)
            return key::None;

        const auto b = static_cast<unsigned char>(buf[0]);

        // == Single byte ==
        if (len == 1)
        {
            if (b == 0x1B) return key::Escape;
            if (b == 0x0D) return key::Enter;
            if (b == 0x7F) return key::Backspace;
            if (b == 0x09) return key::Tab;

            // Ctrl+key, 0x01 through 0x1A
            if (b >= 0x01 && b <= 0x1A)
                return key::ctrl(static_cast<char>(b + 0x60));

            // printable ASCII
            if (b >= 0x20 && b <= 0x7E)
                return b;

            return key::Unknown;
        }

        // == Escape sequences ==
        if (buf[0] != '\033')
            return key::Unknown;

        // CSI sequences - \033[...
        if (buf[1] == '[')
        {
            if (len == 3) // Basic arrows and navigation : \033[A through \033[F
                switch (buf[2])
                {
                case 'A': return key::Up;
                case 'B': return key::Down;
                case 'C': return key::Right;
                case 'D': return key::Left;
                case 'H': return key::Home;
                case 'F': return key::End;
                default: return key::Unknown;
                }

            if (len == 4 && buf[3] == '~') // Extended navigation : \033[n~
                switch (buf[2])
                {
                case '2': return key::Insert;
                case '3': return key::Delete;
                case '5': return key::PageUp;
                case '6': return key::PageDown;
                default: return key::Unknown;
                }

            if (len == 6 && buf[2] == '1' && buf[3] == ';' && buf[4] == '5') // Ctrl+Arrow : \033[1;5A etc.
                switch (buf[5])
                {
                case 'A': return key::CtrlUp;
                case 'B': return key::CtrlDown;
                case 'C': return key::CtrlRight;
                case 'D': return key::CtrlLeft;
                default: return key::Unknown;
                }

            if (len == 5 && buf[3] == '1' && buf[4] == '~') // F1-F4 via CSI : \033[11~ through \033[14~
                switch (buf[2])
                {
                case '1': return key::F1;
                case '2': return key::F2;
                case '3': return key::F3;
                case '4': return key::F4;
                default: return key::Unknown;
                }

            // F5-F12 via CSI : \033[15~ through \033[24~
            // 16, 22, 25 are unassigned in VT220
            if (len == 5 && buf[4] == '~')
                switch ((buf[2] - '0') * 10 + (buf[3] - '0'))
                {
                case 15: return key::F5;
                case 17: return key::F6;
                case 18: return key::F7;
                case 19: return key::F8;
                case 20: return key::F9;
                case 21: return key::F10;
                case 23: return key::F11;
                case 24: return key::F12;
                default: return key::Unknown;
                }
        }

        if (buf[1] == 'O' && len == 3) // SS3 sequences : \033O... (F1-F4 on older xterm and some other terminals)
            switch (buf[2])
            {
            case 'P': return key::F1;
            case 'Q': return key::F2;
            case 'R': return key::F3;
            case 'S': return key::F4;
            default: return key::Unknown;
            }

        return key::Unknown;
    }

    void InputState::poll()
    {
        // Keys that were pressed last frame are now candidates for held or released
        // Anything in m_pressed that doesn't appear again this frame goes to m_released
        m_released.clear();
        for (const Key k : m_pressed)
        {
            if (!m_held.contains(k))
                m_held.insert(k);
        }

        m_pressed.clear();

        // Read all available bytes, loop until stdin is drained
        char buf[32];
        const int n = read_raw(buf, sizeof(buf));

        if (n == 0)
        {
            // Nothing typed this frame, any held keys are now released
            for (const Key k : m_held)
                m_released.insert(k);

            m_held.clear();
            m_last_pressed = key::None;
            return;
        }

        if (const Key k = parse_sequence(buf, n);
            k != key::None && k != key::Unknown)
        {
            m_pressed.insert(k);
            m_last_pressed = k;

            // Release the key that was held but not seen last frame
            if (!m_held.contains(k))
            {
                for (const Key held : m_held)
                    if (held != k)
                        m_released.insert(held);

                m_held.clear();
            }
        }
    }

    bool InputState::contain_in(const KeySet& set, const Key key, const Case sensitivity)
    {
        switch (sensitivity)
        {
        case Case::SENSITIVE:
            return set.contains(key);

        case Case::IGNORE:
        case Case::NONE:
            if (key::is_letter(key))
                return
                    set.contains(key::to_lower(key)) ||
                    set.contains(key::to_upper(key));
            return set.contains(key);

        case Case::UPPER:
        case Case::LOWER:
        default:
            return set.contains(key::to_case(key, sensitivity));
        }
    }

    bool InputState::is_pressed(const Key key, const Case sensitivity) const
    {
        return contain_in(m_pressed, key, sensitivity);
    }

    bool InputState::is_held(const Key key, const Case sensitivity) const
    {
        return contain_in(m_held, key, sensitivity) || contain_in(m_pressed, key, sensitivity);
    }

    bool InputState::is_released(const Key key, const Case sensitivity) const
    {
        return contain_in(m_released, key, sensitivity);
    }
} // namespace tmt

#endif