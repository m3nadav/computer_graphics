#include "cow/cow.h"
#include "cow/head.h"
#include "cow/body.h"
#include "cow/tail.h"
#include "cow/cow_coordinates.h"
#include "shapes/shapes.h"
#include <GLUT/glut.h>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>

// Global cow movement variables
static float cowX = 0.0f;
static float cowZ = 0.0f;
static float cowRotation = 0.0f; // Rotation in degrees around Y-axis

// Global head movement variables
static float headRotationX = 0.0f; // Up/down rotation around Z-axis (-30 to +30 degrees)
static float headRotationY = 0.0f; // Left/right rotation around Y-axis (-45 to +45 degrees)

// Global tail movement variables
static float tailRotationZ = 0.0f; // SHIFT+I/K rotation around Z-axis (-90 to +90 degrees)
static float tailRotationX = 0.0f; // SHIFT+J/L rotation around X-axis (-90 to +90 degrees)

void initCowMovement()
{
    cowX = 0.0f;
    cowZ = 0.0f;
    cowRotation = 0.0f;
    headRotationX = 0.0f;
    headRotationY = 0.0f;
    tailRotationZ = 0.0f;
    tailRotationX = 0.0f;
}

// Movement constants - easily tweakable
const float COW_MOVEMENT_SPEED = 0.5f;  // Units per key press
const float COW_STEERING_ANGLE = 10.0f; // Degrees per key press

// Head movement constants
const float HEAD_ROTATION_SPEED = 5.0f;  // Degrees per key press
const float HEAD_MAX_X_ROTATION = 30.0f; // Max up/down rotation
const float HEAD_MAX_Y_ROTATION = 45.0f; // Max left/right rotation

// Tail movement constants
const float TAIL_ROTATION_SPEED = 15.0f; // Degrees per key press (increased for more visibility)
const float TAIL_MAX_ROTATION = 90.0f;   // Max rotation in any direction
const float TAIL_MAX_K_ROTATION = 45.0f; // Max rotation for CTRL+S to prevent disappearing inside cow

// Structure to represent a 2D vector
struct Vector2D
{
    float x, z;
    Vector2D(float x = 0.0f, float z = 0.0f) : x(x), z(z) {}
};

Vector2D getCowForwardDirection()
{
    // The cow's head faces along the positive X-axis relative to the cow's body
    // So the forward direction is simply the cow's rotation applied to (1, 0)
    // Note: In OpenGL with our camera setup, negative Z is "forward" from camera perspective
    float angleRadians = cowRotation * M_PI / 180.0f;
    float forwardX = cos(angleRadians);
    float forwardZ = -sin(angleRadians); // Negative because -Z is "forward" in OpenGL

    return Vector2D(forwardX, forwardZ);
}

void handleCowMovement(unsigned char key, int x, int y)
{
    // Only process cow movement if no modifier keys are pressed
    int modifiers = glutGetModifiers();
    bool shiftPressed = (modifiers & GLUT_ACTIVE_SHIFT) != 0;
    bool ctrlPressed = (modifiers & GLUT_ACTIVE_CTRL) != 0;
    bool altPressed = (modifiers & GLUT_ACTIVE_ALT) != 0;

    // Don't process if modifiers are pressed (handled by body movement)
    if (shiftPressed || ctrlPressed || altPressed)
    {
        return;
    }

    switch (key)
    {
    case 'w':
    case 'W':
    {
        // Use the head-tail direction as the forward direction
        Vector2D direction = getCowForwardDirection();
        cowX += COW_MOVEMENT_SPEED * direction.x;
        cowZ += COW_MOVEMENT_SPEED * direction.z;
        break;
    }
    case 's':
    case 'S':
    {
        // Move backward relative to head-tail direction
        Vector2D direction = getCowForwardDirection();
        cowX -= COW_MOVEMENT_SPEED * direction.x;
        cowZ -= COW_MOVEMENT_SPEED * direction.z;
        break;
    }
    case 'a':
    case 'A':
        cowRotation += COW_STEERING_ANGLE;
        break;
    case 'd':
    case 'D':
        cowRotation -= COW_STEERING_ANGLE;
        break;
    default:
        break;
    }

    // Keep rotation between 0 and 360 degrees
    if (cowRotation >= 360.0f)
        cowRotation -= 360.0f;
    if (cowRotation < 0.0f)
        cowRotation += 360.0f;

    glutPostRedisplay(); // Request redraw
}

