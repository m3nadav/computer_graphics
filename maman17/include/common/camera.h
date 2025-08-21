#ifndef CAMERA_H
#define CAMERA_H

#include <tuple>

// Camera modes
enum CameraMode
{
    ORBITAL_CAMERA,
    COW_EYE_CAMERA
};

// Camera control system for orbital camera with zoom and cow eye camera
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

    // Camera mode control
    void setCameraMode(CameraMode mode) { currentMode = mode; }
    CameraMode getCameraMode() const { return currentMode; }
    void toggleCameraMode(); // Toggle between orbital and cow eye camera

    // Cow eye camera functions
    void setupCowEyeCamera() const; // Sets up gluLookAt for cow eye camera

    // Getters for current state
    float getDistance() const { return distance; }
    float getAngleX() const { return angleX; }
    float getAngleY() const { return angleY; }

    // Setters for direct control
    void setDistance(float dist);
    void setAngles(float x, float y);

    // Anchor point control methods
    void moveAnchor(float deltaX, float deltaY, float deltaZ);
    void setAnchor(float x, float y, float z);
    std::tuple<float, float, float> getAnchor() const;

    // Direction-relative anchor movement methods
    void moveAnchorForward(float distance);
    void moveAnchorBackward(float distance);
    void moveAnchorLeft(float distance);
    void moveAnchorRight(float distance);

    // Reset to default values
    void resetToDefaults();

private:
    float distance;
    float angleX; // Vertical angle (elevation)
    float angleY; // Horizontal angle (azimuth)
    float minDistance;
    float maxDistance;
    CameraMode currentMode;

    // Anchor point coordinates (where the camera looks at)
    float anchorX;
    float anchorY;
    float anchorZ;

    // Starting values (defaults for this instance)
    float startingDistance;
    float startingAngleX;
    float startingAngleY;
    CameraMode startingMode;
    float startingAnchorX;
    float startingAnchorY;
    float startingAnchorZ;

    void clampValues();
};

#endif