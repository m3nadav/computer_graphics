#ifndef CAMERA_H
#define CAMERA_H

#include <tuple>

// Camera control system for orbital camera with zoom
class CameraController
{
public:
    // Initialize camera with default values
    CameraController();

    // Initialize camera with custom starting position
    CameraController(float distance, float angleX, float angleY);

    // Camera control functions
    void zoomIn(float step = 0.5f);
    void zoomOut(float step = 0.5f);
    void rotate(float deltaX, float deltaY, float sensitivity = 0.5f);
    void setZoomLimits(float minDist, float maxDist);

    // Camera position calculation
    std::tuple<float, float, float> calculatePosition() const;
    void setupGLCamera() const; // Sets up gluLookAt for current camera position

    // Getters for current state
    float getDistance() const { return distance; }
    float getAngleX() const { return angleX; }
    float getAngleY() const { return angleY; }

    // Setters for direct control
    void setDistance(float dist);
    void setAngles(float x, float y);

private:
    float distance;
    float angleX; // Vertical angle (elevation)
    float angleY; // Horizontal angle (azimuth)
    float minDistance;
    float maxDistance;

    void clampValues();
};

#endif // CAMERA_H