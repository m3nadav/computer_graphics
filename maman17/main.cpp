#include <GLUT/glut.h>
#include "scene/scene.h"
#include "cow/cow.h"
#include "environment/environment.h"
#include "common/camera.h"
#include "common/input.h"
#include <cmath>

// Global camera and input handler
static CameraController camera;
static InputHandler *inputHandler = nullptr;

// Custom keyboard handler (cow controls now handled by InputHandler)
void customKeyboardHandler(unsigned char key, int x, int y)
{
    // Any additional custom key handling can go here
    // Cow controls are automatically handled by InputHandler when enabled
}

// Custom main scene drawing
void drawMainScene()
{
    glLoadIdentity();

    // Use camera controller for positioning
    camera.setupGLCamera();

    // Setup lighting for realistic rendering
    setupEnvironmentLighting();

    // Draw world-wide ground texture
    drawWorldGround(100.0f);

    // Draw enhanced procedural meadow (grass only, no ground base)
    drawProceduralMeadow(50.0f, 50.0f, 2000);

    // Draw multiple trees at different positions
    drawTree(-8.0f, 0.0f, -5.0f, 1.2f);
    drawTree(12.0f, 0.0f, -8.0f, 0.9f);
    drawTree(-15.0f, 0.0f, 10.0f, 1.1f);
    drawTree(8.0f, 0.0f, 12.0f, 1.0f);
    drawTree(-3.0f, 0.0f, -15.0f, 0.8f);

    // Draw rocks scattered around
    drawRock(-5.0f, 0.0f, 8.0f, 1.0f);
    drawRock(10.0f, 0.0f, 3.0f, 0.7f);
    drawRock(-12.0f, 0.0f, -2.0f, 1.3f);
    drawRock(15.0f, 0.0f, -12.0f, 0.9f);
    drawRock(2.0f, 0.0f, 18.0f, 1.1f);

    // Draw metal benches
    drawMetalBench(6.0f, 0.0f, -3.0f, 1.0f);
    drawMetalBench(-10.0f, 0.0f, 6.0f, 0.8f);

    // Draw the cow in the center
    drawCow();
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawMainScene(); // Use custom main scene drawing
    glutSwapBuffers();
}

// Timer callback for cow animations
void customTimerCallback()
{
    incrementAnimationFrame(); // Keep animation frame for cow animations (tail, etc.)
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("OpenGL Cow in a Meadow");
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.5f, 0.8f, 1.0f, 1.0f); // Sky blue

    // Initialize camera with standard settings
    camera.setZoomLimits(1.0f, 100.0f);

    // Initialize input handler
    inputHandler = new InputHandler(camera);
    inputHandler->enableCowControls(true); // Enable cow controls for main.cpp
    inputHandler->setKeyboardCallback(customKeyboardHandler);
    inputHandler->setDisplayCallback(customTimerCallback);
    inputHandler->handleCommandLineArgs(argc, argv); // No test scenes for main.cpp

    // Initialize cow movement
    initCowMovement();

    // Set up GLUT callbacks
    glutDisplayFunc(display);
    inputHandler->setupGLUTCallbacks();
    glutSpecialFunc(handleCowSpecialKeys); // Keep special keys for cow movement

    // Start timer for cow animations
    inputHandler->startTimer(50);

    glutMainLoop();

    delete inputHandler;
    return 0;
}