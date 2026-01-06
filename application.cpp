#include "application.hpp"

#include "glad/glad.h"

#include "graphics/camera.hpp"
#include "graphics/mesh.hpp"
#include "graphics/procedural.hpp"
#include "graphics/texture2d.hpp"

#include "math/axis_angle.hpp"
#include "math/quaternion.hpp"
#include "math/mat4.hpp"
#include "math/vec4.hpp"

#include <cassert>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace
{
    GLuint compileShader(const std::filesystem::path& path, GLenum type)
    {
        GLuint shader = glCreateShader(type);
        assert(glGetError() == 0U && shader != 0);

        std::ifstream ifs(path);
        assert(ifs.is_open());
        std::string src((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

        const char* code = src.c_str();
        glShaderSource(shader, 1, &code, nullptr);
        glCompileShader(shader);

        GLint ok = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
        if (!ok)
        {
            GLint len = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
            std::string log(len, '\0');
            glGetShaderInfoLog(shader, len, nullptr, log.data());
            std::cerr << "Shader compile error in " << path << ":\n" << log << std::endl;
            assert(false);
        }

        return shader;
    }

    GLuint linkProgram(GLuint vs, GLuint fs)
    {
        GLuint program = glCreateProgram();
        assert(glGetError() == 0U && program != 0);

        glAttachShader(program, vs);
        glAttachShader(program, fs);
        glLinkProgram(program);

        GLint ok = 0;
        glGetProgramiv(program, GL_LINK_STATUS, &ok);
        if (!ok)
        {
            GLint len = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
            std::string log(len, '\0');
            glGetProgramInfoLog(program, len, nullptr, log.data());
            std::cerr << "Program link error:\n" << log << std::endl;
            assert(false);
        }

        glDetachShader(program, vs);
        glDetachShader(program, fs);

        return program;
    }

    struct AxisVertex
    {
        float x, y, z;
        float r, g, b;
    };

    void buildAxes(GLuint& vao, GLuint& vbo, float axisLen)
    {
        std::vector<AxisVertex> vertices = {
            {0.f, 0.f, 0.f, 1.f, 0.f, 0.f},
            {axisLen, 0.f, 0.f, 1.f, 0.f, 0.f},

            {0.f, 0.f, 0.f, 0.f, 1.f, 0.f},
            {0.f, axisLen, 0.f, 0.f, 1.f, 0.f},

            {0.f, 0.f, 0.f, 0.f, 0.f, 1.f},
            {0.f, 0.f, axisLen, 0.f, 0.f, 1.f},
        };

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(AxisVertex), vertices.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(AxisVertex), (void*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(AxisVertex), (void*)(3 * sizeof(float)));

        glBindVertexArray(0);
    }

    void buildGroundDiscTextured(Mesh& mesh, float radius, int segments, float z, float cr, float cg, float cb, float uvTiling)
    {
        std::vector<Mesh::Vertex> v;
        std::vector<GLuint> idx;

        v.reserve(static_cast<size_t>(segments + 1));
        idx.reserve(static_cast<size_t>(segments * 3));

        auto uvFromXY = [&](float x, float y) -> std::pair<float,float> {
            float u = (x / (2.0f * radius) + 0.5f) * uvTiling;
            float w = (y / (2.0f * radius) + 0.5f) * uvTiling;
            return {u, w};
        };

        {
            auto uv = uvFromXY(0.0f, 0.0f);
            v.push_back({0.f, 0.f, z, 0.f, 0.f, 1.f, cr, cg, cb, uv.first, uv.second});
        }

        for (int i = 0; i < segments; ++i)
        {
            float a = (2.0f * M_PI * static_cast<float>(i)) / static_cast<float>(segments);
            float x = radius * std::cos(a);
            float y = radius * std::sin(a);
            auto uv = uvFromXY(x, y);
            v.push_back({x, y, z, 0.f, 0.f, 1.f, cr, cg, cb, uv.first, uv.second});
        }

        for (int i = 0; i < segments; ++i)
        {
            GLuint c = 0;
            GLuint i0 = static_cast<GLuint>(i + 1);
            GLuint i1 = static_cast<GLuint>((i + 1) % segments + 1);
            idx.push_back(c);
            idx.push_back(i0);
            idx.push_back(i1);
        }

        mesh.upload(v, idx);
    }

    void buildSphere(Mesh& mesh, float radius, int stacks, int slices, float cr, float cg, float cb)
    {
        std::vector<Mesh::Vertex> v;
        std::vector<GLuint> idx;

        v.reserve(static_cast<size_t>((stacks + 1) * (slices + 1)));
        idx.reserve(static_cast<size_t>(stacks * slices * 6));

        for (int i = 0; i <= stacks; ++i)
        {
            float t = static_cast<float>(i) / static_cast<float>(stacks);
            float theta = t * M_PI;
            float st = std::sin(theta);
            float ct = std::cos(theta);

            for (int j = 0; j <= slices; ++j)
            {
                float s = static_cast<float>(j) / static_cast<float>(slices);
                float phi = s * 2.0f * M_PI;
                float sp = std::sin(phi);
                float cp = std::cos(phi);

                float x = radius * st * cp;
                float y = radius * st * sp;
                float z = radius * ct;

                float nx = (radius != 0.0f) ? (x / radius) : 0.0f;
                float ny = (radius != 0.0f) ? (y / radius) : 0.0f;
                float nz = (radius != 0.0f) ? (z / radius) : 1.0f;

                float u = s;
                float w = 1.0f - t;

                v.push_back({x, y, z, nx, ny, nz, cr, cg, cb, u, w});
            }
        }

        int row = slices + 1;
        for (int i = 0; i < stacks; ++i)
        {
            for (int j = 0; j < slices; ++j)
            {
                int i0 = i * row + j;
                int i1 = i0 + 1;
                int i2 = (i + 1) * row + j;
                int i3 = i2 + 1;

                idx.push_back((GLuint)i0);
                idx.push_back((GLuint)i2);
                idx.push_back((GLuint)i1);

                idx.push_back((GLuint)i1);
                idx.push_back((GLuint)i2);
                idx.push_back((GLuint)i3);
            }
        }

        mesh.upload(v, idx);
    }

    void buildPaddleAndBrickMeshes(Mesh& paddle, Mesh& brick)
    {
        {
            std::vector<Mesh::Vertex> v;
            std::vector<GLuint> idx;

            float innerR = 1.40f;
            float outerR = 1.60f;
            float halfSpan = 0.25f;
            int segments = 48;
            float z = 0.02f;

            procedural::makeRingSegmentFlat(innerR, outerR, -halfSpan, +halfSpan, segments, z,
                                            0.90f, 0.60f, 0.20f, v, idx);

            for (auto& vert : v)
            {
                vert.u = 0.0f;
                vert.v = 0.0f;
            }

            paddle.upload(v, idx);
        }

        {
            std::vector<Mesh::Vertex> v;
            std::vector<GLuint> idx;

            float innerR = 1.75f;
            float outerR = 1.90f;
            float halfSpan = 0.10f;
            int segments = 24;
            float z = 0.02f;

            procedural::makeRingSegmentFlat(innerR, outerR, -halfSpan, +halfSpan, segments, z,
                                            0.80f, 0.30f, 0.30f, v, idx);

            for (auto& vert : v)
            {
                vert.u = 0.0f;
                vert.v = 0.0f;
            }

            brick.upload(v, idx);
        }
    }

    Mat4 rotationZ(float angle)
    {
        AxisAngle aa(Vec4(0.0f, 0.0f, 1.0f, 0.0f), angle);
        return aa.toQuaternion().toRotationMatrix();
    }

    Vec4 normalize3(Vec4 v)
    {
        float len = std::sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
        if (len == 0.0f) return Vec4(0.0f, 0.0f, 1.0f, 0.0f);
        return Vec4(v.x/len, v.y/len, v.z/len, 0.0f);
    }

    void setMat4(GLint loc, const Mat4& m)
    {
        glUniformMatrix4fv(loc, 1, GL_FALSE, m.m);
    }

    void buildScreenQuad(GLuint& vao, GLuint& vbo)
    {
        struct V { float x,y,u,v; };
        V verts[6] = {
            {-1.f, -1.f, 0.f, 0.f},
            { 1.f, -1.f, 1.f, 0.f},
            { 1.f,  1.f, 1.f, 1.f},

            {-1.f, -1.f, 0.f, 0.f},
            { 1.f,  1.f, 1.f, 1.f},
            {-1.f,  1.f, 0.f, 1.f},
        };

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(V), (void*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(V), (void*)(2 * sizeof(float)));

        glBindVertexArray(0);
    }
}

static Texture2D g_groundTex;
static Texture2D g_pausedTex;
static Texture2D g_winTex;
static Texture2D g_gameOverTex;

static GLuint g_texlit_vs = 0, g_texlit_fs = 0, g_texlit_prog = 0;
static GLuint g_screen_vs = 0, g_screen_fs = 0, g_screen_prog = 0;

static GLint g_texlit_uModel = -1, g_texlit_uView = -1, g_texlit_uProj = -1;
static GLint g_texlit_uLightDir = -1, g_texlit_uCameraPos = -1;
static GLint g_texlit_uAmbient = -1, g_texlit_uSpecular = -1, g_texlit_uShininess = -1;
static GLint g_texlit_uTex = -1;

static GLint g_screen_uTex = -1;

static GLuint g_screenVao = 0, g_screenVbo = 0;

static bool g_showPaused = false;

Application::Application(int initial_width, int initial_height, std::vector<std::string> arguments)
    : IApplication(initial_width, initial_height, arguments)
    , camera()
    , paddle_mesh()
    , brick_mesh()
    , ground_mesh()
    , sphere_mesh()
    , axis_vertex_shader(0)
    , axis_fragment_shader(0)
    , axis_program(0)
    , lit_vertex_shader(0)
    , lit_fragment_shader(0)
    , lit_program(0)
    , axes_vao(0)
    , axes_vbo(0)
    , axis_u_model(-1)
    , axis_u_view(-1)
    , axis_u_proj(-1)
    , lit_u_model(-1)
    , lit_u_view(-1)
    , lit_u_proj(-1)
    , lit_u_light_dir(-1)
    , lit_u_camera_pos(-1)
    , lit_u_ambient(-1)
    , lit_u_specular(-1)
    , lit_u_shininess(-1)
{
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    camera.setViewportSize(width, height);

    axis_vertex_shader = compileShader(lecture_folder_path / "data" / "shaders" / "axis.vert", GL_VERTEX_SHADER);
    axis_fragment_shader = compileShader(lecture_folder_path / "data" / "shaders" / "axis.frag", GL_FRAGMENT_SHADER);
    axis_program = linkProgram(axis_vertex_shader, axis_fragment_shader);

    lit_vertex_shader = compileShader(lecture_folder_path / "data" / "shaders" / "lit.vert", GL_VERTEX_SHADER);
    lit_fragment_shader = compileShader(lecture_folder_path / "data" / "shaders" / "lit.frag", GL_FRAGMENT_SHADER);
    lit_program = linkProgram(lit_vertex_shader, lit_fragment_shader);

    g_texlit_vs = compileShader(lecture_folder_path / "data" / "shaders" / "texlit.vert", GL_VERTEX_SHADER);
    g_texlit_fs = compileShader(lecture_folder_path / "data" / "shaders" / "texlit.frag", GL_FRAGMENT_SHADER);
    g_texlit_prog = linkProgram(g_texlit_vs, g_texlit_fs);

    g_screen_vs = compileShader(lecture_folder_path / "data" / "shaders" / "screen.vert", GL_VERTEX_SHADER);
    g_screen_fs = compileShader(lecture_folder_path / "data" / "shaders" / "screen.frag", GL_FRAGMENT_SHADER);
    g_screen_prog = linkProgram(g_screen_vs, g_screen_fs);

    glUseProgram(axis_program);
    axis_u_model = glGetUniformLocation(axis_program, "uModel");
    axis_u_view  = glGetUniformLocation(axis_program, "uView");
    axis_u_proj  = glGetUniformLocation(axis_program, "uProj");
    assert(axis_u_model != -1 && axis_u_view != -1 && axis_u_proj != -1);

    glUseProgram(lit_program);
    lit_u_model = glGetUniformLocation(lit_program, "uModel");
    lit_u_view  = glGetUniformLocation(lit_program, "uView");
    lit_u_proj  = glGetUniformLocation(lit_program, "uProj");
    lit_u_light_dir  = glGetUniformLocation(lit_program, "uLightDir");
    lit_u_camera_pos = glGetUniformLocation(lit_program, "uCameraPos");
    lit_u_ambient    = glGetUniformLocation(lit_program, "uAmbient");
    lit_u_specular   = glGetUniformLocation(lit_program, "uSpecular");
    lit_u_shininess  = glGetUniformLocation(lit_program, "uShininess");
    assert(lit_u_model != -1 && lit_u_view != -1 && lit_u_proj != -1);
    assert(lit_u_light_dir != -1 && lit_u_camera_pos != -1);
    assert(lit_u_ambient != -1 && lit_u_specular != -1 && lit_u_shininess != -1);

    glUseProgram(g_texlit_prog);
    g_texlit_uModel = glGetUniformLocation(g_texlit_prog, "uModel");
    g_texlit_uView  = glGetUniformLocation(g_texlit_prog, "uView");
    g_texlit_uProj  = glGetUniformLocation(g_texlit_prog, "uProj");
    g_texlit_uLightDir  = glGetUniformLocation(g_texlit_prog, "uLightDir");
    g_texlit_uCameraPos = glGetUniformLocation(g_texlit_prog, "uCameraPos");
    g_texlit_uAmbient    = glGetUniformLocation(g_texlit_prog, "uAmbient");
    g_texlit_uSpecular   = glGetUniformLocation(g_texlit_prog, "uSpecular");
    g_texlit_uShininess  = glGetUniformLocation(g_texlit_prog, "uShininess");
    g_texlit_uTex        = glGetUniformLocation(g_texlit_prog, "uTex");
    assert(g_texlit_uModel != -1 && g_texlit_uView != -1 && g_texlit_uProj != -1);
    assert(g_texlit_uLightDir != -1 && g_texlit_uCameraPos != -1);
    assert(g_texlit_uAmbient != -1 && g_texlit_uSpecular != -1 && g_texlit_uShininess != -1);
    assert(g_texlit_uTex != -1);

    glUseProgram(g_screen_prog);
    g_screen_uTex = glGetUniformLocation(g_screen_prog, "uTex");
    assert(g_screen_uTex != -1);

    buildAxes(axes_vao, axes_vbo, 3.0f);

    buildGroundDiscTextured(ground_mesh, 2.0f, 128, 0.0f, 1.0f, 1.0f, 1.0f, 4.0f);
    buildSphere(sphere_mesh, 0.20f, 24, 48, 0.90f, 0.90f, 0.90f);
    buildPaddleAndBrickMeshes(paddle_mesh, brick_mesh);

    buildScreenQuad(g_screenVao, g_screenVbo);

    bool okGround = g_groundTex.loadPNG(lecture_folder_path / "data" / "textures" / "ground.png");
    bool okPaused = g_pausedTex.loadPNG(lecture_folder_path / "data" / "textures" / "pause.png", false);
    bool okWin    = g_winTex.loadPNG(lecture_folder_path / "data" / "textures" / "you_win.png", false);
    bool okOver   = g_gameOverTex.loadPNG(lecture_folder_path / "data" / "textures" / "game_over.png", false);

    if (!okGround) std::cerr << "Failed to load ground texture\n";
    if (!okPaused) std::cerr << "Failed to load paused texture\n";
    if (!okWin)    std::cerr << "Failed to load you_win texture\n";
    if (!okOver)   std::cerr << "Failed to load game_over texture\n";

    glClearColor(0.05f, 0.05f, 0.08f, 1.0f);
}

Application::~Application()
{
    if (axes_vbo) glDeleteBuffers(1, &axes_vbo);
    if (axes_vao) glDeleteVertexArrays(1, &axes_vao);

    if (g_screenVbo) glDeleteBuffers(1, &g_screenVbo);
    if (g_screenVao) glDeleteVertexArrays(1, &g_screenVao);

    if (g_screen_prog) glDeleteProgram(g_screen_prog);
    if (g_texlit_prog) glDeleteProgram(g_texlit_prog);

    if (lit_program) glDeleteProgram(lit_program);
    if (axis_program) glDeleteProgram(axis_program);

    if (g_screen_fs) glDeleteShader(g_screen_fs);
    if (g_screen_vs) glDeleteShader(g_screen_vs);

    if (g_texlit_fs) glDeleteShader(g_texlit_fs);
    if (g_texlit_vs) glDeleteShader(g_texlit_vs);

    if (lit_fragment_shader) glDeleteShader(lit_fragment_shader);
    if (lit_vertex_shader) glDeleteShader(lit_vertex_shader);

    if (axis_fragment_shader) glDeleteShader(axis_fragment_shader);
    if (axis_vertex_shader) glDeleteShader(axis_vertex_shader);
}

void Application::update(float delta)
{
    (void)delta;
}

void Application::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Mat4 view = camera.viewMatrix();
    Mat4 proj = camera.projectionMatrix();

    Vec4 camPos = camera.getFrame().position;
    Vec4 lightDir = normalize3(Vec4(1.0f, 1.0f, 2.0f, 0.0f));

    glUseProgram(g_texlit_prog);

    setMat4(g_texlit_uView, view);
    setMat4(g_texlit_uProj, proj);

    glUniform3f(g_texlit_uLightDir, lightDir.x, lightDir.y, lightDir.z);
    glUniform3f(g_texlit_uCameraPos, camPos.x, camPos.y, camPos.z);

    glUniform1f(g_texlit_uAmbient, 0.20f);
    glUniform1f(g_texlit_uSpecular, 0.50f);
    glUniform1f(g_texlit_uShininess, 48.0f);

    glUniform1i(g_texlit_uTex, 0);
    g_groundTex.bind(GL_TEXTURE0);

    {
        Mat4 model = Mat4::identity();
        setMat4(g_texlit_uModel, model);
        ground_mesh.draw();
    }

    glUseProgram(lit_program);

    setMat4(lit_u_view, view);
    setMat4(lit_u_proj, proj);

    glUniform3f(lit_u_light_dir, lightDir.x, lightDir.y, lightDir.z);
    glUniform3f(lit_u_camera_pos, camPos.x, camPos.y, camPos.z);

    glUniform1f(lit_u_ambient, 0.20f);
    glUniform1f(lit_u_specular, 0.60f);
    glUniform1f(lit_u_shininess, 48.0f);

    {
        Mat4 model = Mat4::identity();
        model(2, 3) = 0.20f;
        setMat4(lit_u_model, model);
        sphere_mesh.draw();
    }

    {
        Mat4 model = Mat4::identity();
        setMat4(lit_u_model, model);
        paddle_mesh.draw();
    }

    {
        int count = 12;
        float start = -1.10f;
        float step = 0.20f;

        for (int k = 0; k < count; ++k)
        {
            float ang = start + step * static_cast<float>(k);
            Mat4 model = rotationZ(ang);
            setMat4(lit_u_model, model);
            brick_mesh.draw();
        }
    }

    glUseProgram(axis_program);

    {
        Mat4 model = Mat4::identity();
        model(2, 3) = 0.001f;
        setMat4(axis_u_model, model);
        setMat4(axis_u_view, view);
        setMat4(axis_u_proj, proj);

        glBindVertexArray(axes_vao);
        glDrawArrays(GL_LINES, 0, 6);
        glBindVertexArray(0);
    }

    if (g_showPaused && g_pausedTex.valid())
    {
        glDisable(GL_DEPTH_TEST);
        glUseProgram(g_screen_prog);
        glUniform1i(g_screen_uTex, 0);
        g_pausedTex.bind(GL_TEXTURE0);

        glBindVertexArray(g_screenVao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        glEnable(GL_DEPTH_TEST);
    }
}

void Application::render_ui() {}

void Application::on_resize(int w, int h)
{
    IApplication::on_resize(w, h);
    glViewport(0, 0, w, h);
    camera.setViewportSize(w, h);
}

void Application::on_mouse_move(double x, double y) { (void)x; (void)y; }
void Application::on_mouse_button(int button, int action, int mods) { (void)button; (void)action; (void)mods; }

void Application::on_key_pressed(int key, int scancode, int action, int mods)
{
    (void)scancode;
    (void)mods;

    if (action != GLFW_PRESS)
        return;

    switch (key)
    {
    case GLFW_KEY_1:
        camera.setMode(Camera::Mode::Perspective);
        break;
    case GLFW_KEY_2:
        camera.setMode(Camera::Mode::OrthoTop);
        break;
    case GLFW_KEY_P:
        g_showPaused = !g_showPaused;
        break;
    case GLFW_KEY_R:
        camera.getFrame().position = Vec4(0.0f, 0.0f, 5.0f, 1.0f);
        camera.getFrame().orientation = Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
        break;
    default:
        break;
    }
}
