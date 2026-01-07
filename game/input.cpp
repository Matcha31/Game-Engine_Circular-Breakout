#include "input.hpp"
#include "GLFW/glfw3.h"

void InputState::begin_frame()
{
    pause_pressed = false;
    reset_pressed = false;
    launch_pressed = false;
}

void InputState::handle_key(int key, int action)
{
    auto set_down = [&](bool& flag)
    {
        if (action == GLFW_PRESS || action == GLFW_REPEAT) flag = true;
        if (action == GLFW_RELEASE) flag = false;
    };

    if (key == GLFW_KEY_LEFT || key == GLFW_KEY_A)
    {
        set_down(left_down);
        return;
    }

    if (key == GLFW_KEY_RIGHT || key == GLFW_KEY_D)
    {
        set_down(right_down);
        return;
    }

    if (action != GLFW_PRESS)
        return;

    if (key == GLFW_KEY_P) pause_pressed = true;
    if (key == GLFW_KEY_R) reset_pressed = true;
    if (key == GLFW_KEY_SPACE) launch_pressed = true;
}
