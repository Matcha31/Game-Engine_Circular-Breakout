#pragma once

#include "game_state.hpp"
#include "bricks.hpp"

namespace physics
{
    struct Config
    {
        float ball_radius = 0.04f;
        float outer_limit_r = 2.45f;
        float launch_angle = 0.30f;

        int paddle_count = 3;
        float paddle_r_inner = 2.00f;
        float paddle_r_outer = 2.20f;
        float paddle_half_span = 0.22f;

        int brick_cols = 28;
        int brick_rows = 4;
        float brick_r_inner = 0.60f;
        float brick_r_outer = 0.80f;
        float brick_half_span = 0.0f;

        float paddle_friction = 0.50f;

        // Small positional correction after an overlap
        float separation_epsilon = 1e-4f;

        float dt_substep = 0.005f;
        int max_resolve_iters = 3;
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
    void launch_ball(GameState& s, const Config& cfg);
    void step(GameState& s, BrickField& bricks, const Config& cfg, float dt);
}
