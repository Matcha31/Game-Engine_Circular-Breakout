#pragma once
#include "iapplication.h"
#include "glad/glad.h"

#include "graphics/camera.hpp"
#include "graphics/mesh.hpp"

class Application : public IApplication {
    private:
        Camera camera;

        Mesh paddle_mesh;
        Mesh brick_mesh_a;
        Mesh brick_mesh_b;
        Mesh ground_mesh;
        Mesh sphere_mesh;

        GLuint axis_vertex_shader;
        GLuint axis_fragment_shader;
        GLuint axis_program;

        GLuint lit_vertex_shader;
        GLuint lit_fragment_shader;
        GLuint lit_program;

        GLuint axes_vao;
        GLuint axes_vbo;

        GLint axis_u_model;
        GLint axis_u_view;
        GLint axis_u_proj;

        GLint lit_u_model;
        GLint lit_u_view;
        GLint lit_u_proj;
        GLint lit_u_light_dir;
        GLint lit_u_camera_pos;
        GLint lit_u_ambient;
        GLint lit_u_specular;
        GLint lit_u_shininess;

    public:
        Application(int initial_width, int initial_height, std::vector<std::string> arguments = {});

        virtual ~Application();

        void update(float delta) override;

        void render() override;

        void render_ui() override;

        void on_resize(int width, int height) override;

        void on_mouse_move(double x, double y) override;

        void on_mouse_button(int button, int action, int mods) override;

        void on_key_pressed(int key, int scancode, int action, int mods) override;
};
