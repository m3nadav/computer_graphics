#include <GLUT/glut.h>
#include <iostream>
#include <vector>
#include <functional>
#include <cmath>

#include "environment/environment.h"
#include "environment/lights.h"
#include "shapes/shapes.h"
#include "common/camera.h"
#include "common/input.h"
#include "common/drawing.h"

// Global camera and input handler
static CameraController camera;
static InputHandler *inputHandler = nullptr;

// Using common drawing utilities

// Test: Single tree
void testTree()
{
    camera.setupGLCamera();

    setupEnvironmentLighting();
    drawWorldGround(20.0f);
    DrawingUtils::drawGrid();
    DrawingUtils::drawAxes();

    // Draw a single tree at origin
    drawTree(0.0f, 0.0f, 0.0f, 1.0f);
}

// Test: Single rock
void testRock()
{
    camera.setupGLCamera();

    setupEnvironmentLighting();
    drawWorldGround(20.0f);
    DrawingUtils::drawGrid();
    DrawingUtils::drawAxes();

    // Draw a single rock at origin
    drawRock(0.0f, 0.0f, 0.0f, 1.0f);
}

// Test: Grass patch
void testGrass()
{
    camera.setupGLCamera();

    setupEnvironmentLighting();
    drawWorldGround(20.0f);
    DrawingUtils::drawGrid();
    DrawingUtils::drawAxes();

    // Draw a small grass patch
    drawProceduralMeadow(4.0f, 4.0f, 200);
}

// Test: Metal bench
void testBench()
{
    camera.setupGLCamera();

    setupEnvironmentLighting();
    drawWorldGround(20.0f);
    DrawingUtils::drawGrid();
    DrawingUtils::drawAxes();

    // Draw a single metal bench with lamp at origin for detailed inspection
    drawMetalAndLamp(0.0f, 0.0f, 0.0f, 1.0f, 30.0f);
}

// Test: Multiple objects
void testMultipleObjects()
{
    camera.setupGLCamera();

    setupEnvironmentLighting();
    drawWorldGround(20.0f);
    DrawingUtils::drawGrid();
    DrawingUtils::drawAxes();

    // Draw various objects at different positions
    drawTree(-2.0f, 0.0f, -2.0f, 0.8f);
    drawTree(2.0f, 0.0f, 2.0f, 1.2f);
    drawRock(-1.0f, 0.0f, 2.0f, 0.9f);
    drawRock(1.5f, 0.0f, -1.5f, 1.1f);

    // Add a bench with lamp to the multiple objects test too
    drawMetalAndLamp(0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

    // Small grass patches
    drawProceduralMeadow(6.0f, 6.0f, 300);
}

// Test: Axes and grid only
void testAxesGrid()
{
    camera.setupGLCamera();

    DrawingUtils::drawGrid(5.0f, 0.5f);
    DrawingUtils::drawAxes(3.0f);
}

std::vector<std::pair<std::string, std::function<void()>>> tests = {
    {"Axes & Grid", testAxesGrid},
    {"Single Tree", testTree},
    {"Single Rock", testRock},
    {"Grass Patch", testGrass},
    {"Metal Bench", testBench},
    {"Multiple Objects", testMultipleObjects}};

int currentTest = 0;

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    tests[currentTest].second();
    glutSwapBuffers();
}

// Custom keyboard handler for test switching
void customKeyboardHandler(unsigned char key, int x, int y)
{
    if (key >= '1' && key < '1' + tests.size())
    {
        currentTest = key - '1';
        std::cout << "Switched to: " << tests[currentTest].first << std::endl;
        glutPostRedisplay();
    }
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(900, 700);
    glutCreateWindow("Environment Test - Trees, Rocks, Grass, Benches with Axis Grid");
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.5f, 0.8f, 1.0f, 1.0f); // Sky blue background like main.cpp

    // Initialize camera with standard settings
    camera.setZoomLimits(1.0f, 100.0f);

    // Initialize input handler
    inputHandler = new InputHandler(camera);
    inputHandler->setKeyboardCallback(customKeyboardHandler);
    inputHandler->handleCommandLineArgs(argc, argv, (int)tests.size());

    // Set starting test if provided via command line
    currentTest = inputHandler->getStartingScene();

    // Set up GLUT callbacks
    glutDisplayFunc(display);
    inputHandler->setupGLUTCallbacks();

    // Start timer for animations
    inputHandler->startTimer(50);

    std::cout << "Environment Test Program\n";
    std::cout << "========================\n";
    for (size_t i = 0; i < tests.size(); ++i)
        std::cout << (i + 1) << ": " << tests[i].first << std::endl;
    std::cout << "\nControls:\n";
    std::cout << "- Press 1-" << tests.size() << " to switch tests\n";
    std::cout << "- Right-click and drag to rotate camera\n";
    std::cout << "- Press +/- to zoom in/out\n";
    std::cout << "- ESC to exit\n";
    std::cout << "\nAxis Colors: X=Red, Y=Green, Z=Blue\n";
    std::cout << "Grid: 1 unit spacing on XZ plane\n";

    glutMainLoop();

    delete inputHandler;
    return 0;
}