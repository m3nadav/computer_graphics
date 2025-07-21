#define GL_SILENCE_DEPRECATION // Silence deprecation warnings on macOS
#include "shapes/shapes.h"
#include <GLUT/glut.h>
#include <cmath>

// Window dimensions
static int WINDOW_WIDTH = 1200;
static int WINDOW_HEIGHT = 900;

// Draw a grid for better spatial orientation
void drawGrid(float size, float step)
{
    glDisable(GL_LIGHTING);
    glBegin(GL_LINES);
    glColor3f(0.3f, 0.3f, 0.3f);
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
    glEnable(GL_LIGHTING);
}

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

// Test scene 1: Basic sphere
void drawTestSphere()
{
    glLoadIdentity();
    gluLookAt(2, 2, 2, 0, 0, 0, 0, 1, 0);

    drawGrid(2.0f, 0.5f);

    // Draw coordinate axes
    glDisable(GL_LIGHTING);
    glBegin(GL_LINES);
    glColor3f(1, 0, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(2, 0, 0); // X axis
    glColor3f(0, 1, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 2, 0); // Y axis
    glColor3f(0, 0, 1);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, 2); // Z axis
    glEnd();
    glEnable(GL_LIGHTING);

    setMaterial(0.8f, 0.2f, 0.2f); // Red
    drawSphere(0.5);
}

// Test scene 2: Ellipsoid (spherical)
void drawTestEllipsoidSpherical()
{
    glLoadIdentity();
    gluLookAt(2, 2, 2, 0, 0, 0, 0, 1, 0);

    drawGrid(2.0f, 0.5f);

    setMaterial(0.2f, 0.8f, 0.2f); // Green
    drawEllipsoid(0.5, 0.5, 0.5);  // Sphere-like
}

// Test scene 3: Ellipsoid (elongated)
void drawTestEllipsoidElongated()
{
    glLoadIdentity();
    gluLookAt(2, 2, 2, 0, 0, 0, 0, 1, 0);

    drawGrid(2.0f, 0.5f);

    setMaterial(0.2f, 0.2f, 0.8f); // Blue
    drawEllipsoid(1.0, 0.3, 0.3);  // Elongated in X direction
}

// Test scene 4: Ellipsoid (flattened)
void drawTestEllipsoidFlattened()
{
    glLoadIdentity();
    gluLookAt(2, 2, 2, 0, 0, 0, 0, 1, 0);

    drawGrid(2.0f, 0.5f);

    setMaterial(0.8f, 0.8f, 0.2f); // Yellow
    drawEllipsoid(0.5, 0.5, 0.2);  // Flattened in Z direction
}

// Test scene 5: Cylinder (standard)
void drawTestCylinderStandard()
{
    glLoadIdentity();
    gluLookAt(2, 2, 2, 0, 0, 0, 0, 1, 0);

    drawGrid(2.0f, 0.5f);

    setMaterial(0.8f, 0.2f, 0.8f); // Magenta
    drawCylinder(0.3, 0.3, 1.0);   // Standard cylinder
}

// Test scene 6: Cylinder (cone-like)
void drawTestCylinderCone()
{
    glLoadIdentity();
    gluLookAt(2, 2, 2, 0, 0, 0, 0, 1, 0);

    drawGrid(2.0f, 0.5f);

    setMaterial(0.2f, 0.8f, 0.8f); // Cyan
    drawCylinder(0.5, 0.1, 1.0);   // Cone-like
}

// Test scene 7: Cylinder (wide)
void drawTestCylinderWide()
{
    glLoadIdentity();
    gluLookAt(2, 2, 2, 0, 0, 0, 0, 1, 0);

    drawGrid(2.0f, 0.5f);

    setMaterial(0.8f, 0.4f, 0.2f); // Orange
    drawCylinder(0.8, 0.8, 0.5);   // Wide and short
}

// Test scene 8: Rotated shapes
void drawTestRotatedShapes()
{
    glLoadIdentity();
    gluLookAt(2, 2, 2, 0, 0, 0, 0, 1, 0);

    drawGrid(2.0f, 0.5f);

    // Rotated ellipsoid
    glPushMatrix();
    glRotatef(45, 0, 1, 0);
    setMaterial(0.6f, 0.3f, 0.9f); // Purple
    drawEllipsoid(0.4, 0.8, 0.4);
    glPopMatrix();

    // Rotated cylinder
    glPushMatrix();
    glTranslatef(0.8f, 0, 0);
    glRotatef(90, 0, 0, 1);
    setMaterial(0.3f, 0.9f, 0.6f); // Light green
    drawCylinder(0.2, 0.2, 0.6);
    glPopMatrix();
}

// Test scene 9: Scaled shapes
void drawTestScaledShapes()
{
    glLoadIdentity();
    gluLookAt(2, 2, 2, 0, 0, 0, 0, 1, 0);

    drawGrid(2.0f, 0.5f);

    // Scaled sphere
    glPushMatrix();
    glTranslatef(-0.5f, 0, 0);
    glScalef(0.5f, 1.0f, 1.0f);
    setMaterial(0.9f, 0.6f, 0.3f); // Light orange
    drawSphere(0.4);
    glPopMatrix();

    // Scaled cylinder
    glPushMatrix();
    glTranslatef(0.5f, 0, 0);
    glScalef(1.0f, 0.5f, 1.0f);
    setMaterial(0.3f, 0.6f, 0.9f); // Light blue
    drawCylinder(0.3, 0.3, 0.8);
    glPopMatrix();
}

void display()
{
    glClearColor(0.9f, 0.9f, 0.9f, 1.0f); // Light gray background
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Test Scene 1: Basic Sphere
    setupViewport(0, 9);
    drawTestSphere();

    // Test Scene 2: Ellipsoid (spherical)
    setupViewport(1, 9);
    drawTestEllipsoidSpherical();

    // Test Scene 3: Ellipsoid (elongated)
    setupViewport(2, 9);
    drawTestEllipsoidElongated();

    // Test Scene 4: Ellipsoid (flattened)
    setupViewport(3, 9);
    drawTestEllipsoidFlattened();

    // Test Scene 5: Cylinder (standard)
    setupViewport(4, 9);
    drawTestCylinderStandard();

    // Test Scene 6: Cylinder (cone-like)
    setupViewport(5, 9);
    drawTestCylinderCone();

    // Test Scene 7: Cylinder (wide)
    setupViewport(6, 9);
    drawTestCylinderWide();

    // Test Scene 8: Rotated shapes
    setupViewport(7, 9);
    drawTestRotatedShapes();

    // Test Scene 9: Scaled shapes
    setupViewport(8, 9);
    drawTestScaledShapes();

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
    }
}

void init()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);

    // Set up light
    GLfloat light_position[] = {5.0f, 5.0f, 5.0f, 1.0f};
    GLfloat light_ambient[] = {0.3f, 0.3f, 0.3f, 1.0f};
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
    glutCreateWindow("Shapes Test - Basic Geometric Primitives - Press ESC to exit");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMainLoop();

    return 0;
}