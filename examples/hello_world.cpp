#include "include/termite.hpp"

class HelloWorld : public tmt::App
{
public:
    void on_resize(tmt::Vec2i new_size) override
    {
        canvas.resize(new_size);
        canvas.force_full_render();
    }

    void on_update(float dt) override
    {
        if (input.is_pressed('a', tmt::key::Case::IGNORE) && position.x > 0)
            m_position.x -= 2; 

        if (input.is_pressed('d', tmt::key::Case::IGNORE) && position.x < canvas.size().x)
            m_position.x += 2;

        if (input.is_pressed('w', tmt::key::Case::IGNORE) && position.y > 0)
            m_position.y--;

        if (input.is_pressed('s', tmt::key::Case::IGNORE) && position.y < canvas.size().y)
            m_position.y++;
    }

    void on_render() override
    {
        canvas.fill(tmt::Cell{ " ", tmt::color::DARK_GRAY, tmt::color::WHITE });
        canvas.draw_text(tmt::Vec2i(m_position), "Hello, World!", false, tmt::color::BLACK, tmt::color::CYAN);
        canvas.draw_text({0, 0}, std::to_string(1.0f/dt), false, tmt::color::GREEN, tmt::color::DARK_GRAY);
        canvas.flush();
    }

private:
    tmt::Vec2i m_position = { 5, 5 };
};

int main()
{
    auto app = HelloWorld();
    app.run();
}
