#include "bricks.hpp"
#include <cmath>

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
    for (int r = rows - 1; r >= 0; --r)
    {
        if (at(col, r).alive)
            return r;
    }
    return -1;
}

int BrickField::lowest_alive_row_in_column(int col) const
{
    for (int r = 0; r < rows; ++r)
        if (at(col, r).alive) return r;
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
        bool above = at(col, r + 1).alive;
        at(col, r).alive = above;
    }

    at(col, rows - 1).alive = false;
    return true;
}
