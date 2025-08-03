#include <GLUT/glut.h>
#include <iostream>
#include <vector>
#include <functional>
#include <cmath>

#include "cow/spots.h"
#include "cow/cow.h"
#include "scene/scene.h"
#include "environment/lights.h"
#include "common/camera.h"
#include "common/input.h"
#include "common/drawing.h"

// Global camera and input handler
static CameraController camera;
static InputHandler *inputHandler = nullptr;

// Using DrawingUtils from common/drawing.h

// Test: drawCow test
void testDrawCow()
{
    camera.setupGLCamera();

    // Setup lighting for realistic rendering (same as main.cpp)
    setupEnvironmentLighting();

    DrawingUtils::drawGrid(3.0f, 0.5f); // Add grid
    drawCow();
}

std::vector<std::pair<std::string, std::function<void()>>> tests = {
    {"Draw Cow Test", testDrawCow},
};

int currentTest = 0;

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    tests[currentTest].second();
    glutSwapBuffers();
}

// Custom keyboard handler for test switching (cow controls now handled by InputHandler)
void customKeyboardHandler(unsigned char key, int x, int y)
{
    if (key >= '1' && key < '1' + tests.size())
    {
        currentTest = key - '1';
        std::cout << "Switched to: " << tests[currentTest].first << std::endl;
        std::cout.flush();
        glutPostRedisplay();
    }
    // Cow controls are automatically handled by InputHandler when enabled
}

void specialKeys(int key, int x, int y)
{
    // Handle cow movement special keys (arrow keys)
    handleCowSpecialKeys(key, x, y);
}

// Timer callback for cow animations
void customTimerCallback()
{
    incrementAnimationFrame(); // Keep animation frame for cow animations (tail, etc.)
}

int main(int argc, char **argv)
{
    std::cout << "[DEBUG] main() starting" << std::endl;
    std::cout.flush();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(900, 700);
    glutCreateWindow("Visual Test Harness - Right-click to rotate, Mouse wheel or +/- to zoom");
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.5f, 0.8f, 1.0f, 1.0f); // Sky blue background like main.cpp

    // Initialize camera with standard settings
    camera.setZoomLimits(1.0f, 100.0f);

    // Initialize input handler
    inputHandler = new InputHandler(camera);
    inputHandler->enableCowControls(true); // Enable cow controls for visual test
    inputHandler->setKeyboardCallback(customKeyboardHandler);
    inputHandler->setDisplayCallback(customTimerCallback);
    inputHandler->handleCommandLineArgs(argc, argv, (int)tests.size());

    // Set starting test if provided via command line
    currentTest = inputHandler->getStartingScene();

    // Initialize cow movement
    initCowMovement();

    // Set up GLUT callbacks
    glutDisplayFunc(display);
    inputHandler->setupGLUTCallbacks();
    glutSpecialFunc(specialKeys); // Keep special keys for cow movement

    // Start timer for cow animations
    inputHandler->startTimer(50);

    std::cout << "Visual Test Harness - New Version\n";
    for (size_t i = 0; i < tests.size(); ++i)
        std::cout << (i + 1) << ": " << tests[i].first << std::endl;
    std::cout << "Press number key to switch test, ESC to exit.\n";
    std::cout << "Right-click and drag to rotate camera.\n";
    std::cout << "Mouse wheel or press +/- to zoom in/out.\n";
    std::cout << "Use WASD or Arrow keys to move the cow.\n";
    std::cout << "Use IJKL keys to move the cow's head.\n";
    std::cout.flush();

    glutMainLoop();

    delete inputHandler;
    return 0;
}