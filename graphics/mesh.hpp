#pragma once

#include <vector>
#include "glad/glad.h"

class Mesh
{
public:
    struct Vertex
    {
        float x, y, z;
        float nx, ny, nz;
        float r, g, b;
    };

private:
    GLuint vao;
    GLuint vbo;
    GLuint ibo;
    GLsizei indexCount;

public:
    Mesh();
    Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices);

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;

    ~Mesh();

    void upload(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices);
    void destroy();

    void draw() const;

    bool valid() const { return vao != 0; }
    GLsizei getIndexCount() const { return indexCount; }
};
