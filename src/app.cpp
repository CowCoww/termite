#include "../include/app.hpp"
#include "../include/terminal.hpp"

#include <algorithm>

namespace tmt
{
    App::App() : canvas(terminal::get_size()) {}
    App::App(const Vec2i size) : canvas(size) {}

    App::~App()
    {
        terminal::show_cursor();
        terminal::restore();
    }

    // == Loop control ==

    void App::quit() { m_running = false; }

    void App::set_target_fps(const int fps)
    {
        m_target_fps = std::clamp(fps, 1, 120);
    }

    // == Run ==

    void App::run()
    {
        terminal::set_raw_mode();
        terminal::hide_cursor();
        terminal::clear();
        terminal::init_signals();

        m_running = true;
        this->on_start();

        while (m_running)
        {
            // == Timing ==
            const float dt = m_clock.tick();

            // == Signals ==
            if (terminal::has_quit() || input.is_pressed(tmt::key::ctrl('x')))
            {
                terminal::clear_quit_flag();
                m_running = false;
                break;
            }

            if (terminal::has_resize())
            {
                terminal::clear_resize_flag();
                this->on_resize(terminal::get_size());
                canvas.force_full_render();
            }

            // == Update ==
            this->input.poll();
            this->on_update(dt);

            // == Render ==
            this->on_render();
            canvas.flush();

            // == Wait until next frame ==
            m_clock.sleep_until_next_frame(m_target_fps);
        }

        this->on_stop();
        terminal::show_cursor();
        terminal::restore();
    }

} // namespace tmt