float getCowX() { return cowX; }
float getCowZ() { return cowZ; }
float getCowRotation() { return cowRotation; }

// Unified cow body movement handler (head and tail controls)
bool handleCowBodyMovement(unsigned char key, int x, int y)
{
    // Check modifiers
    int modifiers = glutGetModifiers();
    bool shiftPressed = (modifiers & GLUT_ACTIVE_SHIFT) != 0;
    bool altPressed = (modifiers & GLUT_ACTIVE_ALT) != 0;

    if (shiftPressed)
    {
        // Shift + WASD for head movement
        switch (key)
        {
        case 'w': // Head up (rotate to look up)
        case 'W':
            headRotationX = std::min(headRotationX + HEAD_ROTATION_SPEED, HEAD_MAX_X_ROTATION);
            glutPostRedisplay();
            return true; // Handled, don't process as regular movement
        case 's':        // Head down (rotate to look down)
        case 'S':
            headRotationX = std::max(headRotationX - HEAD_ROTATION_SPEED, -HEAD_MAX_X_ROTATION);
            glutPostRedisplay();
            return true;
        case 'a': // Head left
        case 'A':
            headRotationY = std::min(headRotationY + HEAD_ROTATION_SPEED, HEAD_MAX_Y_ROTATION);
            glutPostRedisplay();
            return true;
        case 'd': // Head right
        case 'D':
            headRotationY = std::max(headRotationY - HEAD_ROTATION_SPEED, -HEAD_MAX_Y_ROTATION);
            glutPostRedisplay();
            return true;
        default:
            return false; // Not handled
        }
    }
    else if (altPressed)
    {
        // Ctrl/Alt + WASD for tail movement (trying both for macOS compatibility)
        switch (key)
        {
        case 'w': // Tail rotation (Z-axis negative)
        case 'W':
            tailRotationZ = std::max(tailRotationZ - TAIL_ROTATION_SPEED, -TAIL_MAX_ROTATION);
            glutPostRedisplay();
            return true; // Handled, don't process as regular movement
        case 's':        // Tail rotation (Z-axis positive) - limited to prevent disappearing inside cow
        case 'S':
            tailRotationZ = std::min(tailRotationZ + TAIL_ROTATION_SPEED, TAIL_MAX_K_ROTATION);
            glutPostRedisplay();
            return true;
        case 'a': // Tail rotation (X-axis positive)
        case 'A':
            tailRotationX = std::min(tailRotationX + TAIL_ROTATION_SPEED, TAIL_MAX_ROTATION);
            glutPostRedisplay();
            return true;
        case 'd': // Tail rotation (X-axis negative)
        case 'D':
            tailRotationX = std::max(tailRotationX - TAIL_ROTATION_SPEED, -TAIL_MAX_ROTATION);
            glutPostRedisplay();
            return true;
        default:
            return false; // Not handled
        }
    }

    return false; // No modifier keys pressed or key not handled
}

// Head rotation getters
float getHeadRotationX() { return headRotationX; }
float getHeadRotationY() { return headRotationY; }

// Tail rotation getters
float getTailRotationZ() { return tailRotationZ; }
float getTailRotationX() { return tailRotationX; }

void drawCow()
{
    glPushMatrix();

    // Apply cow's position and rotation
    glTranslatef(cowX, 0.68, cowZ);
    glRotatef(cowRotation, 0.0f, 1.0f, 0.0f);

    drawBody();
    drawFullHead();
    drawLegs();
    drawTail();

    glPopMatrix();
}