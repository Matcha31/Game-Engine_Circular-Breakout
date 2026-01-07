#pragma once

#include <filesystem>
#include "glad/glad.h"

namespace gfx
{
    GLuint compile_shader(const std::filesystem::path& path, GLenum type);
    GLuint link_program(GLuint vertex_shader, GLuint fragment_shader);
}
