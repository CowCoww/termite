#pragma once

#include <cstring>
#include <string>
#include <string_view>

namespace tmt
{
    using uchar = unsigned char;

    /// @brief Represent a single UTF-8 encoded codepoint
    /// @details UTF-8 is a variable-width encoding, each codepoint is 1 to 4 bytes.
    /// The leading byte encodes the length via its high bits:
    ///   0xxxxxxx -> 1 byte (ASCII)\n
    ///   110xxxxx -> 2 bytes\n
    ///   1110xxxx -> 3 bytes\n
    ///   11110xxx -> 4 bytes\n
    /// Continuation bytes all have the form 10xxxxxx, which is how the decoder
    /// can always resync to the start of a codepoint from any position.
    struct UTF8Char
    {
    private:
        char m_bytes[4] = {' ', 0, 0, 0};

    public:

        /// @brief Initialize to a white space character
        constexpr UTF8Char() = default;

        /// @brief Construct from a single ASCII @c char
        explicit constexpr UTF8Char(const char c) : m_bytes {c, 0, 0, 0} {}

        /// @brief Construct from a @c std::string_view
        /// @details
        explicit constexpr UTF8Char(const std::string_view str)
        {
            if (str.empty())
                return;

            m_bytes[0] = static_cast<char>(str[0]);
            const auto len = this->length();

            for (uchar idx = 1; idx < len; ++idx)
                m_bytes[idx] = str[idx];
        }

        /// @brief Get the byte length of this codepoint from the leading byte
        [[nodiscard]] constexpr uchar length() const
        {
            const auto b = static_cast<uchar>(m_bytes[0]);
            if ((b & 0x80) == 0x00) return 1; // 0xxxxxxx
            if ((b & 0xE0) == 0xC0) return 2; // 110xxxxx
            if ((b & 0xF0) == 0xE0) return 3; // 1110xxxx
            if ((b & 0xF8) == 0xF0) return 4; // 11110xxx
            return 1; // invalid UTF-8, treat as single byte
        }

        /// @brief Convert to @c std::string
        [[nodiscard]] std::string to_string() const
        {
            return { m_bytes, this->length() };
        }

        /// @brief Equality
        bool operator==(const UTF8Char& other) const
        {
            return std::memcmp(this->m_bytes, other.m_bytes, 4) == 0;
        }

        /// @brief Inequality
        bool operator!=(const UTF8Char& other) const
        {
            return !(*this == other);
        }

        /// @brief Copy assignment
        constexpr UTF8Char& operator=(const UTF8Char& other)
        {
            if (this == &other)
                return *this;
        
            std::memcpy(this->m_bytes, other.m_bytes, 4);
            return *this;
        }

        /// @brief Concatenate two @c UTF8Char objects into a @c std::string
        [[nodiscard]] std::string operator+(const UTF8Char& other) const
        {
            return this->to_string() + other.to_string();
        }

        /// @brief Concatenate this @c UTF8Char to the beginning of the given @c std::string
        [[nodiscard]] std::string operator+(const std::string& str) const
        {
            return this->to_string() + str;
        }

        /// @brief Concatenate a @c UTF8Char to the end of a @c std::string
        [[nodiscard]] friend std::string operator+(const std::string& str, const UTF8Char chr)
        {
            return str + chr.to_string();
        }
    };
} // namespace tmt