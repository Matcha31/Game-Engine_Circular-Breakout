#include "shader.hpp"

#include <cassert>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>

namespace gfx
{
    GLuint compile_shader(const std::filesystem::path& path, GLenum type)
    {
        GLuint shader = glCreateShader(type);
        assert(glGetError() == 0U && shader != 0);

        std::ifstream ifs(path);
        if (!ifs.is_open())
        {
            std::cerr << "Failed to open shader file: " << path << "\n";
            assert(false);
        }

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
            std::string log((size_t)len, '\0');
            glGetShaderInfoLog(shader, len, nullptr, log.data());

            std::cerr << "Shader compile error (" << path << "):\n" << log << "\n";
            assert(false);
        }

        return shader;
    }

    GLuint link_program(GLuint vertex_shader, GLuint fragment_shader)
    {
        GLuint program = glCreateProgram();
        assert(glGetError() == 0U && program != 0);

        glAttachShader(program, vertex_shader);
        glAttachShader(program, fragment_shader);
        glLinkProgram(program);

        GLint ok = 0;
        glGetProgramiv(program, GL_LINK_STATUS, &ok);
        if (!ok)
        {
            GLint len = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
            std::string log((size_t)len, '\0');
            glGetProgramInfoLog(program, len, nullptr, log.data());

            std::cerr << "Program link error:\n" << log << "\n";
            assert(false);
        }

        glDetachShader(program, vertex_shader);
        glDetachShader(program, fragment_shader);

        return program;
    }
}
