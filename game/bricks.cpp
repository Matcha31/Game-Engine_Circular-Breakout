#include "bricks.hpp"

void BrickField::build_ring(int count, float start_angle, float step_angle,
                            float a_half_span, float r_inner, float r_outer)
{
    bricks.clear();
    bricks.reserve((size_t)count);

    for (int i = 0; i < count; ++i)
    {
        Brick b;
        b.a_center = start_angle + step_angle * (float)i;
        b.a_half_span = a_half_span;
        b.r_inner = r_inner;
        b.r_outer = r_outer;
        b.alive = true;
        bricks.push_back(b);
    }
}

int BrickField::alive_count() const
{
    int c = 0;
    for (const auto& b : bricks)
        if (b.alive) ++c;
    return c;
}
