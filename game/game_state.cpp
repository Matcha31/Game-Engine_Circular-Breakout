#include "game_state.hpp"

#include <cmath>

namespace
{
    float wrap_angle_0_2pi(float a)
    {
        const float two_pi = (float)(2.0 * M_PI);
        a = std::fmod(a, two_pi);
        if (a < 0.0f) a += two_pi;
        return a;
    }
}

void GameState::reset()
{
    mode = GameMode::WaitingLaunch;

    left_down = false;
    right_down = false;

    paddle_angle = 0.0f;

    ball_r = 0.35f;
    ball_a = 0.0f;

    ball_x = 0.0f;
    ball_y = 0.0f;
    ball_vx = 0.0f;
    ball_vy = 0.0f;
    ball_launched = false;

    ball_radial_speed = 0.0f;
    ball_angular_speed = 0.0f;

    paddle_omega = 0.0f;
    hit_cooldown = 0.0f;
}

void GameState::launch()
{
    if (mode != GameMode::WaitingLaunch)
        return;

    mode = GameMode::Playing;
    ball_launched = true;

    // ball_a = paddle_angle;
    // ball_radial_speed = 0.25f;
    // ball_angular_speed = 0.20f;
}

void GameState::toggle_pause()
{
    if (mode == GameMode::Playing)
    {
        mode = GameMode::Paused;
        return;
    }
    if (mode == GameMode::Paused)
    {
        mode = GameMode::Playing;
        return;
    }
}

void GameState::update(float dt)
{
    if (dt > 0.05f) dt = 0.05f;
    if (dt < 0.0f) dt = 0.0f;

    if (mode == GameMode::Paused || mode == GameMode::GameOver || mode == GameMode::Win)
        return;

    if (hit_cooldown > 0.0f)
    {
        hit_cooldown -= dt;
        if (hit_cooldown < 0.0f) hit_cooldown = 0.0f;
    }

    float dir = 0.0f;
    if (left_down)  dir -= 1.0f;
    if (right_down) dir += 1.0f;

    paddle_omega = dir * paddle_speed;
    paddle_angle = wrap_angle_0_2pi(paddle_angle + paddle_omega * dt);

    if (mode == GameMode::WaitingLaunch)
    {
        ball_a = paddle_angle;
        return;
    }
}
