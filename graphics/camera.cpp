#include "camera.hpp"

namespace
{
    Mat4 makePerspective(float fovY, float aspect, float zNear, float zFar)
    {
        float f = 1.0f / std::tan(fovY * 0.5f);

        float A = (zFar + zNear) / (zNear - zFar);
        float B = (2.0f * zFar * zNear) / (zNear - zFar);

        // Column-major
        return Mat4({
            f / aspect, 0.0f, 0.0f, 0.0f,
            0.0f,       f,    0.0f, 0.0f,
            0.0f,       0.0f, A,   -1.0f,
            0.0f,       0.0f, B,    0.0f
        });
    }

    Mat4 makeOrtho(float left, float right, float bottom, float top,
                   float zNear, float zFar)
    {
        float invRL = 1.0f / (right - left);
        float invTB = 1.0f / (top - bottom);
        float invFN = 1.0f / (zFar - zNear);

        float a = 2.0f * invRL;
        float b = 2.0f * invTB;
        float c = -2.0f * invFN;

        float tx = -(right + left) * invRL;
        float ty = -(top + bottom) * invTB;
        float tz = -(zFar + zNear) * invFN;

        // Column-major
        return Mat4({
            a,    0.0f, 0.0f, 0.0f,
            0.0f, b,    0.0f, 0.0f,
            0.0f, 0.0f, c,    0.0f,
            tx,   ty,   tz,   1.0f
        });
    }
}

Camera::Camera()
    : frame(nullptr),
      mode(Mode::Perspective),
      fovY(45.0f * M_PI / 180.0f),
      orthoHalfHeight(10.0f),
      nearPlane(0.1f),
      farPlane(100.0f),
      viewportWidth(800),
      viewportHeight(600)
{
    // Default: put camera a bit above +Z looking toward origin.
    frame.position = Vec4(0.0f, 0.0f, 5.0f, 1.0f);
    frame.orientation = Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
}

void Camera::setMode(Mode m)
{
    mode = m;
}

Camera::Mode Camera::getMode() const
{
    return mode;
}

void Camera::setViewportSize(int width, int height)
{
    viewportWidth = (width  > 0) ? width  : 1;
    viewportHeight = (height > 0) ? height : 1;
}

void Camera::setPerspective(float fovYRadians, float nearP, float farP)
{
    fovY = fovYRadians;
    nearPlane = nearP;
    farPlane = farP;
}

void Camera::setOrtho(float halfHeight, float nearP, float farP)
{
    orthoHalfHeight = halfHeight;
    nearPlane = nearP;
    farPlane = farP;
}

Mat4 Camera::viewMatrix() const
{
    // Frame::fromWorldMatrix gives world → frame; for camera, that is view.
    return frame.fromWorldMatrix();
}

Mat4 Camera::projectionMatrix() const
{
    float aspect = static_cast<float>(viewportWidth) /
                   static_cast<float>(viewportHeight);

    if (mode == Mode::Perspective)
    {
        return makePerspective(fovY, aspect, nearPlane, farPlane);
    }
    else
    {
        float halfH = orthoHalfHeight;
        float halfW = halfH * aspect;

        float left   = -halfW;
        float right  =  halfW;
        float bottom = -halfH;
        float top    =  halfH;

        return makeOrtho(left, right, bottom, top, nearPlane, farPlane);
    }
}
