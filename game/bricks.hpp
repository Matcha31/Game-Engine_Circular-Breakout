#pragma once

#include <vector>

struct Brick
{
    float a_center = 0.0f;
    float a_half_span = 0.10f;

    float r_inner = 1.05f;
    float r_outer = 1.25f;

    bool alive = true;
};

struct BrickField
{
    std::vector<Brick> bricks;

    void build_ring(int count, float start_angle, float step_angle,
                    float a_half_span, float r_inner, float r_outer);

    int alive_count() const;
};
