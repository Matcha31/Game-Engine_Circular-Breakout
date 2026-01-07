#include "procedural.hpp"
#include <cmath>
#include <cassert>

namespace
{
    void pushVertex(std::vector<Mesh::Vertex>& v,
            float x, float y, float z,
            float nx, float ny, float nz,
            float r, float g, float b)
    {
        Mesh::Vertex vert;
        vert.x = x; vert.y = y; vert.z = z;
        vert.nx = nx; vert.ny = ny; vert.nz = nz;
        vert.r = r; vert.g = g; vert.b = b;
        vert.u = 0.0f; vert.v = 0.0f;
        v.push_back(vert);
    }

    void addTri(std::vector<GLuint>& idx, GLuint a, GLuint b, GLuint c)
    {
        idx.push_back(a);
        idx.push_back(b);
        idx.push_back(c);
    }

    void addQuad(std::vector<GLuint>& idx, GLuint a, GLuint b, GLuint c, GLuint d)
    {
        addTri(idx, a, b, c);
        addTri(idx, a, c, d);
    }

    void normalize2(float& x, float& y)
    {
        float l = std::sqrt(x * x + y * y);
        if (l > 0.0f) { x /= l; y /= l; }
    }
}

void procedural::makeRingSegmentFlat(
        float innerRadius,
        float outerRadius,
        float angleStart,
        float angleEnd,
        int segments,
        float z,
        float cr, float cg, float cb,
        std::vector<Mesh::Vertex>& outVertices,
        std::vector<GLuint>& outIndices
        )
{
    assert(segments >= 1);
    assert(innerRadius >= 0.0f);
    assert(outerRadius > innerRadius);

    outVertices.clear();
    outIndices.clear();

    outVertices.reserve(static_cast<size_t>((segments + 1) * 2));
    outIndices.reserve(static_cast<size_t>(segments * 6));

    for (int i = 0; i <= segments; ++i)
    {
        float t = static_cast<float>(i) / static_cast<float>(segments);
        float a = angleStart + t * (angleEnd - angleStart);

        float ca = std::cos(a);
        float sa = std::sin(a);

        float xi = innerRadius * ca;
        float yi = innerRadius * sa;

        float xo = outerRadius * ca;
        float yo = outerRadius * sa;

        outVertices.push_back({ xi, yi, z, 0.f, 0.f, 1.f, cr, cg, cb });
        outVertices.push_back({ xo, yo, z, 0.f, 0.f, 1.f, cr, cg, cb });
    }

    for (int i = 0; i < segments; ++i)
    {
        GLuint i0 = static_cast<GLuint>(2 * i + 0);
        GLuint i1 = static_cast<GLuint>(2 * i + 1);
        GLuint i2 = static_cast<GLuint>(2 * (i + 1) + 0);
        GLuint i3 = static_cast<GLuint>(2 * (i + 1) + 1);

        outIndices.push_back(i0);
        outIndices.push_back(i1);
        outIndices.push_back(i2);

        outIndices.push_back(i2);
        outIndices.push_back(i1);
        outIndices.push_back(i3);
    }
}

