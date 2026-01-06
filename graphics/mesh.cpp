#include "mesh.hpp"
#include <cassert>

Mesh::Mesh()
    : vao(0), vbo(0), ibo(0), indexCount(0)
{
}

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices)
    : vao(0), vbo(0), ibo(0), indexCount(0)
{
    upload(vertices, indices);
}

Mesh::Mesh(Mesh&& other) noexcept
    : vao(other.vao), vbo(other.vbo), ibo(other.ibo), indexCount(other.indexCount)
{
    other.vao = 0;
    other.vbo = 0;
    other.ibo = 0;
    other.indexCount = 0;
}

Mesh& Mesh::operator=(Mesh&& other) noexcept
{
    if (this == &other)
        return *this;

    destroy();

    vao = other.vao;
    vbo = other.vbo;
    ibo = other.ibo;
    indexCount = other.indexCount;

    other.vao = 0;
    other.vbo = 0;
    other.ibo = 0;
    other.indexCount = 0;

    return *this;
}

Mesh::~Mesh()
{
    destroy();
}

void Mesh::destroy()
{
    if (ibo) glDeleteBuffers(1, &ibo);
    if (vbo) glDeleteBuffers(1, &vbo);
    if (vao) glDeleteVertexArrays(1, &vao);

    vao = 0;
    vbo = 0;
    ibo = 0;
    indexCount = 0;
}

void Mesh::upload(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices)
{
    destroy();

    indexCount = static_cast<GLsizei>(indices.size());

    glGenVertexArrays(1, &vao);
    assert(glGetError() == 0U && vao != 0);
    glBindVertexArray(vao);
    assert(glGetError() == 0U);

    glGenBuffers(1, &vbo);
    assert(glGetError() == 0U && vbo != 0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    assert(glGetError() == 0U);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
    assert(glGetError() == 0U);

    glEnableVertexAttribArray(0);
    assert(glGetError() == 0U);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    assert(glGetError() == 0U);

    glEnableVertexAttribArray(1);
    assert(glGetError() == 0U);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
    assert(glGetError() == 0U);

    glEnableVertexAttribArray(2);
    assert(glGetError() == 0U);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(6 * sizeof(float)));
    assert(glGetError() == 0U);

    glGenBuffers(1, &ibo);
    assert(glGetError() == 0U && ibo != 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    assert(glGetError() == 0U);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
    assert(glGetError() == 0U);

    glBindVertexArray(0);
}

void Mesh::draw() const
{
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}
