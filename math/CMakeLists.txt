add_library(math
    "vec4.hpp"
    "vec4.cpp"
    "mat4.hpp"
    "mat4.cpp"
    "quaternion.hpp"
    "quaternion.cpp"
    "axis_angle.hpp"
    "axis_angle.cpp"
    "polar.hpp"
    "polar.cpp"
    )

# (optional) Search and ling with 3rd party libraries
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
target_link_libraries(math PUBLIC lodepng glad::glad glfw)
