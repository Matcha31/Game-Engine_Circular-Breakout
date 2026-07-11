#include "physics.hpp"

#include <algorithm>
#include <cmath>

namespace
{
    struct Vec2
    {
        float x;
        float y;
    };

    static Vec2 add(Vec2 a, Vec2 b) { return {a.x + b.x, a.y + b.y}; }
    static Vec2 sub(Vec2 a, Vec2 b) { return {a.x - b.x, a.y - b.y}; }
    static Vec2 mul(Vec2 a, float s) { return {a.x * s, a.y * s}; }

    static float dot(Vec2 a, Vec2 b) { return a.x * b.x + a.y * b.y; }
    static float len2(Vec2 a) { return dot(a, a); }
    static float len(Vec2 a) { return std::sqrt(len2(a)); }

    static Vec2 normalize(Vec2 a)
    {
        const float l = len(a);
        if (l <= 1e-8f)
            return {1.0f, 0.0f};
        return {a.x / l, a.y / l};
    }

    // Counter-clockwise tangent
    static Vec2 perp(Vec2 a) { return {-a.y, a.x}; }

    static float clampf(float x, float lo, float hi)
    {
        return std::max(lo, std::min(x, hi));
    }

    static float wrap_0_2pi(float a)
    {
        const float two_pi = static_cast<float>(2.0 * M_PI);
        a = std::fmod(a, two_pi);
        if (a < 0.0f)
            a += two_pi;
        return a;
    }

    static float wrap_pi(float a)
    {
        const float two_pi = static_cast<float>(2.0 * M_PI);
        a = std::fmod(a + static_cast<float>(M_PI), two_pi);
        if (a < 0.0f)
            a += two_pi;
        return a - static_cast<float>(M_PI);
    }

    static float angle_delta(float a, float center)
    {
        return wrap_pi(a - center);
    }

    static Vec2 point_on_circle(float radius, float angle)
    {
        return {radius * std::cos(angle), radius * std::sin(angle)};
    }

    static Vec2 closest_point_on_segment(Vec2 p, Vec2 a, Vec2 b)
    {
        const Vec2 ab = sub(b, a);
        const float denominator = dot(ab, ab);
        if (denominator <= 1e-12f)
            return a;

        const float t = clampf(dot(sub(p, a), ab) / denominator, 0.0f, 1.0f);
        return add(a, mul(ab, t));
    }

    static void enforce_speed(Vec2& v, float speed)
    {
        const float l = len(v);
        if (l > 1e-8f)
            v = mul(v, speed / l);
    }

    struct Hit
    {
        bool hit = false;
        Vec2 normal{0.0f, 0.0f};
        float penetration = 0.0f;
    };

    static void try_point_hit(Vec2 p, float ball_radius, Vec2 closest_point, Hit& best)
    {
        const Vec2 difference = sub(p, closest_point);
        const float distance_squared = len2(difference);
        const float radius_squared = ball_radius * ball_radius;

        if (distance_squared >= radius_squared)
            return;

        const float distance = std::sqrt(distance_squared);
        const float penetration = ball_radius - distance;
        const Vec2 normal = distance > 1e-8f
            ? mul(difference, 1.0f / distance)
            : normalize(p);

        // The closest boundary point gives the largest penetration value.
        if (!best.hit || penetration > best.penetration)
        {
            best.hit = true;
            best.normal = normal;
            best.penetration = penetration;
        }
    }

    static Hit collide_annular_sector(Vec2 p, float ball_radius,
            float r_inner, float r_outer,
            float angle_center, float angle_half_span)
    {
        Hit best;

        const float ball_angle = std::atan2(p.y, p.x);
        const float offset = angle_delta(ball_angle, angle_center);
        const float clamped_offset = clampf(offset, -angle_half_span, angle_half_span);
        const float clamped_angle = angle_center + clamped_offset;

        try_point_hit(p, ball_radius,
                point_on_circle(r_inner, clamped_angle), best);
        try_point_hit(p, ball_radius,
                point_on_circle(r_outer, clamped_angle), best);

        const float left_angle = angle_center - angle_half_span;
        const float right_angle = angle_center + angle_half_span;

        const Vec2 left_inner = point_on_circle(r_inner, left_angle);
        const Vec2 left_outer = point_on_circle(r_outer, left_angle);
        const Vec2 right_inner = point_on_circle(r_inner, right_angle);
        const Vec2 right_outer = point_on_circle(r_outer, right_angle);

        try_point_hit(p, ball_radius,
                closest_point_on_segment(p, left_inner, left_outer), best);
        try_point_hit(p, ball_radius,
                closest_point_on_segment(p, right_inner, right_outer), best);

        return best;
    }

