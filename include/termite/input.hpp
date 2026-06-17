#pragma once

#include "char/key.hpp"

#include <unordered_set>

namespace tmt
{
    /// @brief Tracks key state across frames
    class InputState
    {
    private:
        using Key = key::Key;
        using Case = key::Case;
        using KeySet = std::unordered_set<Key>;

    public:
        /// @brief Poll stdin and update key states for this frame
        /// @note Called automatically once per frame, do not call manually
        void poll();

        /// @brief True only on the frame the key was first pressed
        /// @param key The key to check
        /// @param sensitivity Case-sensitive or not
        [[nodiscard]] bool is_pressed(Key key, Case sensitivity = Case::SENSITIVE) const;

        /// @brief True every frame the key is held
        /// @param key The key to check
        /// @param sensitivity Case-sensitive or not
        [[nodiscard]] bool is_held(Key key, Case sensitivity = Case::SENSITIVE) const;

        /// @brief True on the frame the key was released
        /// @param key The key to check
        /// @param sensitivity Case-sensitive or not
        [[nodiscard]] bool is_released(Key key, Case sensitivity = Case::SENSITIVE) const;

        /// @brief Return the last pressed @c Key
        [[nodiscard]] Key last_pressed() const { return m_last_pressed; }

        /// @brief Return all pressed key in this frame as a @c std::unordered_set<Key>
        [[nodiscard]] const KeySet& pressed_keys() const { return m_pressed; }

        /// @brief Return all pressed key in this frame as a @c std::unordered_set<Key>
        [[nodiscard]] const KeySet& held_keys() const { return m_held; }

        /// @brief Return all pressed key in this frame as a @c std::unordered_set<Key>
        [[nodiscard]] const KeySet& released_keys() const { return m_released; }

    private:
        KeySet m_pressed;  /// Pressed characters in this frame
        KeySet m_held;     /// Characters held this frame
        KeySet m_released; /// Characters released this frame

        /// @brief Last pressed character in a frame
        Key m_last_pressed = key::None;

        /// @brief Find a key in a @c KeySet
        /// @details The function depend on the given case/sensitivity:\n
        /// @c Case::SENSITIVE : Case-sensitive\n
        /// @c Case::IGNORE or @c Case::NONE : Case-insensitive\n
        /// @c Case::UPPER or @c Case::LOWER : convert the key to upper or lowercase
        /// respectively then look for the key as @c Case::SENSITIVE
        static bool contain_in(const KeySet& set, Key key, Case sensitivity);
    };

} // namespace tmt