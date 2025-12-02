// ################################################################################
// Common Framework for Computer Graphics Courses at FI MUNI.
// 
// Copyright (c) 2021-2022 Visitlab (https://visitlab.fi.muni.cz)
// All rights reserved.
// ################################################################################

#include "application.hpp"
#include "glad/glad.h"
#include "lodepng.h"
#include <filesystem>
#include <iostream>

static GLuint load_shader(std::filesystem::path const& path, GLenum const shader_type)
{
    std::filesystem::path const current = std::filesystem::current_path();
    GLuint const shader = glCreateShader(shader_type);
    assert(glGetError() == 0U && shader != 0);
    std::ifstream ifs(path);
    assert(ifs.is_open());
    std::string const str((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    char const* code = str.c_str();
    glShaderSource(shader, 1, &code, nullptr);
    assert(glGetError() == 0U);
    glCompileShader(shader);
    assert(glGetError() == 0U);
    return shader;
}

static GLuint load_texture(std::filesystem::path const& path)
{
    std::vector<unsigned char> texels;
    unsigned int width, height;
    unsigned int error_code = lodepng::decode(texels, width, height, path.string(), LCT_RGBA);
    assert(error_code == 0);

    //flip the image vertically
    for (unsigned int lo = 0, hi = height - 1; lo < hi; ++lo, --hi)
        for (unsigned int*  lo_ptr = (unsigned int*)texels.data() + lo * width,
                         *  lo_end = lo_ptr + width,
                         *  hi_ptr = (unsigned int*)texels.data() + hi * width;
                lo_ptr != lo_end; ++lo_ptr, ++hi_ptr)
            std::swap(*lo_ptr, *hi_ptr);

    GLuint texture;
	glGenTextures(1, &texture);
    assert(glGetError() == 0U);
	glBindTexture(GL_TEXTURE_2D, texture);
    assert(glGetError() == 0U);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)texels.data());
    assert(glGetError() == 0U);
    glGenerateMipmap(GL_TEXTURE_2D);
    assert(glGetError() == 0U);
    return texture;
}

// ----------------------------------------------------------------------------
// Constructors & Destructors
// ----------------------------------------------------------------------------

Application::Application(int initial_width, int initial_height, std::vector<std::string> arguments)
    : IApplication(initial_width, initial_height, arguments)
    , vertex_shader(load_shader(lecture_folder_path / "data" / "shaders" / "sample.vert", GL_VERTEX_SHADER))
    , fragment_shader(load_shader(lecture_folder_path / "data" / "shaders" / "sample.frag", GL_FRAGMENT_SHADER))
    , shader_program([](GLuint const  vertex_shader, GLuint const  fragment_shader) -> GLuint {
            GLuint const  shader_program = glCreateProgram();
            assert(glGetError() == 0U && shader_program != 0);
            glAttachShader(shader_program,vertex_shader);
            assert(glGetError() == 0U);
            glAttachShader(shader_program,fragment_shader);
            assert(glGetError() == 0U);
            glLinkProgram(shader_program);
            assert(glGetError() == 0U);
            glDetachShader(shader_program, vertex_shader);
            assert(glGetError() == 0U);
            glDetachShader(shader_program, fragment_shader);
            assert(glGetError() == 0U);
            return shader_program;
            }(vertex_shader, fragment_shader))
    , vertex_arrays([]() -> GLuint {
            GLuint vertex_arrays;
            glGenVertexArrays(1, &vertex_arrays);
            glBindVertexArray(vertex_arrays);
            return vertex_arrays;
            }())
    , vertex_buffer([]() -> GLuint {
            struct Vertex { float x,y,z,u,v; };
            std::vector<Vertex> const vertices {
                { -0.5f, -0.125f, 0.0f,       0.0f, 0.0f },
                {  0.5f, -0.125f, 0.0f,       1.0f, 0.0f },
                {  0.5f,  0.125f, 0.0f,       1.0f, 1.0f },
                { -0.5f,  0.125f, 0.0f,       0.0f, 1.0f },
            };
            GLuint vertex_buffer;
            glGenBuffers(1, &vertex_buffer);
            assert(glGetError() == 0U);
            glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
            assert(glGetError() == 0U);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices.front(), GL_STATIC_DRAW);
            assert(glGetError() == 0U);
            glEnableVertexAttribArray(0);
            assert(glGetError() == 0U);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
            assert(glGetError() == 0U);
            glEnableVertexAttribArray(1);
            assert(glGetError() == 0U);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3*sizeof(float)));
            assert(glGetError() == 0U);
            return vertex_buffer;
            }())
    , index_buffer([]() -> GLuint {
            std::vector<GLuint> const indices { 0U, 1U, 2U,     0U, 2U, 3U };
            GLuint index_buffer;
            glGenBuffers(1, &index_buffer);
            assert(glGetError() == 0U);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
            assert(glGetError() == 0U);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices.front(), GL_STATIC_DRAW);
            assert(glGetError() == 0U);
            return index_buffer;
            }())
    , texture(load_texture(lecture_folder_path / "data" / "textures" / "you_win.png"))
{
    glViewport(0, 0, width, height);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

Application::~Application()
{

    glDeleteVertexArrays(1U, &vertex_arrays);
    glDeleteTextures(1, &texture);
    glDeleteProgram(shader_program);
    glDeleteShader(fragment_shader);
    glDeleteShader(vertex_shader);
}

// ----------------------------------------------------------------------------
// Methods
// ----------------------------------------------------------------------------

void Application::update(float delta) {}

void Application::render() {
    // Sets the clear color.
    glClearColor(red, green, blue, 1.0f);
    // Clears the window using the above color.
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shader_program);
    assert(glGetError() == 0U);
    glBindVertexArray(vertex_arrays);
    assert(glGetError() == 0U);
    glActiveTexture(GL_TEXTURE0);
    assert(glGetError() == 0U);
    glBindTexture(GL_TEXTURE_2D, texture);
    assert(glGetError() == 0U);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    assert(glGetError() == 0U);
}

void Application::render_ui() {}

void Application::on_resize(int width, int height) {
    // Calls the default implementation to set the class variables.
    IApplication::on_resize(width, height);
    // Changes the viewport.
    glViewport(0, 0, width, height);
}

void Application::on_mouse_move(double x, double y) {}

void Application::on_mouse_button(int button, int action, int mods) {}

void Application::on_key_pressed(int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        red = 0;
        green = 0;
        blue = 0;
        switch (key) {
        case GLFW_KEY_R:
            red = 1;
            break;
        case GLFW_KEY_G:
            green = 1;
            break;
        case GLFW_KEY_B:
            blue = 1;
            break;
        }
    }
}
