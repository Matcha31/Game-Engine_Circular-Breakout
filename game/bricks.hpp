#pragma once
#include <vector>

struct Brick
{
    int col = 0;
    int row = 0;

    float a_center = 0.0f;
    float a_half_span = 0.10f;

    bool alive = true;

    float row_visual = 0.0f;
    int row_target = 0;
};

struct BrickField
{
    int cols = 0;
    int rows = 0;

    std::vector<Brick> bricks;

    void build_wall(int cols_count, int rows_count, float start_angle, float a_half_span);

    int alive_count() const;

    Brick& at(int col, int row);
    const Brick& at(int col, int row) const;

    int top_alive_row_in_column(int col) const;

    int lowest_alive_row_in_column(int col) const;

    bool hit_and_collapse(int col, int row_hit);

    void update_fall(float dt, float rows_per_second = 12.0f);
};