    // The response is applied only while the ball approaches the surface.
    static bool apply_collision_response(Vec2& ball_velocity,
            Vec2 normal,
            Vec2 surface_velocity,
            float friction,
            float fixed_ball_speed)
    {
        const Vec2 relative_velocity = sub(ball_velocity, surface_velocity);
        const float relative_normal_speed = dot(normal, relative_velocity);

        if (relative_normal_speed >= 0.0f)
            return false;

        const Vec2 normal_component = mul(normal, relative_normal_speed);
        const Vec2 normal_velocity_change = mul(normal_component, -2.0f);

        Vec2 tangential_velocity_change{0.0f, 0.0f};

        if (len2(surface_velocity) > 1e-12f)
        {
            const Vec2 tangential_component = sub(relative_velocity, normal_component);
            const float tangential_length = len(tangential_component);

            if (tangential_length > 1e-8f)
            {
                const float correction = std::min(
                        friction * len(normal_component),
                        tangential_length);

                tangential_velocity_change = mul(
                        tangential_component,
                        -correction / tangential_length);
            }
        }

        Vec2 result = add(
                ball_velocity,
                add(normal_velocity_change, tangential_velocity_change));

        // Fixed ball speed
        if (len2(result) <= 1e-12f)
            result = add(ball_velocity, normal_velocity_change);

        enforce_speed(result, fixed_ball_speed);
        ball_velocity = result;
        return true;
    }

    static void separate_ball(Vec2& p, const Hit& hit, float epsilon)
    {
        p = add(p, mul(hit.normal, hit.penetration + epsilon));
    }

    static int angle_to_col(float a, int columns)
    {
        const float two_pi = static_cast<float>(2.0 * M_PI);
        a = wrap_0_2pi(a);

        int column = static_cast<int>(std::floor((a / two_pi) * columns));
        column = std::max(0, std::min(column, columns - 1));
        return column;
    }

    static int wrap_col(int column, int columns)
    {
        column %= columns;
        if (column < 0)
            column += columns;
        return column;
    }
}

