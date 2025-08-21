#include <GLUT/glut.h>
#include "cow/cow.h"
#include "environment/environment.h"
#include "environment/lights.h"
#include "common/animation.h"
#include "common/camera.h"
#include "common/input.h"
#include <cmath>

// Global camera controller and input handler
static CameraController camera(10.0f, 35.0f, 55.0f);
static InputHandler *inputHandler = nullptr;

float worldSize = 50.0f;
/**
 * Orchestrates the complete 3D scene rendering pipeline for the cow meadow environment.
 * Sets up camera positioning, lighting systems, and renders all scene elements including
 * terrain, vegetation, props, and the cow itself. This function coordinates between
 * multiple rendering subsystems to create the final composite scene that the user sees.
 */
void drawMainScene()
{
    glLoadIdentity();

    // Use camera controller for positioning
    camera.setupGLCamera();

    // Setup sun lighting default position and intensity
    setupSunLighting();

    // Draw the sun in the sky
    drawSun();

    // Draw world-wide ground texture large enough to cover the entire scene
    drawWorldGround(worldSize * 3);

    // Draw a procedural meadow containing grass
    drawProceduralMeadow(worldSize, worldSize, 2000);

    // Draw multiple trees at different fixed positions
    drawTree(-8.0f, 0.0f, -5.0f, 1.2f);
    drawTree(12.0f, 0.0f, -8.0f, 0.9f);
    drawTree(-15.0f, 0.0f, 10.0f, 1.1f);
    drawTree(8.0f, 0.0f, 12.0f, 1.0f);
    drawTree(-3.0f, 0.0f, -15.0f, 0.8f);

    // Draw rocks scattered around the scene at random positions
    drawScatteredRocks(worldSize, 0.0f, worldSize, 10);

    // Draw a metal bench and a lamp post
    drawMetalBenchAndLamp(-4, 0.0f, -6, 1.0f, 30.0f);

    // Draw the cow in the center of the scene
    drawCow();
}

/**
 * Main OpenGL display callback that handles the complete frame rendering cycle.
 * Clears buffers, renders the 3D scene, overlays UI elements, and swaps buffers
 * to present the final frame. This is the core rendering entry point.
 */
void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawMainScene(); // Use custom main scene drawing

    // Draw UI overlay
    if (inputHandler)
    {
        inputHandler->drawUI();
    }

    glutSwapBuffers();
}

/**
 * Animation timer callback that advances the global animation frame counter.
 * Drives the tail's time-based animation ensuring smooth progression of animated elements.
 */
void customTimerCallback()
{
    incrementAnimationFrame(); // Keep animation frame for cow animations (tail, etc.)
}

/**
 * Application entry point that initializes OpenGL, sets up the 3D rendering environment,
 * configures camera and input systems, and launches the main event loop. Establishes
 * the complete application framework including window creation, lighting setup, input
 * handling, and animation timing before transferring control to the GLUT main loop.
 */
int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1024, 768);
    glutCreateWindow("OpenGL Cow in a Meadow");
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.5f, 0.8f, 1.0f, 1.0f); // Sky blue

    // Initialize camera with standard settings
    camera.setZoomLimits(1.0f, 100.0f);

    // Initialize input handler
    inputHandler = new InputHandler(camera);
    setCowInputHandler(inputHandler); // Register with cow system for auto-control enabling
    inputHandler->setDisplayCallback(customTimerCallback);
    inputHandler->handleCommandLineArgs(argc, argv); // No test scenes for main.cpp

    // Initialize cow movement
    initCowMovement();

    // Set up GLUT callbacks
    glutDisplayFunc(display);
    inputHandler->setupGLUTCallbacks();

    // Start timer for cow animations
    inputHandler->startTimer(50);

    glutMainLoop();

    delete inputHandler;
    return 0; // 0 means success in unix-like systems
}