#include "bricks.hpp"

#include <algorithm>
#include <cmath>

namespace
{
    float clampf(float x, float lo, float hi)
    {
        return std::max(lo, std::min(x, hi));
    }
}

void BrickField::build_wall(
        int cols_count,
        int rows_count,
        float start_angle,
        float a_half_span)
{
    cols = cols_count;
    rows = rows_count;

    bricks.clear();
    bricks.resize(
            static_cast<std::size_t>(cols)
            * static_cast<std::size_t>(rows));

    const float step =
        static_cast<float>(2.0 * M_PI) / static_cast<float>(cols);

    for (int c = 0; c < cols; ++c)
    {
        const float angle =
            start_angle + step * static_cast<float>(c);

        for (int r = 0; r < rows; ++r)
        {
            Brick brick;
            brick.col = c;
            brick.row = r;
            brick.a_center = angle;
            brick.a_half_span = a_half_span;
            brick.alive = true;

            brick.row_visual = static_cast<float>(r);
            brick.row_target = r;

            // The checker colour belongs to this brick and will follow it
            // when the brick falls.
            brick.color_variant = (c + r) & 1;

            bricks[
                static_cast<std::size_t>(c)
                * static_cast<std::size_t>(rows)
                + static_cast<std::size_t>(r)
            ] = brick;
        }
    }
}

int BrickField::alive_count() const
{
    int count = 0;

    for (const Brick& brick : bricks)
    {
        if (brick.alive)
            ++count;
    }

    return count;
}

Brick& BrickField::at(int col, int row)
{
    return bricks[
        static_cast<std::size_t>(col)
        * static_cast<std::size_t>(rows)
        + static_cast<std::size_t>(row)
    ];
}

const Brick& BrickField::at(int col, int row) const
{
    return bricks[
        static_cast<std::size_t>(col)
        * static_cast<std::size_t>(rows)
        + static_cast<std::size_t>(row)
    ];
}

int BrickField::top_alive_row_in_column(int col) const
{
    if (col < 0 || col >= cols)
        return -1;

    for (int r = rows - 1; r >= 0; --r)
    {
        if (at(col, r).alive)
            return r;
    }

    return -1;
}

int BrickField::lowest_alive_row_in_column(int col) const
{
    if (col < 0 || col >= cols)
        return -1;

    for (int r = 0; r < rows; ++r)
    {
        if (at(col, r).alive)
            return r;
    }

    return -1;
}

int BrickField::bottom_collision_row_in_column(
        int col,
        float settle_tolerance) const
{
    if (col < 0 || col >= cols || rows <= 0)
        return -1;

    const Brick& bottom = at(col, 0);
    if (!bottom.alive)
        return -1;

    if (std::fabs(bottom.row_visual) > std::max(settle_tolerance, 0.0f))
        return -1;

    return 0;
}

bool BrickField::hit_and_collapse(int col, int row_hit)
{
    if (col < 0 || col >= cols)
        return false;

    if (row_hit < 0 || row_hit >= rows)
        return false;

    if (!at(col, row_hit).alive)
        return false;

    for (int r = row_hit; r < rows - 1; ++r)
    {
        Brick& destination = at(col, r);
        const Brick& source = at(col, r + 1);

        if (source.alive)
        {
            const float source_visual_row = source.row_visual;

            destination = source;

            destination.col = col;
            destination.row = r;
            destination.row_visual = source_visual_row;
            destination.row_target = r;
        }
        else
        {
            destination.alive = false;
            destination.col = col;
            destination.row = r;
            destination.row_visual = static_cast<float>(r);
            destination.row_target = r;
        }
    }

    Brick& top = at(col, rows - 1);
    top.alive = false;
    top.col = col;
    top.row = rows - 1;
    top.row_visual = static_cast<float>(rows - 1);
    top.row_target = rows - 1;

    return true;
}

void BrickField::update_fall(float dt, float rows_per_second)
{
    dt = clampf(dt, 0.0f, 0.05f);

    if (dt <= 0.0f)
        return;

    rows_per_second = std::max(rows_per_second, 0.01f);
    const float maximum_step = rows_per_second * dt;

    for (Brick& brick : bricks)
    {
        if (!brick.alive)
            continue;

        const float target =
            static_cast<float>(brick.row_target);

        const float difference =
            target - brick.row_visual;

        brick.row_visual += clampf(
                difference,
                -maximum_step,
                maximum_step);

        if (std::fabs(brick.row_visual - target) < 1e-4f)
            brick.row_visual = target;
    }
}
