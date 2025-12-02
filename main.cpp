// ################################################################################
// Common Framework for Computer Graphics Courses at FI MUNI.
// 
// Copyright (c) 2021-2022 Visitlab (https://visitlab.fi.muni.cz)
// All rights reserved.
// ################################################################################

#include <string>
#include <vector>
#include "application.hpp"
#include "manager.h"
#include "math/vec4.hpp"

void test_vec4_basic()
{
    Vec4 a(1.f, 2.f, 3.f, 1.f);
    Vec4 b(0.f, 1.f, 0.f, 0.f);

    auto c = Vec4::cross3(a, b);
    auto d = Vec4::dot(a, b);

    // just to check it's compiling/running
    (void)c;
    (void)d;
}

int main(int argc, char** argv) {
    constexpr int initial_width = 1024;
    constexpr int initial_height = 768;

    const std::vector<std::string> arguments(argv, argv + argc);

    ApplicationManager manager;
    manager.init(initial_width, initial_height, "PA199 Project", 4, 5);
    if (!manager.is_fail()) {
        // Note that the application has to be created after the manager is initialized.
        Application application(initial_width, initial_height, arguments);
        manager.run(application);

        // Free the entire application before terminating glfw. If this were done in the wrong order
        // application may crash on calling OpenGL (Delete*) calls after destruction of a context.
        // Freeing is done implicitly by enclosing this part of code in block {}.
    }
    test_vec4_basic();

    manager.terminate();
    return 0;
}
