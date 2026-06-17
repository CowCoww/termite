#pragma once

#include "rendering/canvas.hpp"
#include "input.hpp"
#include "clock.hpp"

namespace tmt
{
    /// @brief Base class for termite applications
    /// @details
    /// Owns the main loop, canvas, and input state. Subclass this and override
    /// the virtual methods to build your application
    class App
    {
    public:
        /// @brief Construct an app with a canvas matching the current terminal size
        App();

        /// @brief Construct an app with a fixed canvas size
        /// @param size The canvas size in cells
        explicit App(Vec2i size);

        /// @brief Destructor
        virtual ~App();

        App(const App&)            = delete;
        App& operator=(const App&) = delete;
        App(App&&)                 = delete;
        App& operator=(App&&)      = delete;

        /// @brief Start the main loop blocks until quit() is called
        void run();

    protected:
        Canvas     canvas;
        InputState input;

        // == Overrides ==

        /// @brief Called once before the loop starts
        virtual void on_start() {}

        /// @brief Called once per frame before rendering
        /// @param dt Time since last frame in seconds
        virtual void on_update(float dt) {}

        /// @brief Called once per frame to draw to the terminal
        virtual void on_render() {}

        /// @brief Called once after the loop exits cleanly
        virtual void on_stop() {}

        /// @brief Called when the terminal is resized
        virtual void on_resize(Vec2i new_size) {}

        // == Loop control ==

        /// @brief Signal the loop to exit after the current frame
        void quit();

        /// @brief Set the target frames per second
        /// @param fps The target FPS, clamped to [1; 120]
        void set_target_fps(int fps);

        /// @brief Get the current target FPS
        [[nodiscard]] int target_fps() const { return m_target_fps; }

        /// @brief Get the actual FPS of the last frame
        [[nodiscard]] float fps() const { return m_clock.fps(); }

        /// @brief Get the delta time of the last frame in seconds
        [[nodiscard]] float delta_time() const { return m_clock.delta_time(); }

    private:
        Clock m_clock;
        bool  m_running    = false;
        int   m_target_fps = 24;
    };

} // namespace tmt