#pragma once
#include "iapplication.h"
#include "glad/glad.h"

#include "engine/camera.hpp"

class Application : public IApplication {
  private:
    Camera camera;

    GLuint vertex_shader;
    GLuint fragment_shader;
    GLuint shader_program;

    GLuint axes_vao;
    GLuint axes_vbo;

    GLuint ground_vao;
    GLuint ground_vbo;
    GLuint ground_ibo;
    GLsizei ground_index_count;

    GLint u_model_loc;
    GLint u_view_loc;
    GLint u_proj_loc;

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
