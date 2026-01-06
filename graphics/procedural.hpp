#pragma once

#include <vector>
#include "mesh.hpp"

namespace procedural
{
    void makeRingSegmentFlat(
        float innerRadius,
        float outerRadius,
        float angleStart,
        float angleEnd,
        int segments,
        float z,
        float cr, float cg, float cb,
        std::vector<Mesh::Vertex>& outVertices,
        std::vector<GLuint>& outIndices
    );
}
