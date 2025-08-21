#include "common/input.h"
#include "environment/lights.h"
#include "environment/environment.h"
#include <GLUT/glut.h>
#include <cstdlib>
#include <iostream>

// Include cow controls if available
#ifdef COW_CONTROLS_AVAILABLE
#include "cow/cow.h"
#endif

// Static instance for GLUT callbacks
InputHandler *InputHandler::instance = nullptr;

InputHandler::InputHandler(CameraController &camera)
    : cameraController(camera), rightMouseDown(false), leftMouseDown(false), lastMouseX(0), lastMouseY(0),
      timerActive(false), timerInterval(50), startingScene(0), cowControlsEnabled(false)
{
    instance = this; // Set static instance for GLUT callbacks
}

/** Registers this InputHandler as the active GLUT callback handler for all input events. */
void InputHandler::setupGLUTCallbacks()
{
    glutKeyboardFunc(keyboardWrapper);
    glutSpecialFunc(specialKeysWrapper);
    glutMouseFunc(mouseWrapper);
    glutMotionFunc(motionWrapper);
    glutReshapeFunc(reshapeWrapper);
}

/** Begins the animation timer with specified interval for periodic updates. */
void InputHandler::startTimer(int intervalMs)
{
    timerInterval = intervalMs;
    timerActive = true;
    glutTimerFunc(intervalMs, timerWrapper, 0);
}

/** Processes command line arguments to set initial scene selection. */
void InputHandler::handleCommandLineArgs(int argc, char **argv, int maxScenes)
{
    if (argc > 1 && maxScenes > 0)
    {
        int sceneArg = atoi(argv[1]);
        if (sceneArg >= 1 && sceneArg <= maxScenes)
        {
            startingScene = sceneArg - 1; // Convert to 0-based index
        }
    }
}

// Static GLUT callback wrappers
void InputHandler::keyboardWrapper(unsigned char key, int x, int y)
{
    if (instance)
        instance->handleKeyboard(key, x, y);
}

void InputHandler::specialKeysWrapper(int key, int x, int y)
{
    if (instance)
        instance->handleSpecialKeys(key, x, y);
}

void InputHandler::mouseWrapper(int button, int state, int x, int y)
{
    if (instance)
        instance->handleMouse(button, state, x, y);
}

void InputHandler::motionWrapper(int x, int y)
{
    if (instance)
        instance->handleMotion(x, y);
}

void InputHandler::timerWrapper(int value)
{
    if (instance)
        instance->handleTimer(value);
}

void InputHandler::reshapeWrapper(int w, int h)
{
    if (instance)
        instance->handleReshape(w, h);
}

// ========================================
// KEYBOARD HANDLING METHODS
// ========================================

/** Processes standard keyboard input including cow controls and camera commands. */
void InputHandler::handleKeyboard(unsigned char key, int x, int y)
{
    // Handle common keys first
    if (key == 27) // ESC
    {
        menuSystem.applyQuitClick();
    }
    else if (key == '+' || key == '=') // Zoom in
    {
        cameraController.zoomIn();
        glutPostRedisplay();
    }
    else if (key == '-' || key == '_') // Zoom out
    {
        cameraController.zoomOut();
        glutPostRedisplay();
    }
    else if (key == 'm' || key == 'M') // Menu toggle
    {
        menuSystem.applyMenuClick();
        glutPostRedisplay();
    }
    else if (key == 'h' || key == 'H') // Help
    {
        menuSystem.applyHelpClick();
        glutPostRedisplay();
    }
    else if (key == 'c' || key == 'C') // Camera toggle
    {
        cameraController.toggleCameraMode();
        glutPostRedisplay();
    }
    else if (key == 't' || key == 'T') // Toggle lamp light
    {
        static bool lampOn = true;
        lampOn = !lampOn;
        enableLampLight(lampOn);
        glutPostRedisplay();
    }
    else if (key == 'r' || key == 'R') // Reset scene
    {
        // Reset camera to defaults
        cameraController.resetToDefaults();

        // Reset all lighting to defaults
        resetLightingToDefaults();

// Reset cow position and rotations (if available)
#ifdef COW_CONTROLS_AVAILABLE
        initCowMovement();
#endif

        glutPostRedisplay();
    }
    else if (key == 'i' || key == 'I') // Lamp direction up (pitch)
    {
        float currentAngleX, currentAngleZ;
        getLampDirection(&currentAngleX, &currentAngleZ);
        setLampDirection(currentAngleX + 5.0f, currentAngleZ);
        glutPostRedisplay();
    }
    else if (key == 'k' || key == 'K') // Lamp direction down (pitch)
    {
        float currentAngleX, currentAngleZ;
        getLampDirection(&currentAngleX, &currentAngleZ);
        setLampDirection(currentAngleX - 5.0f, currentAngleZ);
        glutPostRedisplay();
    }
    else if (key == 'j' || key == 'J') // Lamp direction left (yaw)
    {
        float currentAngleX, currentAngleZ;
        getLampDirection(&currentAngleX, &currentAngleZ);
        setLampDirection(currentAngleX, currentAngleZ - 5.0f);
        glutPostRedisplay();
    }
    else if (key == 'l' || key == 'L') // Lamp direction right (yaw)
    {
        float currentAngleX, currentAngleZ;
        getLampDirection(&currentAngleX, &currentAngleZ);
        setLampDirection(currentAngleX, currentAngleZ + 5.0f);
        glutPostRedisplay();
    }
    else if (key == '[') // Decrease lamp intensity
    {
        float intensity = getLampIntensity();
        setLampIntensity(intensity - 0.2f);
        glutPostRedisplay();
    }
    else if (key == ']') // Increase lamp intensity
    {
        float intensity = getLampIntensity();
        setLampIntensity(intensity + 0.2f);
        glutPostRedisplay();
    }
    // Sun position controls (number keys 1-4)
    else if (key == '1') // Sun position (-x,y,-z)
    {
        setSunPosition(0);
        enableCustomLightPosition(false);
        glutPostRedisplay();
    }
    else if (key == '2') // Sun position (-x,y,z)
    {
        setSunPosition(1);
        enableCustomLightPosition(false);
        glutPostRedisplay();
    }
    else if (key == '3') // Sun position (x,y,-z)
    {
        setSunPosition(2);
        enableCustomLightPosition(false);
        glutPostRedisplay();
    }
    else if (key == '4') // Sun position (x,y,z)
    {
        setSunPosition(3);
        enableCustomLightPosition(false);
        glutPostRedisplay();
    }
    else
    {
        // Try cow controls if enabled
        if (cowControlsEnabled)
        {
            if (handleCowControls(key, x, y))
            {
                return; // If cow controls handled the key, don't pass to custom callback
            }
        }

        // Pass to custom callback for additional handling
        if (keyboardCallback)
        {
            keyboardCallback(key, x, y);
        }
    }
}

