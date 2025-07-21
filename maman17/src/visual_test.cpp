#define GL_SILENCE_DEPRECATION // Silence deprecation warnings on macOS
#include <GLUT/glut.h>
#include <iostream>
#include <vector>
#include <functional>
#include <cmath>

#include "cow/spots.h"
#include "cow/body.h"
#include "cow/cow.h"

// Camera control variables
static float cameraDistance = 3.0f;
static float cameraAngleX = 45.0f;
static float cameraAngleY = 45.0f;
static bool rightMouseDown = false;
static int lastMouseX = 0;
static int lastMouseY = 0;

// Test 1: Simple GLUT test
void testSimpleGLUT()
{
    std::cout << "[DEBUG] testSimpleGLUT() called" << std::endl;
    std::cout.flush();

    // Calculate camera position based on angles
    float x = cameraDistance * cos(cameraAngleY * M_PI / 180.0f) * cos(cameraAngleX * M_PI / 180.0f);
    float y = cameraDistance * sin(cameraAngleX * M_PI / 180.0f);
    float z = cameraDistance * sin(cameraAngleY * M_PI / 180.0f) * cos(cameraAngleX * M_PI / 180.0f);

    gluLookAt(x, y, z, 0, 0, 0, 0, 1, 0);

    // Just draw a simple colored quad
    glBegin(GL_QUADS);
    glColor3f(1.0, 0.0, 0.0); // Red
    glVertex3f(-0.5, -0.5, 0);
    glVertex3f(0.5, -0.5, 0);
    glVertex3f(0.5, 0.5, 0);
    glVertex3f(-0.5, 0.5, 0);
    glEnd();
}

// Test 2: drawSpot test
void testDrawSpot()
{
    std::cout << "[DEBUG] testDrawSpot() called" << std::endl;
    std::cout.flush();

    // Calculate camera position based on angles
    float x = cameraDistance * cos(cameraAngleY * M_PI / 180.0f) * cos(cameraAngleX * M_PI / 180.0f);
    float y = cameraDistance * sin(cameraAngleX * M_PI / 180.0f);
    float z = cameraDistance * sin(cameraAngleY * M_PI / 180.0f) * cos(cameraAngleX * M_PI / 180.0f);

    gluLookAt(x, y, z, 0, 0, 0, 0, 1, 0);
    drawSpot(0, 0, 0, 0.3, 0.3, 0.3);
}

// Test 3: drawBody test
void testDrawBody()
{
    std::cout << "[DEBUG] testDrawBody() called" << std::endl;
    std::cout.flush();

    // Calculate camera position based on angles
    float x = cameraDistance * cos(cameraAngleY * M_PI / 180.0f) * cos(cameraAngleX * M_PI / 180.0f);
    float y = cameraDistance * sin(cameraAngleX * M_PI / 180.0f);
    float z = cameraDistance * sin(cameraAngleY * M_PI / 180.0f) * cos(cameraAngleX * M_PI / 180.0f);

    gluLookAt(x, y, z, 0, 0, 0, 0, 1, 0);
    drawBody();
}

// Test 4: drawCow test
void testDrawCow()
{
    std::cout << "[DEBUG] testDrawCow() called" << std::endl;
    std::cout.flush();

    // Calculate camera position based on angles
    float x = cameraDistance * cos(cameraAngleY * M_PI / 180.0f) * cos(cameraAngleX * M_PI / 180.0f);
    float y = cameraDistance * sin(cameraAngleX * M_PI / 180.0f);
    float z = cameraDistance * sin(cameraAngleY * M_PI / 180.0f) * cos(cameraAngleX * M_PI / 180.0f);

    gluLookAt(x, y, z, 0, 0, 0, 0, 1, 0);
    drawCow();
}

std::vector<std::pair<std::string, std::function<void()>>> tests = {
    {"Simple GLUT Test", testSimpleGLUT},
    {"Draw Spot Test", testDrawSpot},
    {"Draw Body Test", testDrawBody},
    {"Draw Cow Test", testDrawCow},
};

int currentTest = 0;

void display()
{
    std::cout << "[DEBUG] display() called, currentTest=" << currentTest << std::endl;
    std::cout.flush();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    tests[currentTest].second();
    glutSwapBuffers();
}

void keyboard(unsigned char key, int, int)
{
    std::cout << "[DEBUG] keyboard() called, key=" << (int)key << std::endl;
    std::cout.flush();
    if (key == 27)
        exit(0); // ESC
    if (key >= '1' && key < '1' + tests.size())
    {
        currentTest = key - '1';
        std::cout << "Switched to: " << tests[currentTest].first << std::endl;
        std::cout.flush();
        glutPostRedisplay();
    }
}

void mouse(int button, int state, int x, int y)
{
    if (button == GLUT_RIGHT_BUTTON)
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

void motion(int x, int y)
{
    if (rightMouseDown)
    {
        int deltaX = x - lastMouseX;
        int deltaY = y - lastMouseY;

        // Update camera angles based on mouse movement
        cameraAngleY += deltaX * 0.5f; // Horizontal rotation
        cameraAngleX += deltaY * 0.5f; // Vertical rotation

        // Clamp vertical angle to prevent flipping
        if (cameraAngleX > 89.0f)
            cameraAngleX = 89.0f;
        if (cameraAngleX < -89.0f)
            cameraAngleX = -89.0f;

        lastMouseX = x;
        lastMouseY = y;

        glutPostRedisplay();
    }
}

void reshape(int w, int h)
{
    std::cout << "[DEBUG] reshape() called, w=" << w << ", h=" << h << std::endl;
    std::cout.flush();
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, (float)w / h, 0.1, 100);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char **argv)
{
    std::cout << "[DEBUG] main() starting" << std::endl;
    std::cout.flush();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(900, 700);
    glutCreateWindow("Visual Test Harness - New");
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.8, 0.8, 0.8, 1.0);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);

    std::cout << "Visual Test Harness - New Version\n";
    for (size_t i = 0; i < tests.size(); ++i)
        std::cout << (i + 1) << ": " << tests[i].first << std::endl;
    std::cout << "Press number key to switch test, ESC to exit.\n";
    std::cout << "Right-click and drag to rotate camera.\n";
    std::cout.flush();

    glutMainLoop();
    return 0;
}