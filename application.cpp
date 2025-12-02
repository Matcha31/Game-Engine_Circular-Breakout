#include "application.hpp"
#include "glad/glad.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include <cassert>

static GLuint load_shader(std::filesystem::path const& path, GLenum const shader_type)
{
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

    GLint status = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (!status) {
        GLint logLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);
        std::string log(logLen, '\0');
        glGetShaderInfoLog(shader, logLen, nullptr, log.data());
        std::cerr << "Shader compile error in " << path << ":\n" << log << std::endl;
        assert(false);
    }

    return shader;
}

Application::Application(int initial_width, int initial_height, std::vector<std::string> arguments)
    : IApplication(initial_width, initial_height, arguments)
    , vertex_shader(load_shader(lecture_folder_path / "data" / "shaders" / "axis.vert", GL_VERTEX_SHADER))
    , fragment_shader(load_shader(lecture_folder_path / "data" / "shaders" / "axis.frag", GL_FRAGMENT_SHADER))
    , shader_program([](GLuint vs, GLuint fs) {
        GLuint program = glCreateProgram();
        assert(glGetError() == 0U && program != 0);

        glAttachShader(program, vs);
        assert(glGetError() == 0U);
        glAttachShader(program, fs);
        assert(glGetError() == 0U);
        glLinkProgram(program);
        assert(glGetError() == 0U);

        GLint status = 0;
        glGetProgramiv(program, GL_LINK_STATUS, &status);
        if (!status) {
            GLint logLen = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLen);
            std::string log(logLen, '\0');
            glGetProgramInfoLog(program, logLen, nullptr, log.data());
            std::cerr << "Program link error:\n" << log << std::endl;
            assert(false);
        }

        glDetachShader(program, vs);
        assert(glGetError() == 0U);
        glDetachShader(program, fs);
        assert(glGetError() == 0U);

        return program;
    }(vertex_shader, fragment_shader))
    , axes_vao(0)
    , axes_vbo(0)
    , ground_vao(0)
    , ground_vbo(0)
    , ground_ibo(0)
    , ground_index_count(0)
    , u_model_loc(-1)
    , u_view_loc(-1)
    , u_proj_loc(-1)
{
    glViewport(0, 0, width, height);
    camera.setViewportSize(width, height);

    glEnable(GL_DEPTH_TEST);

    struct Vertex {
        float x, y, z;
        float r, g, b;
    };

    std::vector<Vertex> const axes_vertices = {
        {0.f, 0.f, 0.f, 1.f, 0.f, 0.f},
        {1.f, 0.f, 0.f, 1.f, 0.f, 0.f},

        {0.f, 0.f, 0.f, 0.f, 1.f, 0.f},
        {0.f, 1.f, 0.f, 0.f, 1.f, 0.f},

        {0.f, 0.f, 0.f, 0.f, 0.f, 1.f},
        {0.f, 0.f, 1.f, 0.f, 0.f, 1.f},
    };

    glGenVertexArrays(1, &axes_vao);
    assert(glGetError() == 0U);
    glBindVertexArray(axes_vao);
    assert(glGetError() == 0U);

    glGenBuffers(1, &axes_vbo);
    assert(glGetError() == 0U);
    glBindBuffer(GL_ARRAY_BUFFER, axes_vbo);
    assert(glGetError() == 0U);
    glBufferData(GL_ARRAY_BUFFER, axes_vertices.size() * sizeof(Vertex), axes_vertices.data(), GL_STATIC_DRAW);
    assert(glGetError() == 0U);

    glEnableVertexAttribArray(0);
    assert(glGetError() == 0U);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    assert(glGetError() == 0U);

    glEnableVertexAttribArray(1);
    assert(glGetError() == 0U);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
    assert(glGetError() == 0U);

    glBindVertexArray(0);

    float ground_radius = 2.0f;
    int segments = 64;
    std::vector<Vertex> ground_vertices;
    std::vector<GLuint> ground_indices;

    ground_vertices.push_back({0.f, 0.f, 0.f, 0.2f, 0.2f, 0.2f});
    for (int i = 0; i < segments; ++i) {
        float angle = (2.0f * 3.14159265358979323846f * i) / static_cast<float>(segments);
        float x = ground_radius * std::cos(angle);
        float y = ground_radius * std::sin(angle);
        float z = 0.0f;
        ground_vertices.push_back({x, y, z, 0.3f, 0.3f, 0.3f});
    }

    for (int i = 0; i < segments; ++i) {
        GLuint centerIndex = 0;
        GLuint i0 = static_cast<GLuint>(i + 1);
        GLuint i1 = static_cast<GLuint>((i + 1) % segments + 1);
        ground_indices.push_back(centerIndex);
        ground_indices.push_back(i0);
        ground_indices.push_back(i1);
    }

    ground_index_count = static_cast<GLsizei>(ground_indices.size());

    glGenVertexArrays(1, &ground_vao);
    assert(glGetError() == 0U);
    glBindVertexArray(ground_vao);
    assert(glGetError() == 0U);

    glGenBuffers(1, &ground_vbo);
    assert(glGetError() == 0U);
    glBindBuffer(GL_ARRAY_BUFFER, ground_vbo);
    assert(glGetError() == 0U);
    glBufferData(GL_ARRAY_BUFFER, ground_vertices.size() * sizeof(Vertex), ground_vertices.data(), GL_STATIC_DRAW);
    assert(glGetError() == 0U);

    glEnableVertexAttribArray(0);
    assert(glGetError() == 0U);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    assert(glGetError() == 0U);

    glEnableVertexAttribArray(1);
    assert(glGetError() == 0U);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
    assert(glGetError() == 0U);

    glGenBuffers(1, &ground_ibo);
    assert(glGetError() == 0U);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ground_ibo);
    assert(glGetError() == 0U);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, ground_indices.size() * sizeof(GLuint), ground_indices.data(), GL_STATIC_DRAW);
    assert(glGetError() == 0U);

    glBindVertexArray(0);

    glUseProgram(shader_program);
    assert(glGetError() == 0U);

    u_model_loc = glGetUniformLocation(shader_program, "uModel");
    u_view_loc  = glGetUniformLocation(shader_program, "uView");
    u_proj_loc  = glGetUniformLocation(shader_program, "uProj");

    assert(u_model_loc != -1);
    assert(u_view_loc  != -1);
    assert(u_proj_loc  != -1);

    glClearColor(0.05f, 0.05f, 0.08f, 1.0f);
}

