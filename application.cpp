#include "application.hpp"

#include "glad/glad.h"

#include "graphics/camera.hpp"
#include "graphics/mesh.hpp"
#include "graphics/procedural.hpp"
#include "graphics/texture2d.hpp"
#include "graphics/shader.hpp"

#include "math/axis_angle.hpp"
#include "math/quaternion.hpp"
#include "math/mat4.hpp"
#include "math/vec4.hpp"

#include "game/game_state.hpp"
#include "game/input.hpp"
#include "game/bricks.hpp"
#include "game/physics.hpp"

#include <cassert>
#include <cmath>
#include <filesystem>
#include <string>
#include <vector>

namespace
{
    struct SceneScale
    {
        float ground_r = 2.5f;
        int ground_segments = 160;
        float ground_uv_tiling = 6.0f;

        float paddle_r_inner = 2.00f;
        float paddle_r_outer = 2.20f;
        float paddle_half_span = 0.22f;

        float brick_r_inner = 0.60f;
        float brick_r_outer = 0.80f;
        float brick_half_span = 0.10f;

        int bricks_per_ring = 16;
        float brick_z0 = 0.02f;
        float brick_height = 0.10f;
        float brick_row_gap_z = 0.00f;

        int brick_rows = 4;
        float brick_row_gap = 0.03f;

        float ball_radius = 0.04f;

        float axis_len = 3.0f;
        float axis_z = 0.002f;

        float outer_limit_r = 2.45f;
    };

    static SceneScale S;

    static float ball_spawn_r()
    {
        float inner = S.brick_r_outer + S.ball_radius + 0.08f;
        float outer = S.paddle_r_inner - S.ball_radius - 0.08f;
        if (outer <= inner) return S.brick_r_outer + S.ball_radius + 0.05f;
        return 0.5f * (inner + outer);
    }
}

namespace
{
    struct AxisVertex
    {
        float x, y, z;
        float r, g, b;
    };

    float wrapAngle0To2Pi(float a)
    {
        const float twoPi = (float)(2.0 * M_PI);
        a = std::fmod(a, twoPi);
        if (a < 0.0f) a += twoPi;
        return a;
    }

    Vec4 normalize3(Vec4 v)
    {
        float len = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
        if (len == 0.0f) return Vec4(0.0f, 0.0f, 1.0f, 0.0f);
        return Vec4(v.x / len, v.y / len, v.z / len, 0.0f);
    }

    Mat4 rotationZ(float angle)
    {
        AxisAngle aa(Vec4(0.0f, 0.0f, 1.0f, 0.0f), angle);
        return aa.toQuaternion().toRotationMatrix();
    }

    void setMat4(GLint loc, const Mat4& m)
    {
        glUniformMatrix4fv(loc, 1, GL_FALSE, m.m);
    }

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

