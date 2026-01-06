add_library(graphics
    "frame.hpp"
    "frame.cpp"
    "camera.hpp"
    "camera.cpp"
    "mesh.hpp"
    "mesh.cpp"
    "procedural.hpp"
    "procedural.cpp"
    )

# (optional) Search and ling with 3rd party libraries
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
target_link_libraries(graphics PUBLIC lodepng glad::glad glfw)