Application::~Application()
{
    glDeleteBuffers(1, &ground_ibo);
    glDeleteBuffers(1, &ground_vbo);
    glDeleteVertexArrays(1, &ground_vao);

    glDeleteBuffers(1, &axes_vbo);
    glDeleteVertexArrays(1, &axes_vao);

    glDeleteProgram(shader_program);
    glDeleteShader(fragment_shader);
    glDeleteShader(vertex_shader);
}

void Application::update(float delta)
{
    (void)delta;
}

void Application::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shader_program);
    assert(glGetError() == 0U);

    Mat4 model = Mat4::identity();
    Mat4 view  = camera.viewMatrix();
    Mat4 proj  = camera.projectionMatrix();

    glUniformMatrix4fv(u_model_loc, 1, GL_FALSE, model.m);
    assert(glGetError() == 0U);
    glUniformMatrix4fv(u_view_loc, 1, GL_FALSE, view.m);
    assert(glGetError() == 0U);
    glUniformMatrix4fv(u_proj_loc, 1, GL_FALSE, proj.m);
    assert(glGetError() == 0U);

    glBindVertexArray(ground_vao);
    assert(glGetError() == 0U);
    glDrawElements(GL_TRIANGLES, ground_index_count, GL_UNSIGNED_INT, nullptr);
    assert(glGetError() == 0U);

    glBindVertexArray(axes_vao);
    assert(glGetError() == 0U);
    glDrawArrays(GL_LINES, 0, 6);
    assert(glGetError() == 0U);

    glBindVertexArray(0);
}

void Application::render_ui() {}

void Application::on_resize(int width, int height)
{
    IApplication::on_resize(width, height);
    glViewport(0, 0, width, height);
    camera.setViewportSize(width, height);
}

void Application::on_mouse_move(double x, double y)
{
    (void)x;
    (void)y;
}

void Application::on_mouse_button(int button, int action, int mods)
{
    (void)button;
    (void)action;
    (void)mods;
}

void Application::on_key_pressed(int key, int scancode, int action, int mods)
{
    (void)scancode;
    (void)mods;

    if (action != GLFW_PRESS)
        return;

    switch (key) {
    case GLFW_KEY_1:
        camera.setMode(Camera::Mode::Perspective);
        break;
    case GLFW_KEY_2:
        camera.setMode(Camera::Mode::OrthoTop);
        break;
    case GLFW_KEY_R:
        camera.getFrame().position    = Vec4(0.0f, 0.0f, 5.0f, 1.0f);
        camera.getFrame().orientation = Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
        break;
    default:
        break;
    }
}
