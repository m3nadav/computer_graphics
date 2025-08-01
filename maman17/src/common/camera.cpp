#include "common/camera.h"
#include <GLUT/glut.h>
#include <cmath>

CameraController::CameraController()
    : distance(5.0f), angleX(35.0f), angleY(55.0f), minDistance(1.0f), maxDistance(100.0f)
{
}

CameraController::CameraController(float distance, float angleX, float angleY)
    : distance(distance), angleX(angleX), angleY(angleY), minDistance(1.0f), maxDistance(100.0f)
{
    clampValues();
}

void CameraController::zoomIn(float step)
{
    distance -= step;
    clampValues();
}

void CameraController::zoomOut(float step)
{
    distance += step;
    clampValues();
}

void CameraController::rotate(float deltaX, float deltaY, float sensitivity)
{
    angleY += deltaX * sensitivity; // Horizontal rotation
    angleX += deltaY * sensitivity; // Vertical rotation
    clampValues();
}

void CameraController::setZoomLimits(float minDist, float maxDist)
{
    minDistance = minDist;
    maxDistance = maxDist;
    clampValues();
}

std::tuple<float, float, float> CameraController::calculatePosition() const
{
    float x = distance * cos(angleY * M_PI / 180.0f) * cos(angleX * M_PI / 180.0f);
    float y = distance * sin(angleX * M_PI / 180.0f);
    float z = distance * sin(angleY * M_PI / 180.0f) * cos(angleX * M_PI / 180.0f);

    return {x, y, z};
}

void CameraController::setupGLCamera() const
{
    std::tuple<float, float, float> pos = calculatePosition();
    float x = std::get<0>(pos);
    float y = std::get<1>(pos);
    float z = std::get<2>(pos);
    gluLookAt(x, y, z,        // Eye position (orbital camera)
              0.0, 0.0, 0.0,  // Look at center
              0.0, 1.0, 0.0); // Up vector
}

void CameraController::setDistance(float dist)
{
    distance = dist;
    clampValues();
}

void CameraController::setAngles(float x, float y)
{
    angleX = x;
    angleY = y;
    clampValues();
}

void CameraController::clampValues()
{
    // Clamp distance
    if (distance < minDistance)
        distance = minDistance;
    if (distance > maxDistance)
        distance = maxDistance;

    // Clamp vertical angle to prevent flipping
    if (angleX > 89.0f)
        angleX = 89.0f;
    if (angleX < 10.0f)
        angleX = 10.0f;

    // Horizontal angle can wrap around freely (no clamping needed)
}