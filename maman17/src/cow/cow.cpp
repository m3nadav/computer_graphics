#define GL_SILENCE_DEPRECATION // Silence deprecation warnings on macOS
#include "cow/cow.h"
#include "cow/head.h"
#include "cow/body.h"
#include "cow/cow_coordinates.h"
#include "shapes/shapes.h"
#include <GLUT/glut.h>
#include <cmath>
#include <cstdlib>
#include <ctime>

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

// Function to calculate the head-tail tangent vector based on actual head and tail positions
Vector2D calculateHeadTailTangent()
{
    // Calculate the head-tail direction vector (from tail to head)
    float deltaX = COW_HEAD_X - COW_TAIL_X;
    float deltaZ = COW_HEAD_Z - COW_TAIL_Z;

    // Normalize the vector to get the unit tangent
    float length = sqrt(deltaX * deltaX + deltaZ * deltaZ);
    if (length > 0.001f)
    {
        deltaX /= length;
        deltaZ /= length;
    }

    // Apply the cow's current rotation to get the actual facing direction
    float angleRadians = cowRotation * M_PI / 180.0f;
    float cosAngle = cos(angleRadians);
    float sinAngle = sin(angleRadians);

    // Rotate the head-tail vector by the cow's rotation
    float rotatedX = deltaX * cosAngle - deltaZ * sinAngle;
    float rotatedZ = deltaX * sinAngle + deltaZ * cosAngle;

    return Vector2D(rotatedX, rotatedZ);
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
        // Move forward along the cow's actual head-tail axis
        Vector2D tangent = calculateHeadTailTangent();
        cowX += COW_MOVEMENT_SPEED * tangent.x;
        cowZ += COW_MOVEMENT_SPEED * tangent.z;
        break;
    }
    case MOVE_BACKWARD:
    {
        // Move backward opposite to the cow's actual head-tail axis
        Vector2D tangentBack = calculateHeadTailTangent();
        cowX -= COW_MOVEMENT_SPEED * tangentBack.x;
        cowZ -= COW_MOVEMENT_SPEED * tangentBack.z;
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

    drawBody();
    drawFullHead();
    drawLegs();
    drawTail();

    glPopMatrix();
}