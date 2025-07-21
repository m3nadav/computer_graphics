#define GL_SILENCE_DEPRECATION // Silence deprecation warnings on macOS
#include "cow/cow.h"
#include "cow/spots.h"
#include "shapes/shapes.h"
#include <GLUT/glut.h>
#include <cmath>
#include <iostream> // Added for debugging output

// Window dimensions
static int WINDOW_WIDTH = 1200; // Increased window size
static int WINDOW_HEIGHT = 900;

// Camera control variables
static float cameraDistance = 4.0f;
static float cameraAngleX = 45.0f;
static float cameraAngleY = 105.0f;
static bool rightMouseDown = false;
static int lastMouseX = 0;
static int lastMouseY = 0;

// Calculate camera position based on angles
float x = cameraDistance * cos(cameraAngleY * M_PI / 180.0f) * cos(cameraAngleX * M_PI / 180.0f);
float y = cameraDistance * sin(cameraAngleX * M_PI / 180.0f);
float z = cameraDistance * sin(cameraAngleY * M_PI / 180.0f) * cos(cameraAngleX * M_PI / 180.0f);

// Draw a grid for better spatial orientation
void drawGrid(float size, float step)
{
    // Save current OpenGL state
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    // Disable lighting and depth testing for grid
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    // Use a lighter color for better visibility
    glColor3f(0.6f, 0.6f, 0.6f);

    glBegin(GL_LINES);
    for (float i = -size; i <= size; i += step)
    {
        // Draw lines parallel to X axis
        glVertex3f(-size, 0, i);
        glVertex3f(size, 0, i);
        // Draw lines parallel to Z axis
        glVertex3f(i, 0, -size);
        glVertex3f(i, 0, size);
    }
    glEnd();

    // Restore OpenGL state
    glPopAttrib();
}

// Function to set up the camera for each viewport
void setupViewport(int index, int total)
{
    int cols = 2; // We'll use a 2x2 grid
    int rows = (total + 1) / 2;

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

// Set up material properties
void setMaterial(float r, float g, float b)
{
    GLfloat mat_ambient[] = {r * 0.2f, g * 0.2f, b * 0.2f, 1.0f};
    GLfloat mat_diffuse[] = {r, g, b, 1.0f};
    GLfloat mat_specular[] = {0.8f, 0.8f, 0.8f, 1.0f};
    GLfloat mat_shininess[] = {50.0f};

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
}

// Test scene 3: Complete cow
void drawTestCow()
{
    glLoadIdentity();

    gluLookAt(x, y, z, 0, 0, 0, 0, 1, 0);

    drawGrid(3.0f, 0.5f); // Add grid
    drawCow();
}

// Test scene 4: Cow head close-up
void drawTestCowHead()
{
    glLoadIdentity();

    gluLookAt(x, y, z, 0, 0, 0, 0, 1, 0);

    drawGrid(2.0f, 0.5f); // Add grid
    glPushMatrix();
    glRotatef(90, 0, 1, 0); // Rotate to see the head better
    drawCow();
    glPopMatrix();
}

void display()
{
    glClearColor(0.9f, 0.9f, 0.9f, 1.0f); // Light gray background
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Test Scene 1: Complete Cow
    setupViewport(0, 2);
    drawTestCow();

    // Test Scene 2: Cow Head Close-up
    setupViewport(1, 2);
    drawTestCowHead();

    glutSwapBuffers();
}

void reshape(int w, int h)
{
    WINDOW_WIDTH = w;
    WINDOW_HEIGHT = h;
}

void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 27: // ESC key
        exit(0);
        break;
    case '+': // Zoom in
    case '=':
        cameraDistance -= 0.5f;
        if (cameraDistance < 1.0f)
            cameraDistance = 1.0f;
        glutPostRedisplay();
        break;
    case '-': // Zoom out
    case '_':
        cameraDistance += 0.5f;
        if (cameraDistance > 20.0f)
            cameraDistance = 20.0f;
        glutPostRedisplay();
        break;
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
        std::cout << "cameraAngleX: " << cameraAngleX << ", cameraAngleY: " << cameraAngleY << std::endl;

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

void init()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE); // Add normalization for better lighting

    // Set up light
    GLfloat light_position[] = {5.0f, 5.0f, 5.0f, 1.0f}; // Adjusted light position
    GLfloat light_ambient[] = {0.3f, 0.3f, 0.3f, 1.0f};  // Increased ambient light
    GLfloat light_diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat light_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};

    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Cow Components Test - Press ESC to exit, Right-click to rotate camera, Mouse wheel or +/- to zoom");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutMainLoop();

    return 0;
}