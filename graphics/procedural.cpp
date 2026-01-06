#include "procedural.hpp"
#include <cmath>
#include <cassert>

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