namespace physics
{
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
            float brick_r_outer)
    {
        cfg.outer_limit_r = outer_limit_r;
        cfg.ball_radius = ball_radius;

        cfg.paddle_count = paddle_count;
        cfg.paddle_r_inner = paddle_r_inner;
        cfg.paddle_r_outer = paddle_r_outer;
        cfg.paddle_half_span = paddle_half_span;

        cfg.brick_cols = brick_cols;
        cfg.brick_rows = brick_rows;
        cfg.brick_r_inner = brick_r_inner;
        cfg.brick_r_outer = brick_r_outer;
        cfg.brick_half_span = static_cast<float>(M_PI / brick_cols);

        cfg.dt_substep = 0.005f;
        cfg.max_resolve_iters = 3;
    }

    void place_ball_waiting(GameState& s, float spawn_r)
    {
        const float angle = s.paddle_angle;

        s.ball_x = spawn_r * std::cos(angle);
        s.ball_y = spawn_r * std::sin(angle);
        s.ball_vx = 0.0f;
        s.ball_vy = 0.0f;
        s.ball_launched = false;

        s.ball_r = spawn_r;
        s.ball_a = wrap_0_2pi(angle);
    }

    void launch_ball(GameState& s, const Config& cfg)
    {
        if (s.mode != GameMode::WaitingLaunch)
            return;

        s.mode = GameMode::Playing;
        s.ball_launched = true;

        const Vec2 radial = normalize({s.ball_x, s.ball_y});
        const Vec2 tangent = perp(radial);

        const Vec2 direction = add(
                mul(radial, std::cos(cfg.launch_angle)),
                mul(tangent, std::sin(cfg.launch_angle)));

        const Vec2 velocity = mul(direction, s.ball_speed);

        s.ball_vx = velocity.x;
        s.ball_vy = velocity.y;
    }

    static void resolve_one_step(GameState& s,
            BrickField& bricks,
            const Config& cfg,
            float h)
    {
        if (s.mode != GameMode::Playing || !s.ball_launched)
            return;

        Vec2 p{s.ball_x, s.ball_y};
        Vec2 v{s.ball_vx, s.ball_vy};

        p = add(p, mul(v, h));

        const float radius_from_origin = len(p);
        if (radius_from_origin + cfg.ball_radius > cfg.outer_limit_r)
        {
            s.mode = GameMode::GameOver;
            s.ball_launched = false;
            s.ball_vx = 0.0f;
            s.ball_vy = 0.0f;
            return;
        }

        for (int iteration = 0; iteration < cfg.max_resolve_iters; ++iteration)
        {
            bool corrected_overlap = false;

            // ----- Ball against paddles -----
            Hit best_paddle_hit;

            for (int i = 0; i < cfg.paddle_count; ++i)
            {
                const float center = s.paddle_angle
                    + static_cast<float>(i)
                    * static_cast<float>(2.0 * M_PI / cfg.paddle_count);

                const Hit hit = collide_annular_sector(
                        p,
                        cfg.ball_radius,
                        cfg.paddle_r_inner,
                        cfg.paddle_r_outer,
                        center,
                        cfg.paddle_half_span);

                if (hit.hit
                        && (!best_paddle_hit.hit
                            || hit.penetration > best_paddle_hit.penetration))
                {
                    best_paddle_hit = hit;
                }
            }

            if (best_paddle_hit.hit)
            {
                const Vec2 contact_point = sub(
                        p,
                        mul(best_paddle_hit.normal, cfg.ball_radius));
                const Vec2 paddle_velocity = mul(
                        perp(contact_point),
                        s.paddle_omega);

                const bool bounced = apply_collision_response(
                        v,
                        best_paddle_hit.normal,
                        paddle_velocity,
                        cfg.paddle_friction,
                        s.ball_speed);

                separate_ball(p, best_paddle_hit, cfg.separation_epsilon);
                corrected_overlap = true;

                if (bounced)
                    ++s.hit_paddle_count;
            }

            // ----- Ball against the brick wall -----
            const float ball_angle = wrap_0_2pi(std::atan2(p.y, p.x));
            const int central_column = angle_to_col(ball_angle, cfg.brick_cols);

            const int candidates[3] = {
                wrap_col(central_column, cfg.brick_cols),
                wrap_col(central_column - 1, cfg.brick_cols),
                wrap_col(central_column + 1, cfg.brick_cols)
            };

            Hit best_brick_hit;
            int best_column = -1;

            for (const int column : candidates)
            {
                const int row = bricks.lowest_alive_row_in_column(column);
                if (row < 0)
                    continue;

                const Brick& brick = bricks.at(column, row);
                const Hit hit = collide_annular_sector(
                        p,
                        cfg.ball_radius,
                        cfg.brick_r_inner,
                        cfg.brick_r_outer,
                        brick.a_center,
                        cfg.brick_half_span);

                if (hit.hit
                        && (!best_brick_hit.hit
                            || hit.penetration > best_brick_hit.penetration))
                {
                    best_brick_hit = hit;
                    best_column = column;
                }
            }

            if (best_brick_hit.hit && best_column >= 0)
            {
                const bool bounced = apply_collision_response(
                        v,
                        best_brick_hit.normal,
                        {0.0f, 0.0f},
                        0.0f,
                        s.ball_speed);

                separate_ball(p, best_brick_hit, cfg.separation_epsilon);
                corrected_overlap = true;

                if (bounced)
                {
                    const int row = bricks.lowest_alive_row_in_column(best_column);
                    if (row >= 0 && bricks.hit_and_collapse(best_column, row))
                    {
                        ++s.hit_brick_count;

                        if (bricks.alive_count() == 0)
                        {
                            s.mode = GameMode::Win;
                            s.ball_launched = false;
                            s.ball_vx = 0.0f;
                            s.ball_vy = 0.0f;
                            break;
                        }
                    }
                }
            }

            if (!corrected_overlap)
                break;
        }

        s.ball_x = p.x;
        s.ball_y = p.y;
        s.ball_vx = v.x;
        s.ball_vy = v.y;
        s.ball_r = len(p);
        s.ball_a = wrap_0_2pi(std::atan2(p.y, p.x));
    }

    void step(GameState& s, BrickField& bricks, const Config& cfg, float dt)
    {
        dt = clampf(dt, 0.0f, 0.05f);

        if (s.mode == GameMode::Paused
                || s.mode == GameMode::GameOver
                || s.mode == GameMode::Win)
        {
            return;
        }

        if (s.mode == GameMode::WaitingLaunch)
        {
            place_ball_waiting(s, s.ball_r);
            return;
        }

        int substeps = 1;
        if (cfg.dt_substep > 1e-6f)
            substeps = static_cast<int>(std::ceil(dt / cfg.dt_substep));
        substeps = std::max(1, substeps);

        const float h = dt / static_cast<float>(substeps);
        for (int i = 0; i < substeps; ++i)
        {
            resolve_one_step(s, bricks, cfg, h);
            if (s.mode != GameMode::Playing)
                break;
        }
    }
}
