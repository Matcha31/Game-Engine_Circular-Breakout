#include "physics.hpp"

#include <cmath>
#include <algorithm>

namespace
{
    struct Vec2 { float x, y; };

    static Vec2 add(Vec2 a, Vec2 b) { return {a.x + b.x, a.y + b.y}; }
    static Vec2 sub(Vec2 a, Vec2 b) { return {a.x - b.x, a.y - b.y}; }
    static Vec2 mul(Vec2 a, float s) { return {a.x * s, a.y * s}; }

    static float dot(Vec2 a, Vec2 b) { return a.x * b.x + a.y * b.y; }
    static float len2(Vec2 a) { return dot(a, a); }
    static float len(Vec2 a) { return std::sqrt(len2(a)); }

    static Vec2 normalize(Vec2 a)
    {
        float l = len(a);
        if (l <= 1e-8f) return {1.0f, 0.0f};
        return {a.x / l, a.y / l};
    }

    static Vec2 perp(Vec2 a) { return {-a.y, a.x}; }

    static float clampf(float x, float lo, float hi)
    {
        if (x < lo) return lo;
        if (x > hi) return hi;
        return x;
    }

    static float wrap_0_2pi(float a)
    {
        float two = (float)(2.0 * M_PI);
        a = std::fmod(a, two);
        if (a < 0.0f) a += two;
        return a;
    }

    static float wrap_pi(float a)
    {
        float two = (float)(2.0 * M_PI);
        a = std::fmod(a + (float)M_PI, two);
        if (a < 0.0f) a += two;
        return a - (float)M_PI;
    }

    static float angle_delta(float a, float center)
    {
        return wrap_pi(a - center);
    }

    static Vec2 point_on_circle(float r, float a)
    {
        return {r * std::cos(a), r * std::sin(a)};
    }

    static Vec2 closest_point_on_segment(Vec2 p, Vec2 a, Vec2 b)
    {
        Vec2 ab = sub(b, a);
        float denom = dot(ab, ab) + 1e-12f;
        float t = dot(sub(p, a), ab) / denom;
        t = clampf(t, 0.0f, 1.0f);
        return add(a, mul(ab, t));
    }

    static Vec2 reflect(Vec2 v, Vec2 n)
    {
        float vn = dot(v, n);
        return sub(v, mul(n, 2.0f * vn));
    }

    static void enforce_speed(Vec2& v, float speed)
    {
        v = mul(normalize(v), speed);
    }

    struct Hit
    {
        bool hit = false;
        Vec2 n{0, 0};
        float pen = 0.0f;
    };

    static void try_point_hit(Vec2 p, float ball_radius, Vec2 q, Hit& best)
    {
        Vec2 diff = sub(p, q);
        float d2 = len2(diff);
        if (d2 >= ball_radius * ball_radius) return;

        float d = std::sqrt(d2);
        float pen = ball_radius - d;
        Vec2 n = (d <= 1e-8f) ? normalize(p) : mul(diff, 1.0f / d);

        if (!best.hit || pen > best.pen)
        {
            best.hit = true;
            best.pen = pen;
            best.n = n;
        }
    }

    static Hit collide_annular_sector(Vec2 p, float ball_radius,
            float r_inner, float r_outer,
            float a_center, float a_half)
    {
        Hit best;

        float a = std::atan2(p.y, p.x);
        float d = angle_delta(a, a_center);
        float dc = clampf(d, -a_half, +a_half);
        float a_clamped = a_center + dc;

        Vec2 q_inner = point_on_circle(r_inner, a_clamped);
        Vec2 q_outer = point_on_circle(r_outer, a_clamped);

        try_point_hit(p, ball_radius, q_inner, best);
        try_point_hit(p, ball_radius, q_outer, best);

        float aL = a_center - a_half;
        float aR = a_center + a_half;

        Vec2 A0 = point_on_circle(r_inner, aL);
        Vec2 A1 = point_on_circle(r_outer, aL);
        Vec2 B0 = point_on_circle(r_inner, aR);
        Vec2 B1 = point_on_circle(r_outer, aR);

        Vec2 qL = closest_point_on_segment(p, A0, A1);
        Vec2 qR = closest_point_on_segment(p, B0, B1);

        try_point_hit(p, ball_radius, qL, best);
        try_point_hit(p, ball_radius, qR, best);

        return best;
    }

    static int angle_to_col(float a, int cols)
    {
        float two = (float)(2.0 * M_PI);
        a = wrap_0_2pi(a);
        int c = (int)std::floor((a / two) * (float)cols);
        if (c < 0) c = 0;
        if (c >= cols) c = cols - 1;
        return c;
    }

    static int wrap_col(int c, int cols)
    {
        c %= cols;
        if (c < 0) c += cols;
        return c;
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

        cfg.brick_half_span = (float)(M_PI / (double)brick_cols);

        cfg.dt_substep = 0.005f;
        cfg.max_resolve_iters = 3;
    }

    void place_ball_waiting(GameState& s, float spawn_r)
    {
        float a = s.paddle_angle;

        s.ball_x = spawn_r * std::cos(a);
        s.ball_y = spawn_r * std::sin(a);

        s.ball_vx = 0.0f;
        s.ball_vy = 0.0f;
        s.ball_launched = false;

        s.ball_r = spawn_r;
        s.ball_a = wrap_0_2pi(a);
    }

    void launch_ball(GameState& s, const Config& cfg)
    {
        if (s.mode != GameMode::WaitingLaunch)
            return;

        s.mode = GameMode::Playing;
        s.ball_launched = true;

        Vec2 p{ s.ball_x, s.ball_y };
        Vec2 radial = normalize(p);
        Vec2 tangent = perp(radial);

        Vec2 v = mul(radial, s.ball_speed);
        enforce_speed(v, s.ball_speed);

        s.ball_vx = v.x;
        s.ball_vy = v.y;
    }

