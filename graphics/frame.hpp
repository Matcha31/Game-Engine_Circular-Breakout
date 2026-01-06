#pragma once

#include "../math/vec4.hpp"
#include "../math/quaternion.hpp"
#include "../math/mat4.hpp"

class Frame
{
public:
    Frame* parent;
    Vec4 position; // x,y,z used, w ignored
    Quaternion orientation;

    Frame();
    explicit Frame(Frame* parentFrame);

    void setParent(Frame* parentFrame);

    Mat4 toParentMatrix() const;
    Mat4 fromParentMatrix() const;

    Mat4 toWorldMatrix() const;
    Mat4 fromWorldMatrix() const;
};
