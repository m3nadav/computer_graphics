#include <GLUT/glut.h>
#include "scene/scene.h"
#include "cow/cow.h"
#include "environment/environment.h"
#include "environment/lights.h"
#include "common/camera.h"
#include "common/input.h"
#include <cmath>

// Global camera and input handler
static CameraController camera(10.0f, 35.0f, 55.0f);
static InputHandler *inputHandler = nullptr;

// LAMP POST CONTROLS:
// - Left Mouse Drag: Rotate lamp direction (yaw and pitch)
// - L key: Toggle lamp on/off
// - [ key: Decrease lamp intensity
// - ] key: Increase lamp intensity

// Custom keyboard handler (cow controls now handled by InputHandler)
void customKeyboardHandler(unsigned char key, int x, int y)
{
    // Handle sun position controls
    handleSunControls(key, x, y);

    // Handle lamp controls
    switch (key)
    {
    case '[':
        // Decrease lamp intensity
        {
            float intensity = getLampIntensity();
            setLampIntensity(intensity - 0.2f);
            glutPostRedisplay();
        }
        break;
    case ']':
        // Increase lamp intensity
        {
            float intensity = getLampIntensity();
            setLampIntensity(intensity + 0.2f);
            glutPostRedisplay();
        }
        break;
    }

    // Any additional custom key handling can go here
    // Cow controls are automatically handled by InputHandler when enabled
}

float worldSize = 50.0f;
// Custom main scene drawing
void drawMainScene()
{
    glLoadIdentity();

    // Use camera controller for positioning
    camera.setupGLCamera();

    // Setup sun lighting
    setupSunLighting();

    // Draw the sun in the sky
    drawSun();

    // Draw world-wide ground texture
    drawWorldGround(worldSize * 3);

    // Draw enhanced procedural meadow (grass only, no ground base)
    drawProceduralMeadow(worldSize, worldSize, 2000);

    // Draw multiple trees at different positions
    drawTree(-8.0f, 0.0f, -5.0f, 1.2f);
    drawTree(12.0f, 0.0f, -8.0f, 0.9f);
    drawTree(-15.0f, 0.0f, 10.0f, 1.1f);
    drawTree(8.0f, 0.0f, 12.0f, 1.0f);
    drawTree(-3.0f, 0.0f, -15.0f, 0.8f);

    // Draw rocks scattered around
    drawScatteredRocks(worldSize, 0.0f, worldSize, 10);

    // Draw metal benches
    drawMetalBenchAndLamp(-4, 0.0f, -6, 1.0f, 30.0f);

    // Draw the cow in the center
    drawCow();
}

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

// Timer callback for cow animations
void customTimerCallback()
{
    incrementAnimationFrame(); // Keep animation frame for cow animations (tail, etc.)
}

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
    inputHandler->setKeyboardCallback(customKeyboardHandler);
    inputHandler->setDisplayCallback(customTimerCallback);
    inputHandler->handleCommandLineArgs(argc, argv); // No test scenes for main.cpp

    // Initialize cow movement
    initCowMovement();

    // Set up GLUT callbacks
    glutDisplayFunc(display);
    inputHandler->setupGLUTCallbacks(); // This now includes special key handling

    // Start timer for cow animations
    inputHandler->startTimer(50);

    glutMainLoop();

    delete inputHandler;
    return 0;
}