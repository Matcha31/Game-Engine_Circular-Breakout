#pragma once

#include "../game/game_state.hpp"
#include "../game/bricks.hpp"

namespace game
{
    struct CollisionConfig
    {
        float ball_radius = 0.20f;

        float paddle_r_inner = 1.40f;
        float paddle_r_outer = 1.60f;
        float paddle_a_half_span = 0.25f;

        float brick_r_inner = 1.75f;
        float brick_r_outer = 1.90f;
        float brick_a_half_span = 0.10f;

        float bounce_push = 0.002f;
        float hit_cooldown = 0.03f;

        float paddle_influence = 0.35f;
    };

    void resolve_collisions(GameState& state, BrickField& bricks, const CollisionConfig& cfg);
}
