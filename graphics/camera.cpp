#include "camera.hpp"
#include <cmath>

namespace
{
    constexpr float PI = 3.14159265358979323846f;

    Mat4 makePerspective(float fovY, float aspect, float zNear, float zFar)
    {
        float f = 1.0f / std::tan(fovY * 0.5f);

        float A = (zFar + zNear) / (zNear - zFar);
        float B = (2.0f * zFar * zNear) / (zNear - zFar);

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

        return Mat4({
            a,    0.0f, 0.0f, 0.0f,
            0.0f, b,    0.0f, 0.0f,
            0.0f, 0.0f, c,    0.0f,
            tx,   ty,   tz,   1.0f
        });
    }

    float maxf(float a, float b)
    {
        return (a > b) ? a : b;
    }

    float minf(float a, float b)
    {
        return (a < b) ? a : b;
    }
}

Camera::Camera()
    : frame(nullptr)
    , mode(Mode::Perspective)
    , fovY(45.0f * PI / 180.0f)
    , orthoHalfHeight(2.8f)
    , nearPlane(0.1f)
    , farPlane(100.0f)
    , viewportWidth(800)
    , viewportHeight(600)
    , sceneRadius(2.8f)
{
    updateFrameForMode();
}

float Camera::aspectRatio() const
{
    return static_cast<float>(viewportWidth) /
           static_cast<float>(viewportHeight);
}

float Camera::fittedOrthoHalfHeight() const
{
    const float margin = 1.10f;
    const float aspect = aspectRatio();

    // Orthographic projection uses halfW = halfH * aspect.
    // In a narrow window, halfH must grow so halfW still covers the arena.
    if (aspect < 1.0f)
        return margin * sceneRadius / aspect;

    return margin * sceneRadius;
}

float Camera::fittedPerspectiveDistance() const
{
    const float margin = 1.15f;
    const float aspect = aspectRatio();

    const float verticalFov = fovY;
    const float horizontalFov = 2.0f * std::atan(std::tan(fovY * 0.5f) * aspect);
    const float limitingFov = minf(verticalFov, horizontalFov);

    // Fit a bounding sphere around the play area. This is conservative and
    // independent from the 45 degree pitch.
    return (margin * sceneRadius) / std::sin(limitingFov * 0.5f);
}

void Camera::updateFrameForMode()
{
    if (mode == Mode::OrthoTop)
    {
        // With your Frame implementation, identity orientation means that
        // camera local -Z points along world -Z. From z > 0, this is top-down.
        const float topDistance = 25.0f;

        frame.position = Vec4(0.0f, 0.0f, topDistance, 1.0f);
        frame.orientation = Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
        orthoHalfHeight = fittedOrthoHalfHeight();
        return;
    }

    // Perspective view: camera looks at the origin from the -Y/+Z side.
    // The +X rotation pitches camera local -Z toward +Y and -Z in world space.
    const float pitch = 45.0f * PI / 180.0f;
    const float distance = fittedPerspectiveDistance();

    frame.position = Vec4(
            0.0f,
            -distance * std::sin(pitch),
             distance * std::cos(pitch),
            1.0f);

    frame.orientation = Quaternion(
            std::sin(pitch * 0.5f),
            0.0f,
            0.0f,
            std::cos(pitch * 0.5f)).normalized();
}

void Camera::setMode(Mode m)
{
    if (m == mode)
        return;

    mode = m;
    updateFrameForMode();
}

Camera::Mode Camera::getMode() const
{
    return mode;
}

void Camera::setViewportSize(int width, int height)
{
    viewportWidth  = (width  > 0) ? width  : 1;
    viewportHeight = (height > 0) ? height : 1;

    updateFrameForMode();
}

void Camera::setSceneRadius(float radius)
{
    sceneRadius = maxf(radius, 0.1f);
    updateFrameForMode();
}

void Camera::setPerspective(float fovYRadians, float nearP, float farP)
{
    fovY = fovYRadians;
    nearPlane = nearP;
    farPlane = farP;

    updateFrameForMode();
}

void Camera::setOrtho(float halfHeight, float nearP, float farP)
{
    sceneRadius = maxf(halfHeight, 0.1f);
    orthoHalfHeight = fittedOrthoHalfHeight();
    nearPlane = nearP;
    farPlane = farP;

    updateFrameForMode();
}

Mat4 Camera::viewMatrix() const
{
    return frame.fromWorldMatrix();
}

Mat4 Camera::projectionMatrix() const
{
    const float aspect = aspectRatio();

    if (mode == Mode::Perspective)
    {
        return makePerspective(fovY, aspect, nearPlane, farPlane);
    }

    const float halfH = orthoHalfHeight;
    const float halfW = halfH * aspect;

    return makeOrtho(-halfW, halfW, -halfH, halfH, nearPlane, farPlane);
}
