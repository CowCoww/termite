#include "vec2.hpp"

namespace tmt
{
    struct Rect
    {
        int x, y, width, height;

        Rect() : x(0), y(0), width(0), height(0) {}

        Rect(const int x, const int y, const int width, const int height) :
            x(x), y(y), width(width), height(height)
        {}

        Rect(const Vec2i position, const Vec2i size) :
            x(position.x), y(position.y),
            width(size.x), height(size.y)
        {}
    };
}