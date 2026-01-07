add_library(game
    "game_state.hpp"
    "game_state.cpp"
    "input.hpp"
    "input.cpp"
    "bricks.hpp"
    "bricks.cpp"
    )

# (optional) Search and ling with 3rd party libraries
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
target_link_libraries(game PUBLIC lodepng glad::glad glfw)