    void buildScreenQuad(GLuint& vao, GLuint& vbo)
    {
        struct V { float x, y, u, v; };
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

    void buildGroundDiscTextured(Mesh& mesh, float radius, int segments, float z, float cr, float cg, float cb, float uvTiling)
    {
        std::vector<Mesh::Vertex> v;
        std::vector<GLuint> idx;

        v.reserve((size_t)segments + 1);
        idx.reserve((size_t)segments * 3);

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
            float a = (float)(2.0 * M_PI) * (float)i / (float)segments;
            float x = radius * std::cos(a);
            float y = radius * std::sin(a);
            auto uv = uvFromXY(x, y);
            v.push_back({x, y, z, 0.f, 0.f, 1.f, cr, cg, cb, uv.first, uv.second});
        }

        for (int i = 0; i < segments; ++i)
        {
            GLuint c = 0;
            GLuint i0 = (GLuint)(i + 1);
            GLuint i1 = (GLuint)((i + 1) % segments + 1);
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

        v.reserve((size_t)(stacks + 1) * (size_t)(slices + 1));
        idx.reserve((size_t)stacks * (size_t)slices * 6);

        for (int i = 0; i <= stacks; ++i)
        {
            float t = (float)i / (float)stacks;
            float theta = (float)M_PI * t;
            float st = std::sin(theta);
            float ct = std::cos(theta);

            for (int j = 0; j <= slices; ++j)
            {
                float s = (float)j / (float)slices;
                float phi = (float)(2.0 * M_PI) * s;
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

    void buildPaddleAndBrickMeshes(Mesh& paddle, Mesh& brick_a, Mesh& brick_b)
    {
        {
            std::vector<Mesh::Vertex> v;
            std::vector<GLuint> idx;

            procedural::makeRingSegmentSolid(
                    S.paddle_r_inner,
                    S.paddle_r_outer,
                    -S.paddle_half_span,
                    +S.paddle_half_span,
                    64,
                    0.02f,
                    0.10f,
                    0.90f, 0.60f, 0.20f,
                    v, idx
                    );

            paddle.upload(v, idx);
        }

        auto buildBrick = [&](Mesh& out, float r, float g, float b)
        {
            std::vector<Mesh::Vertex> v;
            std::vector<GLuint> idx;

            const int cols = S.bricks_per_ring;
            const float a_half = (float)(M_PI / (double)cols);

            float eps = 0.0005f;
            float a0 = -a_half - eps;
            float a1 = +a_half + eps;

            float z0 = S.brick_z0;
            float z1 = S.brick_z0 + S.brick_height;

            procedural::makeRingSegmentSolid(
                    S.brick_r_inner,
                    S.brick_r_outer,
                    a0,
                    a1,
                    32,
                    z0,
                    z1,
                    r, g, b,
                    v, idx
                    );

            out.upload(v, idx);
        };

        buildBrick(brick_a, 0.75f, 0.35f, 0.30f);
        buildBrick(brick_b, 0.25f, 0.65f, 0.35f);
    }
}

static GameState g_state;
static InputState g_input;
static BrickField g_bricks;

static Texture2D g_groundTex;
static Texture2D g_pausedTex;
static Texture2D g_winTex;
static Texture2D g_gameOverTex;

static physics::Config g_phys;

static GLuint g_texlit_vs = 0, g_texlit_fs = 0, g_texlit_prog = 0;
static GLuint g_screen_vs = 0, g_screen_fs = 0, g_screen_prog = 0;

static GLint g_texlit_uModel = -1, g_texlit_uView = -1, g_texlit_uProj = -1;
static GLint g_texlit_uLightDir = -1, g_texlit_uCameraPos = -1;
static GLint g_texlit_uAmbient = -1, g_texlit_uSpecular = -1, g_texlit_uShininess = -1;
static GLint g_texlit_uTex = -1;

static GLint g_screen_uTex = -1;

static GLuint g_screenVao = 0, g_screenVbo = 0;

Application::Application(int initial_width, int initial_height, std::vector<std::string> arguments)
    : IApplication(initial_width, initial_height, arguments)
    , camera()
    , paddle_mesh()
    , brick_mesh_a()
    , brick_mesh_b()
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

    axis_vertex_shader = gfx::compile_shader(lecture_folder_path / "data" / "shaders" / "axis.vert", GL_VERTEX_SHADER);
    axis_fragment_shader = gfx::compile_shader(lecture_folder_path / "data" / "shaders" / "axis.frag", GL_FRAGMENT_SHADER);
    axis_program = gfx::link_program(axis_vertex_shader, axis_fragment_shader);

    lit_vertex_shader = gfx::compile_shader(lecture_folder_path / "data" / "shaders" / "lit.vert", GL_VERTEX_SHADER);
    lit_fragment_shader = gfx::compile_shader(lecture_folder_path / "data" / "shaders" / "lit.frag", GL_FRAGMENT_SHADER);
    lit_program = gfx::link_program(lit_vertex_shader, lit_fragment_shader);

    g_texlit_vs = gfx::compile_shader(lecture_folder_path / "data" / "shaders" / "texlit.vert", GL_VERTEX_SHADER);
    g_texlit_fs = gfx::compile_shader(lecture_folder_path / "data" / "shaders" / "texlit.frag", GL_FRAGMENT_SHADER);
    g_texlit_prog = gfx::link_program(g_texlit_vs, g_texlit_fs);

    g_screen_vs = gfx::compile_shader(lecture_folder_path / "data" / "shaders" / "screen.vert", GL_VERTEX_SHADER);
    g_screen_fs = gfx::compile_shader(lecture_folder_path / "data" / "shaders" / "screen.frag", GL_FRAGMENT_SHADER);
    g_screen_prog = gfx::link_program(g_screen_vs, g_screen_fs);

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

    // buildAxes(axes_vao, axes_vbo, 3.0f);
    buildGroundDiscTextured(
            ground_mesh,
            S.ground_r,
            S.ground_segments,
            0.0f,
            1.0f, 1.0f, 1.0f,
            S.ground_uv_tiling
            );
    buildSphere(sphere_mesh, S.ball_radius, 24, 48, 0.95f, 0.95f, 0.95f);
    buildPaddleAndBrickMeshes(paddle_mesh, brick_mesh_a, brick_mesh_b);

    physics::sync_config_from_scene(
            g_phys,
            S.outer_limit_r,
            S.ball_radius,
            3,
            S.paddle_r_inner,
            S.paddle_r_outer,
            S.paddle_half_span,
            S.bricks_per_ring,
            S.brick_rows,
            S.brick_r_inner,
            S.brick_r_outer
            );

    buildScreenQuad(g_screenVao, g_screenVbo);

    g_groundTex.loadPNG(lecture_folder_path / "data" / "textures" / "ground.png");
    g_pausedTex.loadPNG(lecture_folder_path / "data" / "textures" / "pause.png", false);
    g_winTex.loadPNG(lecture_folder_path / "data" / "textures" / "you_win.png", false);
    g_gameOverTex.loadPNG(lecture_folder_path / "data" / "textures" / "game_over.png", false);

    glClearColor(0.05f, 0.05f, 0.08f, 1.0f);

    g_state.reset();
    g_state.outer_limit_r = S.outer_limit_r;
    g_state.ball_r = ball_spawn_r();
    physics::place_ball_waiting(g_state, g_state.ball_r);
    g_state.ball_a = g_state.paddle_angle;

    const int cols = S.bricks_per_ring;
    const float a_half = (float)(M_PI / (double)cols);
    g_bricks.build_wall(cols, S.brick_rows, 0.0f, a_half);
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
    // if (axis_program) glDeleteProgram(axis_program);

    if (g_screen_fs) glDeleteShader(g_screen_fs);
    if (g_screen_vs) glDeleteShader(g_screen_vs);

    if (g_texlit_fs) glDeleteShader(g_texlit_fs);
    if (g_texlit_vs) glDeleteShader(g_texlit_vs);

    if (lit_fragment_shader) glDeleteShader(lit_fragment_shader);
    if (lit_vertex_shader) glDeleteShader(lit_vertex_shader);

    // if (axis_fragment_shader) glDeleteShader(axis_fragment_shader);
    // if (axis_vertex_shader) glDeleteShader(axis_vertex_shader);
}

void Application::update(float delta)
{
    g_state.left_down = g_input.left_down;
    g_state.right_down = g_input.right_down;

    g_state.update(delta);
    physics::step(g_state, g_bricks, g_phys, delta);

    g_input.begin_frame();
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

    glUniform1f(g_texlit_uAmbient, 0.50f);
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
        float x = g_state.ball_r * std::cos(g_state.ball_a);
        float y = g_state.ball_r * std::sin(g_state.ball_a);

        Mat4 model = Mat4::identity();
        model(0, 3) = x;
        model(1, 3) = y;
        model(2, 3) = S.ball_radius;

        setMat4(lit_u_model, model);
        sphere_mesh.draw();
    }

    {
        for (int i = 0; i < 3; ++i)
        {
            float offset = (float)(2.0 * M_PI / 3.0) * (float)i;
            Mat4 model = rotationZ(g_state.paddle_angle + offset);
            setMat4(lit_u_model, model);
            paddle_mesh.draw();
        }
    }

    {
        for (const Brick& b : g_bricks.bricks)
        {
            if (!b.alive) continue;

            Mat4 model = rotationZ(b.a_center);

            float z = S.brick_z0 + (float)b.row * (S.brick_height + S.brick_row_gap_z);
            model(2, 3) = z;

            setMat4(lit_u_model, model);

            bool alt = ((b.col + b.row) & 1) != 0;
            if (alt) brick_mesh_b.draw();
            else     brick_mesh_a.draw();
        }
    }

    // glUseProgram(axis_program);
    //
    // {
    //     Mat4 model = Mat4::identity();
    //     model(2, 3) = 0.001f;
    //
    //     setMat4(axis_u_model, model);
    //     setMat4(axis_u_view, view);
    //     setMat4(axis_u_proj, proj);
    //
    //     glBindVertexArray(axes_vao);
    //     glDrawArrays(GL_LINES, 0, 6);
    //     glBindVertexArray(0);
    // }

    if (g_state.mode == GameMode::Paused && g_pausedTex.valid())
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

    if (g_state.mode == GameMode::GameOver && g_gameOverTex.valid())
    {
        glDisable(GL_DEPTH_TEST);

        glUseProgram(g_screen_prog);
        glUniform1i(g_screen_uTex, 0);
        g_gameOverTex.bind(GL_TEXTURE0);

        glBindVertexArray(g_screenVao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        glEnable(GL_DEPTH_TEST);
    }

    if (g_state.mode == GameMode::Win && g_winTex.valid())
    {
        glDisable(GL_DEPTH_TEST);

        glUseProgram(g_screen_prog);
        glUniform1i(g_screen_uTex, 0);
        g_winTex.bind(GL_TEXTURE0);

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
    camera.getFrame().position = Vec4(0.0f, -3.0f, 2.6f, 1.0f);
    camera.getFrame().orientation = Quaternion(0.35f, 0.0f, 0.0f, 0.94f).normalized();
}

void Application::on_mouse_move(double x, double y) { (void)x; (void)y; }
void Application::on_mouse_button(int button, int action, int mods) { (void)button; (void)action; (void)mods; }

void Application::on_key_pressed(int key, int scancode, int action, int mods)
{
    (void)scancode;
    (void)mods;

    g_input.handle_key(key, action);

    if (action == GLFW_PRESS)
    {
        if (key == GLFW_KEY_1) camera.setMode(Camera::Mode::Perspective);
        if (key == GLFW_KEY_2) camera.setMode(Camera::Mode::OrthoTop);
    }

    if (g_input.pause_pressed) g_state.toggle_pause();
    if (g_input.launch_pressed) physics::launch_ball(g_state, g_phys);
    if (g_input.reset_pressed)
    {
        g_state.reset();
        g_state.outer_limit_r = S.outer_limit_r;
        g_state.ball_r = ball_spawn_r();
        physics::place_ball_waiting(g_state, g_state.ball_r);
        g_state.ball_a = g_state.paddle_angle;

        const int cols = S.bricks_per_ring;
        const float a_half = (float)(M_PI / (double)cols);
        g_bricks.build_wall(cols, S.brick_rows, 0.0f, a_half);
    }
}
