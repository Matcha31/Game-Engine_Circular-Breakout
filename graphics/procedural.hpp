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

    void makeRingSegmentSolid(
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
    );
}