    static void resolve_one_step(GameState& s, BrickField& bricks, const Config& cfg, float h)
    {
        if (s.mode != GameMode::Playing || !s.ball_launched)
            return;

        Vec2 p{ s.ball_x, s.ball_y };
        Vec2 v{ s.ball_vx, s.ball_vy };

        p = add(p, mul(v, h));

        float pr = std::sqrt(p.x * p.x + p.y * p.y);
        if (pr + cfg.ball_radius > cfg.outer_limit_r)
        {
            s.mode = GameMode::GameOver;
            s.ball_launched = false;
            s.ball_vx = 0.0f;
            s.ball_vy = 0.0f;
            return;
        }

        auto apply_basic_hit = [&](const Hit& hit)
        {
            p = add(p, mul(hit.n, hit.pen + cfg.bounce_push));
            v = reflect(v, hit.n);
            enforce_speed(v, s.ball_speed);
        };

        for (int iter = 0; iter < cfg.max_resolve_iters; ++iter)
        {
            bool any = false;

            Hit bestP;
            float bestCenter = 0.0f;

            for (int i = 0; i < cfg.paddle_count; ++i)
            {
                float center = s.paddle_angle + (float)i * (float)(2.0 * M_PI / (double)cfg.paddle_count);

                Hit hP = collide_annular_sector(p, cfg.ball_radius,
                        cfg.paddle_r_inner, cfg.paddle_r_outer,
                        center, cfg.paddle_half_span);

                if (hP.hit && (!bestP.hit || hP.pen > bestP.pen))
                {
                    bestP = hP;
                    bestCenter = center;
                }
            }

            if (bestP.hit && s.hit_cooldown <= 0.0f)
            {
                p = add(p, mul(bestP.n, bestP.pen + cfg.bounce_push));

                float vn_in = dot(v, bestP.n);
                if (vn_in < 0.0f)
                    v = reflect(v, bestP.n);

                Vec2 radial = normalize(p);
                Vec2 tangent = perp(radial);

                float a = wrap_0_2pi(std::atan2(p.y, p.x));
                float off = angle_delta(a, bestCenter);
                float t = clampf(off / cfg.paddle_half_span, -1.0f, 1.0f);

                float rc = std::sqrt(p.x * p.x + p.y * p.y);
                float vt = s.paddle_omega * rc;
                Vec2 vp = mul(tangent, vt);

                v = add(v, mul(vp, cfg.paddle_influence));
                v = add(v, mul(tangent, cfg.paddle_aim * t * s.ball_speed));
                v = add(v, mul(tangent, cfg.paddle_face_bias * s.ball_speed));

                // minimum separating component
                float vn_out = dot(v, bestP.n);
                float vn_min = cfg.min_separating_speed * s.ball_speed;
                if (vn_out < vn_min)
                {
                    v = add(v, mul(bestP.n, (vn_min - vn_out)));
                }

                enforce_speed(v, s.ball_speed);

                s.hit_cooldown = cfg.paddle_hit_cooldown;
                any = true;
            }

            float a = wrap_0_2pi(std::atan2(p.y, p.x));
            int c0 = angle_to_col(a, cfg.brick_cols);

            int candidates[3] = {
                wrap_col(c0, cfg.brick_cols),
                wrap_col(c0 - 1, cfg.brick_cols),
                wrap_col(c0 + 1, cfg.brick_cols)
            };

            Hit bestB;
            int bestCol = -1;

            for (int ii = 0; ii < 3; ++ii)
            {
                int c = candidates[ii];

                int row = bricks.lowest_alive_row_in_column(c);
                if (row < 0) continue;

                const Brick& b = bricks.at(c, row);

                Hit hB = collide_annular_sector(p, cfg.ball_radius,
                        cfg.brick_r_inner, cfg.brick_r_outer,
                        b.a_center, cfg.brick_half_span);

                if (hB.hit && (!bestB.hit || hB.pen > bestB.pen))
                {
                    bestB = hB;
                    bestCol = c;
                }
            }

            if (bestB.hit && bestCol >= 0)
            {
                int row_hit = bricks.lowest_alive_row_in_column(bestCol);
                bricks.hit_and_collapse(bestCol, row_hit);

                apply_basic_hit(bestB);
                any = true;

                if (bricks.alive_count() == 0)
                {
                    s.mode = GameMode::Win;
                    s.ball_launched = false;
                    s.ball_vx = 0.0f;
                    s.ball_vy = 0.0f;
                    break;
                }
            }

            if (!any)
                break;
        }

        s.ball_x = p.x;
        s.ball_y = p.y;
        s.ball_vx = v.x;
        s.ball_vy = v.y;

        s.ball_r = std::sqrt(p.x * p.x + p.y * p.y);
        s.ball_a = wrap_0_2pi(std::atan2(p.y, p.x));
    }

    void step(GameState& s, BrickField& bricks, const Config& cfg, float dt)
    {
        if (dt < 0.0f) dt = 0.0f;
        if (dt > 0.05f) dt = 0.05f;

        if (s.mode == GameMode::Paused || s.mode == GameMode::GameOver || s.mode == GameMode::Win)
            return;

        if (s.mode == GameMode::WaitingLaunch)
        {
            place_ball_waiting(s, s.ball_r);
            return;
        }

        int steps = 1;
        if (cfg.dt_substep > 1e-6f)
            steps = (int)std::ceil(dt / cfg.dt_substep);
        steps = std::max(1, steps);

        float h = dt / (float)steps;

        for (int i = 0; i < steps; ++i)
            resolve_one_step(s, bricks, cfg, h);
    }
}
