#define GL_SILENCE_DEPRECATION // Silence deprecation warnings on macOS
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

void initCowMovement()
{
    cowX = 0.0f;
    cowZ = 0.0f;
    cowRotation = 0.0f;
}

// Movement constants - easily tweakable
const float COW_MOVEMENT_SPEED = 0.5f;  // Units per key press
const float COW_STEERING_ANGLE = 10.0f; // Degrees per key press

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

void drawCow()
{
    glPushMatrix();

    // Apply cow's position and rotation
    glTranslatef(cowX, 0.0f, cowZ);
    glRotatef(cowRotation, 0.0f, 1.0f, 0.0f);

    // Debug: Print cow position and rotation
    std::cout << "[DEBUG] Cow Position: (" << cowX << ", 0, " << cowZ << "), Rotation: " << cowRotation << "°" << std::endl;

    // Debug: Calculate and print body center coordinates (should be at cow position)
    std::cout << "[DEBUG] Body Center: (" << cowX << ", 0, " << cowZ << ")" << std::endl;

    // Debug: Calculate and print tail coordinates after rotation
    float angleRadians = cowRotation * M_PI / 180.0f;
    float tailX = cowX + COW_TAIL_X * cos(angleRadians) - COW_TAIL_Z * sin(angleRadians);
    float tailZ = cowZ + COW_TAIL_X * sin(angleRadians) + COW_TAIL_Z * cos(angleRadians);
    std::cout << "[DEBUG] Tail Position: (" << tailX << ", " << COW_TAIL_Y << ", " << tailZ << ")" << std::endl;
    std::cout << "[DEBUG] ---" << std::endl;

    drawBody();
    drawFullHead();
    drawLegs();
    drawTail();

    glPopMatrix();
}