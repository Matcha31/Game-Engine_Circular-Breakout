# This is a tutorial file. Feel free to remove it.

add_library(some_my_library
    "some_library_file.hpp"
    "some_library_file.cpp"
    )

# (optional) Search and ling with 3rd party libraries
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
target_link_libraries(some_my_library PUBLIC lodepng glad::glad glfw)
