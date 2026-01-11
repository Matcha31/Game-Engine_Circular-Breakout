#include "bricks.hpp"
#include <cmath>
#include <cstdio>

static float clampf(float x, float lo, float hi)
{
    if (x < lo) return lo;
    if (x > hi) return hi;
    return x;
}

void BrickField::build_wall(int cols_count, int rows_count, float start_angle, float a_half_span)
{
    cols = cols_count;
    rows = rows_count;

    bricks.clear();
    bricks.resize((size_t)cols * (size_t)rows);

    float step = (float)(2.0 * M_PI) / (float)cols;

    for (int c = 0; c < cols; ++c)
    {
        float a = start_angle + step * (float)c;

        for (int r = 0; r < rows; ++r)
        {
            Brick b;
            b.col = c;
            b.row = r;
            b.a_center = a;
            b.a_half_span = a_half_span;
            b.alive = true;

            b.row_visual = (float)r;
            b.row_target = r;

            bricks[(size_t)c * (size_t)rows + (size_t)r] = b;
        }
    }
}

int BrickField::alive_count() const
{
    int c = 0;
    for (const auto& b : bricks)
        if (b.alive) ++c;
    return c;
}

Brick& BrickField::at(int col, int row)
{
    return bricks[(size_t)col * (size_t)rows + (size_t)row];
}

const Brick& BrickField::at(int col, int row) const
{
    return bricks[(size_t)col * (size_t)rows + (size_t)row];
}

int BrickField::top_alive_row_in_column(int col) const
{
    for (int r = 0; r < rows; ++r)
    {
        if (at(col, r).alive)
            return r;
    }
    return -1;
}

int BrickField::lowest_alive_row_in_column(int col) const
{
//     for (int r = rows - 1; r >= 0; --r)
    for (int r = 0; r < rows; ++r)
    {
        if (at(col, r).alive)
            return r;
    }
    return -1;
}

bool BrickField::hit_and_collapse(int col, int row_hit)
{
    if (col < 0 || col >= cols) return false;
    if (row_hit < 0 || row_hit >= rows) return false;
    if (!at(col, row_hit).alive) return false;

    at(col, row_hit).alive = false;

    for (int r = row_hit; r < rows - 1; ++r)
    {
        const Brick& src = at(col, r + 1);
        Brick& dst = at(col, r);

        dst.alive = src.alive;

        if (dst.alive)
        {
            dst.row_visual = (float)(r + 1);
            dst.row_target = r;
        }
        else
        {
            dst.row_visual = (float)r;
            dst.row_target = r;
        }
    }

    Brick& top = at(col, rows - 1);
    top.alive = false;
    top.row_visual = (float)(rows - 1);
    top.row_target = rows - 1;

    return true;
}

void BrickField::update_fall(float dt, float rows_per_second)
{
    for (auto& b : bricks)
    {
        float diff = (float)b.row_target - b.row_visual;
        if (b.alive && std::fabs(diff) > 1e-4f)
        {
            printf("MOVING col=%d row_target=%d row_visual=%f diff=%f\n",
                    b.col, b.row_target, b.row_visual, diff);
            break;
        }
    }

    if (dt <= 0.0f) return;
    if (rows_per_second <= 0.0f) rows_per_second = 1.0f;

    float max_step = rows_per_second * dt;

    for (auto& b : bricks)
    {
        float target = (float)b.row_target;
        float diff = target - b.row_visual;

        float step = clampf(diff, -max_step, +max_step);
        b.row_visual += step;

        if (std::fabs(b.row_visual - target) < 1e-3f)
            b.row_visual = target;
    }
}
