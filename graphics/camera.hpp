#pragma once

#include "frame.hpp"
#include "../math/mat4.hpp"

class Camera
{
public:
    enum class Mode
    {
        Perspective,
        OrthoTop
    };

private:
    Frame frame;
    Mode mode;

    float fovY;
    float orthoHalfHeight;
    float nearPlane;
    float farPlane;

    int viewportWidth;
    int viewportHeight;

    float sceneRadius;

    float aspectRatio() const;
    float fittedOrthoHalfHeight() const;
    float fittedPerspectiveDistance() const;
    void updateFrameForMode();

public:
    Camera();

    Frame& getFrame()             { return frame; }
    const Frame& getFrame() const { return frame; }

    void setMode(Mode m);
    Mode getMode() const;

    void setViewportSize(int width, int height);
    void setSceneRadius(float radius);

    void setPerspective(float fovYRadians, float nearP, float farP);
    void setOrtho(float halfHeight, float nearP, float farP);

    Mat4 viewMatrix() const;
    Mat4 projectionMatrix() const;
};
