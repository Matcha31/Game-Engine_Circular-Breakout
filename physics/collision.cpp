#include "collision.hpp"

#include <cmath>

namespace
{
    float wrap_0_2pi(float a)
    {
        const float two_pi = (float)(2.0 * M_PI);
        a = std::fmod(a, two_pi);
        if (a < 0.0f) a += two_pi;
        return a;
    }

    float angle_dist(float a, float b)
    {
        const float two_pi = (float)(2.0 * M_PI);
        float d = std::fabs(wrap_0_2pi(a) - wrap_0_2pi(b));
        if (d > two_pi - d) d = two_pi - d;
        return d;
    }

    float clamp01(float x)
    {
        if (x < 0.0f) return 0.0f;
        if (x > 1.0f) return 1.0f;
        return x;
    }

    float angular_radius(float r, float ball_radius)
    {
        if (r <= 1e-6f) return (float)M_PI;
        float s = clamp01(ball_radius / r);
        return std::asin(s);
    }

    bool overlaps_polar_band(float ball_r, float ball_a, float ball_radius,
                             float band_r_inner, float band_r_outer,
                             float center_a, float half_span_a)
    {
        float r_min = ball_r - ball_radius;
        float r_max = ball_r + ball_radius;

        if (r_max < band_r_inner) return false;
        if (r_min > band_r_outer) return false;

        float ar = angular_radius(ball_r, ball_radius);
        float a_allow = half_span_a + ar;

        return angle_dist(ball_a, center_a) <= a_allow;
    }
}

namespace game
{
    void resolve_collisions(GameState& state, BrickField& bricks, const CollisionConfig& cfg)
    {
        if (state.mode != GameMode::Playing)
            return;

        if (state.hit_cooldown > 0.0f)
            return;

        bool bounced = false;

        for (Brick& b : bricks.bricks)
        {
            if (!b.alive) continue;

            if (!overlaps_polar_band(state.ball_r, state.ball_a, cfg.ball_radius,
                                     b.r_inner, b.r_outer,
                                     b.a_center, b.a_half_span))
                continue;

            b.alive = false;

            state.ball_radial_speed = -state.ball_radial_speed;

            if (state.ball_radial_speed > 0.0f)
                state.ball_r = b.r_inner - cfg.ball_radius - cfg.bounce_push;
            else
                state.ball_r = b.r_outer + cfg.ball_radius + cfg.bounce_push;

            bounced = true;
            break;
        }

        if (!bounced)
        {
            for (int i = 0; i < 3; ++i)
            {
                float paddle_center = state.paddle_angle + (float)(2.0 * M_PI / 3.0) * (float)i;

                if (!overlaps_polar_band(state.ball_r, state.ball_a, cfg.ball_radius,
                                         cfg.paddle_r_inner, cfg.paddle_r_outer,
                                         paddle_center, cfg.paddle_a_half_span))
                    continue;

                if (state.ball_radial_speed > 0.0f)
                    state.ball_radial_speed = -state.ball_radial_speed;
                else
                    state.ball_radial_speed = std::fabs(state.ball_radial_speed);

                state.ball_r = cfg.paddle_r_inner - cfg.ball_radius - cfg.bounce_push;

                state.ball_angular_speed += cfg.paddle_influence * state.paddle_omega;

                bounced = true;
                break;
            }
        }

        if (bounced)
            state.hit_cooldown = cfg.hit_cooldown;

        if (bricks.alive_count() == 0)
            state.mode = GameMode::Win;
    }
}
