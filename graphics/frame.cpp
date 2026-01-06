#include "frame.hpp"

Frame::Frame()
    : parent(nullptr),
      position(0.0f, 0.0f, 0.0f, 1.0f),
      orientation(0.0f, 0.0f, 0.0f, 1.0f)
{
}

Frame::Frame(Frame* parentFrame)
    : parent(parentFrame),
      position(0.0f, 0.0f, 0.0f, 1.0f),
      orientation(0.0f, 0.0f, 0.0f, 1.0f)
{
}

void Frame::setParent(Frame* parentFrame)
{
    parent = parentFrame;
}

Mat4 Frame::toParentMatrix() const
{
    Quaternion q = orientation.normalized();
    Mat4 m = q.toRotationMatrix();

    m(0,3) = position.x;
    m(1,3) = position.y;
    m(2,3) = position.z;

    return m;
}

Mat4 Frame::fromParentMatrix() const
{
    Quaternion qInv = orientation.inverse();
    Mat4 m = qInv.toRotationMatrix();

    Vec4 t(position.x, position.y, position.z, 0.0f);
    Vec4 tLocal = m * t;
    Vec4 neg = -tLocal;

    m(0,3) = neg.x;
    m(1,3) = neg.y;
    m(2,3) = neg.z;

    return m;
}

Mat4 Frame::toWorldMatrix() const
{
    Mat4 localToParent = toParentMatrix();
    if (!parent)
        return localToParent;

    Mat4 parentToWorld = parent->toWorldMatrix();
    return parentToWorld * localToParent;
}

Mat4 Frame::fromWorldMatrix() const
{
    Mat4 parentFromWorld;
    if (!parent)
    {
        parentFromWorld = Mat4::identity();
    }
    else
    {
        parentFromWorld = parent->fromWorldMatrix();
    }

    Mat4 fromParent = fromParentMatrix();
    return fromParent * parentFromWorld;
}
