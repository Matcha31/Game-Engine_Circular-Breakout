add_library(physics
    "collision.hpp"
    "collision.cpp"
    )

# (optional) Search and ling with 3rd party libraries
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
target_link_libraries(physics PUBLIC lodepng glad::glad glfw)
