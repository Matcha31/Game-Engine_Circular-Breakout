#pragma once

struct InputState
{
    bool left_down = false;
    bool right_down = false;

    bool pause_pressed = false;
    bool reset_pressed = false;
    bool launch_pressed = false;

    void begin_frame();
    void handle_key(int key, int action);
};