bool InputHandler::handleCowControls(unsigned char key, int x, int y)
{
// Forward to cow control functions if they exist
#ifdef COW_CONTROLS_AVAILABLE
    // First try body movement (head/tail with modifiers) - returns true if handled
    if (handleCowHeadAndTailMovement(key, x, y))
    {
        return true; // Key was handled by body movement
    }

    // If not handled by body movement, try regular cow movement
    return handleCowMovement(key, x, y); // Return whether the key was actually handled
#else
    return false; // Cow controls not available
#endif
}

// ========================================
// SPECIAL KEY HANDLING METHODS
// ========================================

void InputHandler::handleSpecialKeys(int key, int x, int y)
{
    // Define camera anchor movement step size
    const float ANCHOR_MOVE_STEP = 1.0f;

    // Arrow keys now directly control camera anchor point with direction-relative movement
    switch (key)
    {
    case GLUT_KEY_UP:
        cameraController.moveAnchorForward(ANCHOR_MOVE_STEP);
        glutPostRedisplay();
        return;
    case GLUT_KEY_DOWN:
        cameraController.moveAnchorBackward(ANCHOR_MOVE_STEP);
        glutPostRedisplay();
        return;
    case GLUT_KEY_LEFT:
        cameraController.moveAnchorLeft(ANCHOR_MOVE_STEP);
        glutPostRedisplay();
        return;
    case GLUT_KEY_RIGHT:
        cameraController.moveAnchorRight(ANCHOR_MOVE_STEP);
        glutPostRedisplay();
        return;
    }

    // Pass to custom special key callback if set
    if (specialKeyCallback)
    {
        specialKeyCallback(key, x, y);
    }
}

// ========================================
// MOUSE HANDLING METHODS
// ========================================

/** Handles mouse button press and release events for camera control. */
void InputHandler::handleMouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON)
    {
        if (state == GLUT_DOWN)
        {
            // Handle menu clicks only (lamp control now via IJKL keys)
            menuSystem.handleMenuClick(x, y);
        }
        // Note: Left mouse drag removed - no longer tracking for lamp control
    }
    else if (button == GLUT_RIGHT_BUTTON)
    {
        if (state == GLUT_DOWN)
        {
            rightMouseDown = true;
            lastMouseX = x;
            lastMouseY = y;
        }
        else if (state == GLUT_UP)
        {
            rightMouseDown = false;
        }
    }
}

/** Processes mouse movement for camera rotation when mouse buttons are held. */
void InputHandler::handleMotion(int x, int y)
{
    if (rightMouseDown)
    {
        int deltaX = x - lastMouseX;
        int deltaY = y - lastMouseY;

        cameraController.rotate(deltaX, deltaY);

        lastMouseX = x;
        lastMouseY = y;

        glutPostRedisplay();
    }
    // Note: Left mouse drag lamp control removed - now using IJKL keys for lamp direction
}

// ========================================
// OTHER HANDLERS
// ========================================

void InputHandler::handleTimer(int value)
{
    if (displayCallback)
    {
        displayCallback(); // Custom timer callback
    }

    glutPostRedisplay();

    if (timerActive)
    {
        glutTimerFunc(timerInterval, timerWrapper, 0);
    }
}

void InputHandler::handleReshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (float)w / h, 1.0, 1000.0);
    glMatrixMode(GL_MODELVIEW);
}

// ========================================
// UI DRAWING METHODS
// ========================================

/** Renders on-screen user interface elements and control information. */
void InputHandler::drawUI()
{
    menuSystem.drawUI();
}