void procedural::makeRingSegmentSolid(
        float r_inner,
        float r_outer,
        float a0,
        float a1,
        int segments,
        float z0,
        float z1,
        float cr, float cg, float cb,
        std::vector<Mesh::Vertex>& out_vertices,
        std::vector<GLuint>& out_indices
        )
{
    out_vertices.clear();
    out_indices.clear();

    if (segments < 1) segments = 1;

    float da = (a1 - a0) / (float)segments;

    auto angle = [&](int i) { return a0 + da * (float)i; };

    GLuint base_top = 0;
    for (int i = 0; i <= segments; ++i)
    {
        float a = angle(i);
        float ca = std::cos(a);
        float sa = std::sin(a);

        float xi = r_inner * ca;
        float yi = r_inner * sa;
        float xo = r_outer * ca;
        float yo = r_outer * sa;

        pushVertex(out_vertices, xi, yi, z1, 0.f, 0.f, 1.f, cr, cg, cb);
        pushVertex(out_vertices, xo, yo, z1, 0.f, 0.f, 1.f, cr, cg, cb);
    }

    GLuint top_count = (GLuint)(2 * (segments + 1));

    for (int i = 0; i < segments; ++i)
    {
        GLuint i0 = base_top + (GLuint)(2 * i);
        GLuint o0 = i0 + 1;
        GLuint i1 = base_top + (GLuint)(2 * (i + 1));
        GLuint o1 = i1 + 1;
        addQuad(out_indices, i0, o0, o1, i1);
    }

    GLuint base_bottom = (GLuint)out_vertices.size();
    for (int i = 0; i <= segments; ++i)
    {
        float a = angle(i);
        float ca = std::cos(a);
        float sa = std::sin(a);

        float xi = r_inner * ca;
        float yi = r_inner * sa;
        float xo = r_outer * ca;
        float yo = r_outer * sa;

        pushVertex(out_vertices, xi, yi, z0, 0.f, 0.f, -1.f, cr, cg, cb);
        pushVertex(out_vertices, xo, yo, z0, 0.f, 0.f, -1.f, cr, cg, cb);
    }

    for (int i = 0; i < segments; ++i)
    {
        GLuint i0 = base_bottom + (GLuint)(2 * i);
        GLuint o0 = i0 + 1;
        GLuint i1 = base_bottom + (GLuint)(2 * (i + 1));
        GLuint o1 = i1 + 1;
        addQuad(out_indices, i0, i1, o1, o0);
    }

    GLuint base_outer = (GLuint)out_vertices.size();
    for (int i = 0; i <= segments; ++i)
    {
        float a = angle(i);
        float ca = std::cos(a);
        float sa = std::sin(a);

        float x = r_outer * ca;
        float y = r_outer * sa;

        float nx = ca;
        float ny = sa;
        normalize2(nx, ny);

        pushVertex(out_vertices, x, y, z0, nx, ny, 0.f, cr, cg, cb);
        pushVertex(out_vertices, x, y, z1, nx, ny, 0.f, cr, cg, cb);
    }

    for (int i = 0; i < segments; ++i)
    {
        GLuint b0 = base_outer + (GLuint)(2 * i);
        GLuint t0 = b0 + 1;
        GLuint b1 = base_outer + (GLuint)(2 * (i + 1));
        GLuint t1 = b1 + 1;
        addQuad(out_indices, b0, b1, t1, t0);
    }

    GLuint base_inner = (GLuint)out_vertices.size();
    for (int i = 0; i <= segments; ++i)
    {
        float a = angle(i);
        float ca = std::cos(a);
        float sa = std::sin(a);

        float x = r_inner * ca;
        float y = r_inner * sa;

        float nx = -ca;
        float ny = -sa;
        normalize2(nx, ny);

        pushVertex(out_vertices, x, y, z0, nx, ny, 0.f, cr, cg, cb);
        pushVertex(out_vertices, x, y, z1, nx, ny, 0.f, cr, cg, cb);
    }

    for (int i = 0; i < segments; ++i)
    {
        GLuint b0 = base_inner + (GLuint)(2 * i);
        GLuint t0 = b0 + 1;
        GLuint b1 = base_inner + (GLuint)(2 * (i + 1));
        GLuint t1 = b1 + 1;
        addQuad(out_indices, b0, t0, t1, b1);
    }

    float a_start = a0;
    float ca0 = std::cos(a_start);
    float sa0 = std::sin(a_start);
    {
        GLuint base_side0 = (GLuint)out_vertices.size();

        float tx = -sa0;
        float ty =  ca0;

        pushVertex(out_vertices, r_inner * ca0, r_inner * sa0, z0, -tx, -ty, 0.f, cr, cg, cb);
        pushVertex(out_vertices, r_outer * ca0, r_outer * sa0, z0, -tx, -ty, 0.f, cr, cg, cb);
        pushVertex(out_vertices, r_outer * ca0, r_outer * sa0, z1, -tx, -ty, 0.f, cr, cg, cb);
        pushVertex(out_vertices, r_inner * ca0, r_inner * sa0, z1, -tx, -ty, 0.f, cr, cg, cb);

        addQuad(out_indices, base_side0, base_side0 + 1, base_side0 + 2, base_side0 + 3);
    }

    float a_end = a1;
    float ca1 = std::cos(a_end);
    float sa1 = std::sin(a_end);
    {
        GLuint base_side1 = (GLuint)out_vertices.size();

        float tx = -sa1;
        float ty =  ca1;

        pushVertex(out_vertices, r_inner * ca1, r_inner * sa1, z0, tx, ty, 0.f, cr, cg, cb);
        pushVertex(out_vertices, r_outer * ca1, r_outer * sa1, z0, tx, ty, 0.f, cr, cg, cb);
        pushVertex(out_vertices, r_outer * ca1, r_outer * sa1, z1, tx, ty, 0.f, cr, cg, cb);
        pushVertex(out_vertices, r_inner * ca1, r_inner * sa1, z1, tx, ty, 0.f, cr, cg, cb);

        addQuad(out_indices, base_side1, base_side1 + 3, base_side1 + 2, base_side1 + 1);
    }
}
