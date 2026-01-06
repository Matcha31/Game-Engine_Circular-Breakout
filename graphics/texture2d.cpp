#include "texture2d.hpp"
#include "lodepng.h"
#include <cassert>
#include <vector>

Texture2D::Texture2D() : id(0) {}

Texture2D::~Texture2D()
{
    destroy();
}

Texture2D::Texture2D(Texture2D&& other) noexcept : id(other.id)
{
    other.id = 0;
}

Texture2D& Texture2D::operator=(Texture2D&& other) noexcept
{
    if (this == &other) return *this;
    destroy();
    id = other.id;
    other.id = 0;
    return *this;
}

void Texture2D::destroy()
{
    if (id)
        glDeleteTextures(1, &id);
    id = 0;
}

bool Texture2D::loadPNG(const std::filesystem::path& path, bool generateMipmaps)
{
    destroy();

    std::vector<unsigned char> texels;
    unsigned int width = 0, height = 0;
    unsigned int error_code = lodepng::decode(texels, width, height, path.string(), LCT_RGBA);
    if (error_code != 0)
        return false;

    for (unsigned int lo = 0, hi = height - 1; lo < hi; ++lo, --hi)
    {
        for (unsigned int* lo_ptr = (unsigned int*)texels.data() + lo * width,
                          * lo_end = lo_ptr + width,
                          * hi_ptr = (unsigned int*)texels.data() + hi * width;
             lo_ptr != lo_end; ++lo_ptr, ++hi_ptr)
        {
            std::swap(*lo_ptr, *hi_ptr);
        }
    }

    glGenTextures(1, &id);
    assert(glGetError() == 0U && id != 0);

    glBindTexture(GL_TEXTURE_2D, id);
    assert(glGetError() == 0U);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)width, (GLsizei)height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texels.data());
    assert(glGetError() == 0U);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, generateMipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    if (generateMipmaps)
    {
        glGenerateMipmap(GL_TEXTURE_2D);
        assert(glGetError() == 0U);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    return true;
}

void Texture2D::bind(GLenum textureUnit) const
{
    glActiveTexture(textureUnit);
    glBindTexture(GL_TEXTURE_2D, id);
}
