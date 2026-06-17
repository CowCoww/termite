#pragma once

/// @brief Handle input characters, keybinds,...
namespace tmt::key
{
    /// @brief Key identity, is either a Unicode codepoint or a special key constant
    /// @details Printable characters map directly to their codepoint value\n
    /// Special keys (arrows, function keys, etc.) use constants above @c 255 to avoid
    /// clashing with any real codepoint\n Ctrl combos are computed via @c key::ctrl()
    using Key = int;

    /// @brief Case and sensitivity flags
    enum class Case : unsigned char
    {
        NONE  = 0, /// Character that have no case
        LOWER = 1, /// Lowercase representation
        UPPER = 2, /// Uppercase representation
        SENSITIVE, /// Case-sensitive
        IGNORE     /// Ignore case
    };

    // == Special ASCII keys ==
    constexpr Key None      =  0;
    constexpr Key Tab       =  9;
    constexpr Key Enter     = 13;
    constexpr Key Escape    = 27;
    constexpr Key Backspace = 127;

    // == Special keys ==
    constexpr Key Up        = 256;
    constexpr Key Down      = 257;
    constexpr Key Left      = 258;
    constexpr Key Right     = 259;

    constexpr Key CtrlUp    = 260;
    constexpr Key CtrlDown  = 261;
    constexpr Key CtrlLeft  = 262;
    constexpr Key CtrlRight = 263;

    constexpr Key Home      = 264;
    constexpr Key End       = 265;
    constexpr Key PageUp    = 266;
    constexpr Key PageDown  = 267;
    constexpr Key Insert    = 268;
    constexpr Key Delete    = 269;

    constexpr Key F1        = 270;
    constexpr Key F2        = 271;
    constexpr Key F3        = 272;
    constexpr Key F4        = 273;
    constexpr Key F5        = 274;
    constexpr Key F6        = 275;
    constexpr Key F7        = 276;
    constexpr Key F8        = 277;
    constexpr Key F9        = 278;
    constexpr Key F10       = 279;
    constexpr Key F11       = 280;
    constexpr Key F12       = 281;

    constexpr Key Unknown   = -1;

    // == Methods ==

    /// @brief Compute the key value for a Ctrl+key command
    /// @details
    /// In ASCII, control characters occupy @c 0x00-0x1F, holding @c Ctrl while
    /// pressing a letter clears bits @c 6 and @c 7 of the character's codepoint
    /// @param chr The letter key (case-insensitive)
    /// @return The Key value for that @c Ctrl combo
    [[nodiscard]] constexpr Key ctrl(const char chr) noexcept
    {
        return static_cast<unsigned char>(chr) & 0x1F;
    }

    /// @brief Determine which case a character is
    /// @return @c Case::LOWER for lowercase characters,
    /// @c Case::UPPER for uppercase characters,
    /// @c Case::NONE for other characters
    constexpr Case which_case(const Key key)
    {
        if ('a' <= key && key <= 'z') return Case::LOWER;
        if ('A' <= key && key <= 'Z') return Case::UPPER;

        return Case::NONE;
    }

    /// @brief Convert a key to uppercase
    constexpr Key to_upper(const Key key)
    {
        if ('a' <= key && key <= 'z')
            return key - 'a' + 'A';
        return key;
    }

    /// @brief Convert a key to lowercase
    constexpr Key to_lower(const Key key)
    {
        if ('A' <= key && key <= 'Z')
            return key - 'A' + 'a';
        return key;
    }

    /// @brief Convert a character into a certain key
    /// @details Works as intended with @c Case::LOWER and @c Case::UPPER,
    /// if target_case = @c Case::SENSITIVE it returns the original input,
    /// and Case::IGNORE or CASE::NONE converts the key to uppercase
    constexpr Key to_case(const Key key, const Case target_case)
    {
        switch (target_case)
        {
        case Case::UPPER:
        case Case::IGNORE:
        case Case::NONE:
            if ('a' <= key && key <= 'z')
                return key - 'a' + 'A';
            return key;

        case Case::LOWER:
            if ('A' <= key && key <= 'Z')
                return key - 'A' + 'a';
            return key;

        case Case::SENSITIVE:
        default:
            return key;
        }
    }

    /// @brief True if the key is a printable ASCII character
    [[nodiscard]] constexpr bool is_char(const Key k) noexcept
    {
        return k >= 0x20 && k <= 0x7E;
    }

    /// @brief True if the key is a digit between [@c 0, @c 9]
    constexpr bool is_digit(const Key key) { return '0' <= key && key <= '9'; }

    /// @brief True if the key is a letter between [@c 'a',@c 'z'] or [@c 'A',@c 'Z']
    constexpr bool is_letter(const Key key)
    {
        return ('a' <= key && key <= 'z') || ('A' <= key && key <= 'Z');
    }

    /// @brief True if the key is alphanumeric (@c is_digit() or @c is_letter())
    constexpr bool is_alphanum(const Key key)
    {
        return
            ('a' <= key && key <= 'z') ||
            ('A' <= key && key <= 'Z') ||
            ('0' <= key && key <= '9');
    }
} // namespace tmt::key