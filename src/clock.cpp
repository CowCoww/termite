#include "../include/termite/clock.hpp"

#include <thread>

namespace tmt
{
    Clock::Clock() :
        m_last_tick(clock_t::now()),
        m_frame_start(m_last_tick)
    {}

    float Clock::tick()
    {
        const auto now = clock_t::now();
        m_delta_time   = std::chrono::duration_cast<duration_t>(now - m_last_tick).count();
        m_fps          = (m_delta_time > 0.0f) ? 1.0f/m_delta_time : 0.0f;
        m_last_tick    = now;
        return m_delta_time;
    }

    void Clock::sleep_until_next_frame(const float target_fps)
    {
        using ms =  std::chrono::duration<float, std::milli>;

        const auto frame_end = clock_t::now();
        const auto frame_duration = std::chrono::duration_cast<ms>(frame_end - m_frame_start);
        const auto target         = ms(1000.0f/static_cast<float>(target_fps));

        if (frame_duration < target)
            std::this_thread::sleep_for(target - frame_duration);

    }
} // namespace tmt