#define GL_SILENCE_DEPRECATION // Silence deprecation warnings on macOS
#include <GLUT/glut.h>
#include <iostream>
#include <vector>
#include <functional>
#include <cmath>

#include "environment/environment.h"
#include "shapes/shapes.h"

// Camera control variables
static float cameraDistance = 5.0f;
static float cameraAngleX = 35.0f;
static float cameraAngleY = 55.0f;
static bool rightMouseDown = false;
static int lastMouseX = 0;
static int lastMouseY = 0;

// Draw coordinate axes (X=red, Y=green, Z=blue)
void drawAxes(float length = 2.0f)
{
    glDisable(GL_LIGHTING);
    glLineWidth(2.0f);

    glBegin(GL_LINES);

    // X-axis (Red)
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(length, 0.0f, 0.0f);

    // Y-axis (Green)
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, length, 0.0f);

    // Z-axis (Blue)
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, length);

    glEnd();

    glLineWidth(1.0f);
    glEnable(GL_LIGHTING);
}

// Draw grid on XZ plane
void drawGrid(float size = 10.0f, float step = 1.0f)
{
    glDisable(GL_LIGHTING);
    glColor3f(0.3f, 0.3f, 0.3f);
    glLineWidth(1.0f);

    glBegin(GL_LINES);

    // Grid lines parallel to X axis
    for (float z = -size; z <= size; z += step)
    {
        glVertex3f(-size, 0.0f, z);
        glVertex3f(size, 0.0f, z);
    }

    // Grid lines parallel to Z axis
    for (float x = -size; x <= size; x += step)
    {
        glVertex3f(x, 0.0f, -size);
        glVertex3f(x, 0.0f, size);
    }

    glEnd();

    glEnable(GL_LIGHTING);
}

std::tuple<float, float, float> calculateCoordinates()
{
    float x = cameraDistance * cos(cameraAngleY * M_PI / 180.0f) * cos(cameraAngleX * M_PI / 180.0f);
    float y = cameraDistance * sin(cameraAngleX * M_PI / 180.0f);
    float z = cameraDistance * sin(cameraAngleY * M_PI / 180.0f) * cos(cameraAngleX * M_PI / 180.0f);

    return {x, y, z};
}

// Test: Single tree
void testTree()
{
    auto coords = calculateCoordinates();
    float x = std::get<0>(coords);
    float y = std::get<1>(coords);
    float z = std::get<2>(coords);

    gluLookAt(x, y, z, 0, 0, 0, 0, 1, 0);

    setupEnvironmentLighting();
    drawWorldGround(20.0f);
    drawGrid();
    drawAxes();

    // Draw a single tree at origin
    drawTree(0.0f, 0.0f, 0.0f, 1.0f);
}

// Test: Single rock
void testRock()
{
    auto coords = calculateCoordinates();
    float x = std::get<0>(coords);
    float y = std::get<1>(coords);
    float z = std::get<2>(coords);

    gluLookAt(x, y, z, 0, 0, 0, 0, 1, 0);

    setupEnvironmentLighting();
    drawWorldGround(20.0f);
    drawGrid();
    drawAxes();

    // Draw a single rock at origin
    drawRock(0.0f, 0.0f, 0.0f, 1.0f);
}

// Test: Grass patch
void testGrass()
{
    auto coords = calculateCoordinates();
    float x = std::get<0>(coords);
    float y = std::get<1>(coords);
    float z = std::get<2>(coords);

    gluLookAt(x, y, z, 0, 0, 0, 0, 1, 0);

    setupEnvironmentLighting();
    drawWorldGround(20.0f);
    drawGrid();
    drawAxes();

    // Draw a small grass patch
    drawProceduralMeadow(4.0f, 4.0f, 200);
}

// Test: Multiple objects
void testMultipleObjects()
{
    auto coords = calculateCoordinates();
    float x = std::get<0>(coords);
    float y = std::get<1>(coords);
    float z = std::get<2>(coords);

    gluLookAt(x, y, z, 0, 0, 0, 0, 1, 0);

    setupEnvironmentLighting();
    drawWorldGround(20.0f);
    drawGrid();
    drawAxes();

    // Draw various objects at different positions
    drawTree(-2.0f, 0.0f, -2.0f, 0.8f);
    drawTree(2.0f, 0.0f, 2.0f, 1.2f);
    drawRock(-1.0f, 0.0f, 2.0f, 0.9f);
    drawRock(1.5f, 0.0f, -1.5f, 1.1f);

    // Small grass patches
    drawProceduralMeadow(6.0f, 6.0f, 300);
}

// Test: Axes and grid only
void testAxesGrid()
{
    auto coords = calculateCoordinates();
    float x = std::get<0>(coords);
    float y = std::get<1>(coords);
    float z = std::get<2>(coords);

    gluLookAt(x, y, z, 0, 0, 0, 0, 1, 0);

    drawGrid(5.0f, 0.5f);
    drawAxes(3.0f);
}

std::vector<std::pair<std::string, std::function<void()>>> tests = {
    {"Axes & Grid", testAxesGrid},
    {"Single Tree", testTree},
    {"Single Rock", testRock},
    {"Grass Patch", testGrass},
    {"Multiple Objects", testMultipleObjects}};

int currentTest = 0;

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    tests[currentTest].second();
    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y)
{
    if (key == 27) // ESC
        exit(0);
    if (key >= '1' && key < '1' + tests.size())
    {
        currentTest = key - '1';
        std::cout << "Switched to: " << tests[currentTest].first << std::endl;
        glutPostRedisplay();
    }
    else if (key == '+' || key == '=') // Zoom in
    {
        cameraDistance -= 0.5f;
        if (cameraDistance < 1.0f)
            cameraDistance = 1.0f;
        glutPostRedisplay();
    }
    else if (key == '-' || key == '_') // Zoom out
    {
        cameraDistance += 0.5f;
        if (cameraDistance > 100.0f)
            cameraDistance = 100.0f;
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
        if (cameraAngleX < 10.0f)
            cameraAngleX = 10.0f;

        lastMouseX = x;
        lastMouseY = y;

        glutPostRedisplay();
    }
}

void timer(int value)
{
    glutPostRedisplay();         // Request redraw
    glutTimerFunc(50, timer, 0); // Schedule next timer call in 50ms
}

void reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, (float)w / h, 0.1, 100);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(900, 700);
    glutCreateWindow("Environment Test - Trees, Rocks, Grass with Axis Grid");
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.5f, 0.8f, 1.0f, 1.0f); // Sky blue background

    // Handle command line argument for starting test
    if (argc > 1)
    {
        int testArg = atoi(argv[1]);
        if (testArg >= 1 && testArg <= (int)tests.size())
        {
            currentTest = testArg - 1; // Convert to 0-based index
        }
    }

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutTimerFunc(50, timer, 0); // Start timer for animations

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
    return 0;
}