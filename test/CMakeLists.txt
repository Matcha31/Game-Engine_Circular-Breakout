find_package(GTest CONFIG REQUIRED)

add_executable(PA199_project_tests
    "vec4_test.cpp"
    "mat4_test.cpp"
    "tests.cpp"
    )

# Search and ling with 3rd party libraries
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)

target_link_libraries(PA199_project_tests PUBLIC
  GTest::gtest GTest::gtest_main lodepng glad::glad glfw

  math
)

# Exposes the test (e.g., for Visual Studio).
add_test(NAME PA199_project_tests COMMAND PA199_project_tests)

# Installing the tests. Do not modify!
install(TARGETS PA199_project_tests DESTINATION .)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/gtest.dll OPTIONAL DESTINATION .)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/gtest.dll OPTIONAL DESTINATION .)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/gtest_main.dll OPTIONAL DESTINATION .)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/gtest_main.dll OPTIONAL DESTINATION .)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/gtest_main.dll OPTIONAL DESTINATION .)
