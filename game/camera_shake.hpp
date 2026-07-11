#pragma once
#include <cmath>

struct CameraShake
{
    float time = 0.0f;
    float duration = 0.0f;
    float amplitude = 0.0f;

    void trigger(float amp, float dur)
    {
        if (dur <= 0.0f) return;
        amplitude = std::max(amplitude, amp);
        duration = std::max(duration, dur);
        time = duration;
    }

    void update(float dt)
    {
        if (time <= 0.0f) return;
        time -= dt;
        if (time < 0.0f) time = 0.0f;
    }

    float strength() const
    {
        if (duration <= 1e-6f) return 0.0f;
        float t = time / duration;
        return amplitude * t * t;
    }

    void offset(float global_time, float& out_x, float& out_y) const
    {
        float s = strength();
        if (s <= 0.0f) { out_x = 0.0f; out_y = 0.0f; return; }

        float nx = std::sin(global_time * 37.0f) + 0.5f * std::sin(global_time * 91.0f);
        float ny = std::cos(global_time * 43.0f) + 0.5f * std::cos(global_time * 79.0f);

        out_x = s * nx;
        out_y = s * ny;
    }
};
