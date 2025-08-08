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
const float TAIL_ROTATION_SPEED = 10.0f; // Degrees per key press
const float TAIL_MAX_ROTATION = 90.0f;   // Max rotation in any direction
const float TAIL_MAX_K_ROTATION = 30.0f; // Max rotation for SHIFT+K to prevent disappearing inside cow

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

// Movement actions enum
enum MovementAction
{
    MOVE_FORWARD,
    MOVE_BACKWARD,
    TURN_LEFT,
    TURN_RIGHT,
    NO_ACTION
};

// Function to map regular keys to actions
MovementAction mapRegularKey(unsigned char key)
{
    switch (key)
    {
    case 'w':
    case 'W':
        return MOVE_FORWARD;
    case 's':
    case 'S':
        return MOVE_BACKWARD;
    case 'a':
    case 'A':
        return TURN_LEFT;
    case 'd':
    case 'D':
        return TURN_RIGHT;
    default:
        return NO_ACTION;
    }
}

// Function to map special keys to actions
MovementAction mapSpecialKey(int key)
{
    switch (key)
    {
    case GLUT_KEY_UP:
        return MOVE_FORWARD;
    case GLUT_KEY_DOWN:
        return MOVE_BACKWARD;
    case GLUT_KEY_LEFT:
        return TURN_LEFT;
    case GLUT_KEY_RIGHT:
        return TURN_RIGHT;
    default:
        return NO_ACTION;
    }
}

// Single function to execute movement actions
void executeCowAction(MovementAction action)
{
    switch (action)
    {
    case MOVE_FORWARD:
    {
        // Use the head-tail direction as the forward direction
        Vector2D direction = getCowForwardDirection();
        cowX += COW_MOVEMENT_SPEED * direction.x;
        cowZ += COW_MOVEMENT_SPEED * direction.z;
        break;
    }
    case MOVE_BACKWARD:
    {
        // Move backward relative to head-tail direction
        Vector2D direction = getCowForwardDirection();
        cowX -= COW_MOVEMENT_SPEED * direction.x;
        cowZ -= COW_MOVEMENT_SPEED * direction.z;
        break;
    }
    case TURN_LEFT:
        cowRotation += COW_STEERING_ANGLE;
        break;
    case TURN_RIGHT:
        cowRotation -= COW_STEERING_ANGLE;
        break;
    case NO_ACTION:
        break; // No action needed
    }

    // Keep rotation between 0 and 360 degrees
    if (cowRotation >= 360.0f)
        cowRotation -= 360.0f;
    if (cowRotation < 0.0f)
        cowRotation += 360.0f;

    glutPostRedisplay(); // Request redraw
}

void handleCowMovement(unsigned char key, int x, int y)
{
    MovementAction action = mapRegularKey(key);
    executeCowAction(action);
}

void handleCowSpecialKeys(int key, int x, int y)
{
    MovementAction action = mapSpecialKey(key);
    executeCowAction(action);
}

float getCowX() { return cowX; }
float getCowZ() { return cowZ; }
float getCowRotation() { return cowRotation; }

// Unified cow body movement handler (head and tail controls)
void handleCowBodyMovement(unsigned char key, int x, int y)
{
    // Check if Shift modifier is pressed
    int modifiers = glutGetModifiers();
    bool shiftPressed = (modifiers & GLUT_ACTIVE_SHIFT) != 0;

    if (shiftPressed)
    {
        // Shift is pressed, handle tail movement
        switch (key)
        {
        case 'i': // Tail rotation (Z-axis negative)
        case 'I':
            tailRotationZ = std::max(tailRotationZ - TAIL_ROTATION_SPEED, -TAIL_MAX_ROTATION);
            break;
        case 'k': // Tail rotation (Z-axis positive) - limited to prevent disappearing inside cow
        case 'K':
            tailRotationZ = std::min(tailRotationZ + TAIL_ROTATION_SPEED, TAIL_MAX_K_ROTATION);
            break;
        case 'j': // Tail rotation (X-axis positive)
        case 'J':
            tailRotationX = std::min(tailRotationX + TAIL_ROTATION_SPEED, TAIL_MAX_ROTATION);
            break;
        case 'l': // Tail rotation (X-axis negative)
        case 'L':
            tailRotationX = std::max(tailRotationX - TAIL_ROTATION_SPEED, -TAIL_MAX_ROTATION);
            break;
        default:
            return; // Don't redraw if no tail movement
        }
    }
    else
    {
        // No shift, handle head movement
        switch (key)
        {
        case 'i': // Head up (rotate to look up)
        case 'I':
            headRotationX = std::min(headRotationX + HEAD_ROTATION_SPEED, HEAD_MAX_X_ROTATION);
            break;
        case 'k': // Head down (rotate to look down)
        case 'K':
            headRotationX = std::max(headRotationX - HEAD_ROTATION_SPEED, -HEAD_MAX_X_ROTATION);
            break;
        case 'j': // Head left
        case 'J':
            headRotationY = std::min(headRotationY + HEAD_ROTATION_SPEED, HEAD_MAX_Y_ROTATION);
            break;
        case 'l': // Head right
        case 'L':
            headRotationY = std::max(headRotationY - HEAD_ROTATION_SPEED, -HEAD_MAX_Y_ROTATION);
            break;
        default:
            return; // Don't redraw if no head movement
        }
    }

    glutPostRedisplay(); // Request redraw only if movement occurred
}

// Legacy function name for backwards compatibility
void handleHeadMovement(unsigned char key, int x, int y)
{
    handleCowBodyMovement(key, x, y);
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