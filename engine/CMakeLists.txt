add_library(engine
    "frame.hpp"
    "frame.cpp"
    "camera.hpp"
    "camera.cpp"
    )

# (optional) Search and ling with 3rd party libraries
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
target_link_libraries(engine PUBLIC lodepng glad::glad glfw)
