#include "common/input.h"
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
    : cameraController(camera), rightMouseDown(false), lastMouseX(0), lastMouseY(0),
      timerActive(false), timerInterval(50), startingScene(0), cowControlsEnabled(false)
{
    instance = this; // Set static instance for GLUT callbacks
}

void InputHandler::setupGLUTCallbacks()
{
    glutKeyboardFunc(keyboardWrapper);
    glutMouseFunc(mouseWrapper);
    glutMotionFunc(motionWrapper);
    glutReshapeFunc(reshapeWrapper);
}

void InputHandler::startTimer(int intervalMs)
{
    timerInterval = intervalMs;
    timerActive = true;
    glutTimerFunc(intervalMs, timerWrapper, 0);
}

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
    else
    {
        // Try cow controls if enabled
        if (cowControlsEnabled)
        {
            handleCowControls(key, x, y);
        }

        // Pass to custom callback for additional handling
        if (keyboardCallback)
        {
            keyboardCallback(key, x, y);
        }
    }
}

void InputHandler::handleCowControls(unsigned char key, int x, int y)
{
// Forward to cow control functions if they exist
#ifdef COW_CONTROLS_AVAILABLE
    handleCowMovement(key, x, y);
    handleHeadMovement(key, x, y);
#endif
}

// ========================================
// MOUSE HANDLING METHODS
// ========================================

void InputHandler::handleMouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        menuSystem.handleMenuClick(x, y);
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

void InputHandler::drawUI()
{
    menuSystem.drawUI();
}