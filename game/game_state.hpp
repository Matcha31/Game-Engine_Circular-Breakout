#pragma once

enum class GameMode
{
    WaitingLaunch,
    Playing,
    Paused,
    Win,
    GameOver
};

struct GameState
{
    GameMode mode = GameMode::WaitingLaunch;

    bool left_down = false;
    bool right_down = false;

    float paddle_angle = 0.0f;
    float paddle_speed = 0.8f;

    float ball_r = 0.35f;
    float ball_a = 0.0f;

    float ball_x = 0.0f;
    float ball_y = 0.0f;

    float ball_vx = 0.0f;
    float ball_vy = 0.0f;

    float ball_speed = 0.8f;
    bool ball_launched = false;

    float ball_radial_speed = 0.0f;
    float ball_angular_speed = 0.0f;

    float outer_limit_r = 2.2f;

    float paddle_omega = 0.0f;
    float hit_cooldown = 0.0f;

    void reset();
    void launch();
    void toggle_pause();
    void update(float dt);
};
