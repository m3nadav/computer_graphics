#include <GLUT/glut.h>
#include "scene/scene.h"
#include "cow/cow.h"
#include "environment/environment.h"
#include <cmath>

// Camera control variables
static float cameraDistance = 5.0f;
static float cameraAngleX = 35.0f;
static float cameraAngleY = 55.0f;
static bool rightMouseDown = false;
static int lastMouseX = 0;
static int lastMouseY = 0;
static const float MIN_DISTANCE = 1.0f;
static const float MAX_DISTANCE = 100.0f;
static const float ZOOM_STEP = 0.5f;

// Combined keyboard handler for cow movement, head movement, and zoom
void keyboardHandler(unsigned char key, int x, int y)
{
    // Handle ESC key
    if (key == 27)
    {
        exit(0);
    }

    // Handle zoom controls
    if (key == '+' || key == '=')
    {
        cameraDistance -= ZOOM_STEP;
        if (cameraDistance < MIN_DISTANCE)
        {
            cameraDistance = MIN_DISTANCE;
        }
        glutPostRedisplay();
        return;
    }
    if (key == '-' || key == '_')
    {
        cameraDistance += ZOOM_STEP;
        if (cameraDistance > MAX_DISTANCE)
        {
            cameraDistance = MAX_DISTANCE;
        }
        glutPostRedisplay();
        return;
    }

    // Try cow movement first
    handleCowMovement(key, x, y);
    // Then try head movement (this will only redraw if head moved)
    handleHeadMovement(key, x, y);
}

// Custom scene drawing with zoom support
void drawZoomableScene()
{
    glLoadIdentity();

    // Calculate camera position based on angles and distance (orbital camera)
    float cameraX = cameraDistance * cos(cameraAngleY * M_PI / 180.0f) * cos(cameraAngleX * M_PI / 180.0f);
    float cameraY = cameraDistance * sin(cameraAngleX * M_PI / 180.0f);
    float cameraZ = cameraDistance * sin(cameraAngleY * M_PI / 180.0f) * cos(cameraAngleX * M_PI / 180.0f);

    gluLookAt(cameraX, cameraY, cameraZ, // Eye position with orbital camera
              0.0, 0.0, 0.0,             // Look at center
              0.0, 1.0, 0.0);            // Up vector

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

    // Draw the cow in the center
    drawCow();
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawZoomableScene(); // Use custom zoomable scene
    glutSwapBuffers();
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
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (float)w / h, 1.0, 1000.0);
    glMatrixMode(GL_MODELVIEW);
}

void timer(int value)
{
    incrementAnimationFrame();   // Keep animation frame for cow animations (tail, etc.)
    glutPostRedisplay();         // Request redraw
    glutTimerFunc(50, timer, 0); // Schedule next timer call in 50ms
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("OpenGL Cow in a Meadow");
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.5f, 0.8f, 1.0f, 1.0f); // Sky blue

    // Handle command line argument (ignored for main.cpp as it has no test scenes)
    if (argc > 1)
    {
        // Ignore argument - main.cpp has no test scenes
    }

    // Initialize cow movement
    initCowMovement();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutTimerFunc(50, timer, 0); // Keep timer for cow animations

    // Register keyboard handlers for cow and head movement
    glutKeyboardFunc(keyboardHandler);
    glutSpecialFunc(handleCowSpecialKeys);

    glutMainLoop();
    return 0;
}