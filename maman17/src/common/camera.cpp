#include "common/camera.h"
#include <GLUT/glut.h>
#include <cmath>
#include <algorithm>

// Only include cow headers if cow controls are available
#ifdef COW_CONTROLS_AVAILABLE
#include "cow/cow.h"
#include "cow/cow_coordinates.h"
#include "cow/cow_camera_constants.h"
#endif

CameraController::CameraController()
    : distance(5.0f), angleX(35.0f), angleY(55.0f), minDistance(1.0f), maxDistance(100.0f), currentMode(ORBITAL_CAMERA)
{
}

CameraController::CameraController(float distance, float angleX, float angleY)
    : distance(distance), angleX(angleX), angleY(angleY), minDistance(1.0f), maxDistance(100.0f), currentMode(ORBITAL_CAMERA)
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
    if (currentMode == COW_EYE_CAMERA)
    {
        setupCowEyeCamera();
    }
    else
    {
        // Orbital camera mode
        std::tuple<float, float, float> pos = calculatePosition();
        float x = std::get<0>(pos);
        float y = std::get<1>(pos);
        float z = std::get<2>(pos);
        gluLookAt(x, y, z,        // Eye position (orbital camera)
                  0.0, 0.0, 0.0,  // Look at center
                  0.0, 1.0, 0.0); // Up vector
    }
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

void CameraController::resetToDefaults()
{
    distance = 5.0f;
    angleX = 35.0f;
    angleY = 55.0f;
    currentMode = ORBITAL_CAMERA;
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

void CameraController::toggleCameraMode()
{
    if (currentMode == ORBITAL_CAMERA)
    {
        currentMode = COW_EYE_CAMERA;
    }
    else
    {
        currentMode = ORBITAL_CAMERA;
    }
}

void CameraController::setupCowEyeCamera() const
{
#ifdef COW_CONTROLS_AVAILABLE
    // Get cow's world position and rotation
    float cowX = getCowX();
    float cowZ = getCowZ();
    float cowRotation = getCowRotation();

    // Get head rotations (these should already be clamped by cow controls)
    float headRotationX = getHeadRotationX();
    float headRotationY = getHeadRotationY();

    // Double-check the limits are respected (safety clamp)
    headRotationX = std::max(-HEAD_MAX_X_ROTATION, std::min(HEAD_MAX_X_ROTATION, headRotationX));
    headRotationY = std::max(-HEAD_MAX_Y_ROTATION, std::min(HEAD_MAX_Y_ROTATION, headRotationY));

    // Calculate head position in world space
    // Start with cow's position
    float headWorldX = cowX;
    float headWorldY = COW_BASE_Y_POSITION; // Cow's base Y position from drawCow()
    float headWorldZ = cowZ;

    // Add head offset (from drawFullHead translation)
    float cowRotationRad = -cowRotation * M_PI / 180.0f; // Negate to fix camera rotation direction
    headWorldX += COW_HEAD_X * cos(cowRotationRad) - COW_HEAD_Z * sin(cowRotationRad);
    headWorldY += COW_HEAD_Y;
    headWorldZ += COW_HEAD_X * sin(cowRotationRad) + COW_HEAD_Z * cos(cowRotationRad);

    // Position camera at the front of the head (where eyes are)
    // Eyes are at position relative to head center
    float eyeOffsetX = COW_EYE_OFFSET_X;
    float eyeOffsetY = COW_EYE_OFFSET_Y;
    float eyeOffsetZ = COW_EYE_OFFSET_Z;

    // Apply head rotations to eye offset
    float headRotXRad = headRotationX * M_PI / 180.0f;
    float headRotYRad = headRotationY * M_PI / 180.0f;

    // For eye position, use simpler rotation matching the head visual rotation
    // Apply Y rotation (left/right turn, inverted for camera)
    float rotatedEyeX = eyeOffsetX * cos(-headRotYRad) - eyeOffsetZ * sin(-headRotYRad);
    float rotatedEyeZ = eyeOffsetX * sin(-headRotYRad) + eyeOffsetZ * cos(-headRotYRad);
    float rotatedEyeY = eyeOffsetY; // Keep Y position simple for now

    // Apply cow rotation to rotated eye position
    float finalEyeX = rotatedEyeX * cos(cowRotationRad) - rotatedEyeZ * sin(cowRotationRad);
    float finalEyeZ = rotatedEyeX * sin(cowRotationRad) + rotatedEyeZ * cos(cowRotationRad);

    // Final camera position
    float cameraX = headWorldX + finalEyeX;
    float cameraY = headWorldY + rotatedEyeY;
    float cameraZ = headWorldZ + finalEyeZ;

    // Calculate look direction based on head orientation
    // Default forward direction is positive X
    float lookDirectionX = 1.0f;
    float lookDirectionY = 0.0f;
    float lookDirectionZ = 0.0f;

    // Create proper first-person camera using spherical coordinates
    // Convert head rotations to proper yaw/pitch for camera
    float yaw = -headRotationY * M_PI / 180.0f;  // Left/right (inverted)
    float pitch = headRotationX * M_PI / 180.0f; // Up/down

    // Calculate look direction using spherical coordinates
    // This avoids gimbal lock by using proper first-person math
    float rotatedLookX = cos(pitch) * cos(yaw);
    float rotatedLookY = sin(pitch);
    float rotatedLookZ = cos(pitch) * sin(yaw);

    // Apply cow rotation to look direction
    float finalLookX = rotatedLookX * cos(cowRotationRad) - rotatedLookZ * sin(cowRotationRad);
    float finalLookZ = rotatedLookX * sin(cowRotationRad) + rotatedLookZ * cos(cowRotationRad);

    // Calculate target point (where the cow is looking)
    float targetX = cameraX + finalLookX;
    float targetY = cameraY + rotatedLookY;
    float targetZ = cameraZ + finalLookZ;

    // Set up the camera
    gluLookAt(cameraX, cameraY, cameraZ, // Eye position (cow's eye)
              targetX, targetY, targetZ, // Look at target
              0.0f, 1.0f, 0.0f);         // Up vector (world up)
#else
    // Fallback to orbital camera if cow controls are not available
    std::tuple<float, float, float> pos = calculatePosition();
    float x = std::get<0>(pos);
    float y = std::get<1>(pos);
    float z = std::get<2>(pos);
    gluLookAt(x, y, z,        // Eye position (orbital camera)
              0.0, 0.0, 0.0,  // Look at center
              0.0, 1.0, 0.0); // Up vector
#endif
}