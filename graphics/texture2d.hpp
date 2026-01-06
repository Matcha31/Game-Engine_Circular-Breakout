#pragma once

#include "glad/glad.h"
#include <filesystem>

class Texture2D
{
    GLuint id;

public:
    Texture2D();
    ~Texture2D();

    Texture2D(const Texture2D&) = delete;
    Texture2D& operator=(const Texture2D&) = delete;

    Texture2D(Texture2D&& other) noexcept;
    Texture2D& operator=(Texture2D&& other) noexcept;

    bool loadPNG(const std::filesystem::path& path, bool generateMipmaps = true);
    void destroy();

    void bind(GLenum textureUnit = GL_TEXTURE0) const;

    GLuint getId() const { return id; }
    bool valid() const { return id != 0; }
};
