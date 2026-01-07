#pragma once

#include "game_state.hpp"
#include "bricks.hpp"

namespace physics
{
    struct Config
    {
        float ball_radius = 0.08f;

        float outer_limit_r = 2.35f;

        int paddle_count = 3;
        float paddle_r_inner = 1.70f;
        float paddle_r_outer = 1.90f;
        float paddle_half_span = 0.22f;

        int brick_cols = 28;
        int brick_rows = 4;
        float brick_r_inner = 0.50f;
        float brick_r_outer = 0.70f;
        float brick_half_span = 0.0f;
        float brick_z0 = 0.02f;
        float brick_height = 0.10f;
        float brick_row_gap_z = 0.0f;

        float bounce_push = 0.0015f;

        float paddle_influence = 0.8f;

        float dt_substep = 0.005f;
    };

    void sync_config_from_scene(Config& cfg,
                                float outer_limit_r,
                                float ball_radius,
                                int paddle_count,
                                float paddle_r_inner,
                                float paddle_r_outer,
                                float paddle_half_span,
                                int brick_cols,
                                int brick_rows,
                                float brick_r_inner,
                                float brick_r_outer);

    void place_ball_waiting(GameState& s, float spawn_r);
    void launch_ball(GameState& s);
    void step(GameState& s, BrickField& bricks, const Config& cfg, float dt);
}
