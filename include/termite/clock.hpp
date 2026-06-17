#pragma once

#include <chrono>

namespace tmt
{
    struct Clock
    {
        Clock();

        float tick();

        void sleep_until_next_frame(float target_fps);

        [[nodiscard]] float delta_time() const { return m_delta_time; }

        [[nodiscard]] float fps() const { return 1.0f / m_delta_time; }


    private:
        using clock_t      = std::chrono::steady_clock;
        using time_point_t = std::chrono::time_point<clock_t>;
        using duration_t   = std::chrono::duration<float>;

        time_point_t m_last_tick;
        time_point_t m_frame_start;
        float m_delta_time = 0.0f;
        float m_fps = 0.0f;
    };
}