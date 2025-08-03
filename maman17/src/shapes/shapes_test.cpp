#include "shapes/shapes.h"
#include <GLUT/glut.h>
#include <cmath>
#include <tuple>
#include <string>
#include <utility>

#include "common/camera.h"
#include "common/input.h"
#include "common/drawing.h"

// Window dimensions
static int WINDOW_WIDTH = 1200;
static int WINDOW_HEIGHT = 900;

// Global camera and input handler
static CameraController camera;
static InputHandler *inputHandler = nullptr;
static int currentScene = 0; // 0, 1, or 2 for the three scene groups

// Using common drawing and camera utilities

// Function to set up the camera for each viewport
void setupViewport(int index, int total)
{
    int cols = 3; // We'll use a 3x3 grid for more shapes
    int rows = (total + 2) / 3;

    int width = WINDOW_WIDTH / cols;
    int height = WINDOW_HEIGHT / rows;

    int row = index / cols;
    int col = index % cols;

    glViewport(col * width, (rows - 1 - row) * height, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (float)width / height, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// Using common drawing utilities for setMaterial and renderText

// Test scene 1: Basic sphere
void drawTestSphere()
{
    glLoadIdentity();
    camera.setupGLCamera();

    DrawingUtils::drawGrid(2.0f, 0.5f);
    DrawingUtils::drawAxes(2.0f);

    // Render title
    DrawingUtils::renderText("Basic Sphere", -0.8f, 1.5f, 0.0f);

    DrawingUtils::setMaterial(0.8f, 0.2f, 0.2f); // Red
    drawSphere(0.5);
}

// Test scene 2: Ellipsoid (spherical)
void drawTestEllipsoidSpherical()
{
    glLoadIdentity();
    camera.setupGLCamera();

    DrawingUtils::drawGrid(2.0f, 0.5f);

    // Render title
    DrawingUtils::renderText("Spherical Ellipsoid", -1.0f, 1.5f, 0.0f);

    DrawingUtils::setMaterial(0.2f, 0.8f, 0.2f); // Green
    drawEllipsoid(0.5, 0.5, 0.5);                // Sphere-like
}

// Test scene 3: Ellipsoid (elongated)
void drawTestEllipsoidElongated()
{
    glLoadIdentity();
    camera.setupGLCamera();

    DrawingUtils::drawGrid(2.0f, 0.5f);

    // Render title
    DrawingUtils::renderText("Elongated Ellipsoid", -1.0f, 1.5f, 0.0f);

    DrawingUtils::setMaterial(0.2f, 0.2f, 0.8f); // Blue
    drawEllipsoid(1.0, 0.3, 0.3);                // Elongated in X direction
}

// Test scene 4: Ellipsoid (flattened)
void drawTestEllipsoidFlattened()
{
    glLoadIdentity();
    camera.setupGLCamera();

    DrawingUtils::drawGrid(2.0f, 0.5f);

    // Render title
    DrawingUtils::renderText("Flattened Ellipsoid", -1.0f, 1.5f, 0.0f);

    DrawingUtils::setMaterial(0.8f, 0.8f, 0.2f); // Yellow
    drawEllipsoid(0.5, 0.5, 0.2);                // Flattened in Z direction
}

// Test scene 5: Cylinder (standard)
void drawTestCylinderStandard()
{
    glLoadIdentity();
    camera.setupGLCamera();

    DrawingUtils::drawGrid(2.0f, 0.5f);

    // Render title
    DrawingUtils::renderText("Standard Cylinder", -1.0f, 1.5f, 0.0f);

    DrawingUtils::setMaterial(0.8f, 0.2f, 0.8f); // Magenta
    drawCylinder(0.3, 0.3, 1.0);                 // Standard cylinder
}

// Test scene 6: Cylinder (cone-like)
void drawTestCylinderCone()
{
    glLoadIdentity();
    camera.setupGLCamera();

    DrawingUtils::drawGrid(2.0f, 0.5f);

    // Render title
    DrawingUtils::renderText("Cone-like Cylinder", -1.0f, 1.5f, 0.0f);

    DrawingUtils::setMaterial(0.2f, 0.8f, 0.8f); // Cyan
    drawCylinder(0.5, 0.1, 1.0);                 // Cone-like
}

// Test scene 7: Cylinder (wide)
void drawTestCylinderWide()
{
    glLoadIdentity();
    camera.setupGLCamera();

    DrawingUtils::drawGrid(2.0f, 0.5f);

    // Render title
    DrawingUtils::renderText("Wide Cylinder", -0.8f, 1.5f, 0.0f);

    DrawingUtils::setMaterial(0.8f, 0.4f, 0.2f); // Orange
    drawCylinder(0.8, 0.8, 0.5);                 // Wide and short
}

// Test scene 8: Rotated shapes
void drawTestRotatedShapes()
{
    glLoadIdentity();
    camera.setupGLCamera();

    DrawingUtils::drawGrid(2.0f, 0.5f);

    // Render title
    DrawingUtils::renderText("Rotated Shapes", -0.8f, 1.5f, 0.0f);

    // Rotated ellipsoid
    glPushMatrix();
    glRotatef(45, 0, 1, 0);
    DrawingUtils::setMaterial(0.6f, 0.3f, 0.9f); // Purple
    drawEllipsoid(0.4, 0.8, 0.4);
    glPopMatrix();

    // Rotated cylinder
    glPushMatrix();
    glTranslatef(0.8f, 0, 0);
    glRotatef(90, 0, 0, 1);
    DrawingUtils::setMaterial(0.3f, 0.9f, 0.6f); // Light green
    drawCylinder(0.2, 0.2, 0.6);
    glPopMatrix();
}

// Test scene 9: Scaled shapes
void drawTestScaledShapes()
{
    glLoadIdentity();
    camera.setupGLCamera();

    DrawingUtils::drawGrid(2.0f, 0.5f);

    // Render title
    DrawingUtils::renderText("Scaled Shapes", -0.8f, 1.5f, 0.0f);

    // Scaled sphere
    glPushMatrix();
    glTranslatef(-0.5f, 0, 0);
    glScalef(0.5f, 1.0f, 1.0f);
    // DrawingUtils::setMaterial(0.9f, 0.6f, 0.3f); // Light orange
    drawSphere(0.4);
    glPopMatrix();

    // Scaled cylinder
    glPushMatrix();
    glTranslatef(0.5f, 0, 0);
    glScalef(1.0f, 0.5f, 1.0f);
    // DrawingUtils::setMaterial(0.3f, 0.6f, 0.9f); // Light blue
    drawCylinder(0.3, 0.3, 0.8);
    glPopMatrix();
}

void display()
{
    glClearColor(0.9f, 0.9f, 0.9f, 1.0f); // Light gray background
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Show 3 shapes per scene based on currentScene
    // Scene 0: Shapes 0-2 (Basic Sphere, Ellipsoid Spherical, Ellipsoid Elongated)
    // Scene 1: Shapes 3-5 (Ellipsoid Flattened, Cylinder Standard, Cylinder Cone)
    // Scene 2: Shapes 6-8 (Cylinder Wide, Rotated Shapes, Scaled Shapes)

    if (currentScene == 0)
    {
        setupViewport(0, 3);
        drawTestSphere();
        setupViewport(1, 3);
        drawTestEllipsoidSpherical();
        setupViewport(2, 3);
        drawTestEllipsoidElongated();
    }
    else if (currentScene == 1)
    {
        setupViewport(0, 3);
        drawTestEllipsoidFlattened();
        setupViewport(1, 3);
        drawTestCylinderStandard();
        setupViewport(2, 3);
        drawTestCylinderCone();
    }
    else if (currentScene == 2)
    {
        setupViewport(0, 3);
        drawTestCylinderWide();
        setupViewport(1, 3);
        drawTestRotatedShapes();
        setupViewport(2, 3);
        drawTestScaledShapes();
    }

    glutSwapBuffers();
}

// Custom reshape function for viewport handling
void customReshapeCallback(int w, int h)
{
    WINDOW_WIDTH = w;
    WINDOW_HEIGHT = h;
}

// Custom keyboard handler for scene switching
void customKeyboardHandler(unsigned char key, int x, int y)
{
    switch (key)
    {
    case '1': // Scene 1
        currentScene = 0;
        glutPostRedisplay();
        break;
    case '2': // Scene 2
        currentScene = 1;
        glutPostRedisplay();
        break;
    case '3': // Scene 3
        currentScene = 2;
        glutPostRedisplay();
        break;
    }
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Shapes Test - Right-click to rotate camera, +/- to zoom - Press ESC to exit");

    // Initialize OpenGL
    DrawingUtils::initOpenGL();
    DrawingUtils::setupLighting();

    // Initialize camera with standard settings
    camera.setZoomLimits(1.0f, 100.0f);

    // Initialize input handler
    inputHandler = new InputHandler(camera);
    inputHandler->setKeyboardCallback(customKeyboardHandler);
    inputHandler->handleCommandLineArgs(argc, argv, 3); // 3 scenes available

    // Set starting scene if provided via command line
    currentScene = inputHandler->getStartingScene();

    // Set up GLUT callbacks
    glutDisplayFunc(display);
    inputHandler->setupGLUTCallbacks();

    // Override reshape callback for custom window handling
    glutReshapeFunc(customReshapeCallback);

    // Start timer for animations
    inputHandler->startTimer(50);

    glutMainLoop();

    delete inputHandler;
    return 0;
}