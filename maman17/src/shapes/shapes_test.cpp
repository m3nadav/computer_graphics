#define GL_SILENCE_DEPRECATION // Silence deprecation warnings on macOS
#include "shapes/shapes.h"
#include <GLUT/glut.h>
#include <cmath>
#include <tuple>
#include <string>
#include <utility>

// Window dimensions
static int WINDOW_WIDTH = 1200;
static int WINDOW_HEIGHT = 900;

// Camera control variables
static float cameraDistance = 5.0f;
static float cameraAngleX = 35.0f;
static float cameraAngleY = 55.0f;
static bool rightMouseDown = false;
static int lastMouseX = 0;
static int lastMouseY = 0;
static int currentScene = 0; // 0, 1, or 2 for the three scene groups

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

// Calculate camera position based on angles
std::tuple<float, float, float> calculateCoordinates()
{
    float x = cameraDistance * cos(cameraAngleY * M_PI / 180.0f) * cos(cameraAngleX * M_PI / 180.0f);
    float y = cameraDistance * sin(cameraAngleX * M_PI / 180.0f);
    float z = cameraDistance * sin(cameraAngleY * M_PI / 180.0f) * cos(cameraAngleX * M_PI / 180.0f);

    return {x, y, z};
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

// Render text in 3D space
void renderText(const std::string &text, float x, float y, float z)
{
    glDisable(GL_LIGHTING);
    glColor3f(0.0f, 0.0f, 0.0f); // Black text

    glRasterPos3f(x, y, z);
    for (char c : text)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
    }

    glEnable(GL_LIGHTING);
}

// Test scene 1: Basic sphere
void drawTestSphere()
{
    glLoadIdentity();
    auto [x, y, z] = calculateCoordinates();
    gluLookAt(x, y, z, 0, 0, 0, 0, 1, 0);

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

    // Render title
    renderText("Basic Sphere", -0.8f, 1.5f, 0.0f);

    setMaterial(0.8f, 0.2f, 0.2f); // Red
    drawSphere(0.5);
}

// Test scene 2: Ellipsoid (spherical)
void drawTestEllipsoidSpherical()
{
    glLoadIdentity();
    auto [x, y, z] = calculateCoordinates();
    gluLookAt(x, y, z, 0, 0, 0, 0, 1, 0);

    drawGrid(2.0f, 0.5f);

    // Render title
    renderText("Spherical Ellipsoid", -1.0f, 1.5f, 0.0f);

    setMaterial(0.2f, 0.8f, 0.2f); // Green
    drawEllipsoid(0.5, 0.5, 0.5);  // Sphere-like
}

// Test scene 3: Ellipsoid (elongated)
void drawTestEllipsoidElongated()
{
    glLoadIdentity();
    auto [x, y, z] = calculateCoordinates();
    gluLookAt(x, y, z, 0, 0, 0, 0, 1, 0);

    drawGrid(2.0f, 0.5f);

    // Render title
    renderText("Elongated Ellipsoid", -1.0f, 1.5f, 0.0f);

    setMaterial(0.2f, 0.2f, 0.8f); // Blue
    drawEllipsoid(1.0, 0.3, 0.3);  // Elongated in X direction
}

// Test scene 4: Ellipsoid (flattened)
void drawTestEllipsoidFlattened()
{
    glLoadIdentity();
    auto [x, y, z] = calculateCoordinates();
    gluLookAt(x, y, z, 0, 0, 0, 0, 1, 0);

    drawGrid(2.0f, 0.5f);

    // Render title
    renderText("Flattened Ellipsoid", -1.0f, 1.5f, 0.0f);

    setMaterial(0.8f, 0.8f, 0.2f); // Yellow
    drawEllipsoid(0.5, 0.5, 0.2);  // Flattened in Z direction
}

// Test scene 5: Cylinder (standard)
void drawTestCylinderStandard()
{
    glLoadIdentity();
    auto [x, y, z] = calculateCoordinates();
    gluLookAt(x, y, z, 0, 0, 0, 0, 1, 0);

    drawGrid(2.0f, 0.5f);

    // Render title
    renderText("Standard Cylinder", -1.0f, 1.5f, 0.0f);

    setMaterial(0.8f, 0.2f, 0.8f); // Magenta
    drawCylinder(0.3, 0.3, 1.0);   // Standard cylinder
}

// Test scene 6: Cylinder (cone-like)
void drawTestCylinderCone()
{
    glLoadIdentity();
    auto [x, y, z] = calculateCoordinates();
    gluLookAt(x, y, z, 0, 0, 0, 0, 1, 0);

    drawGrid(2.0f, 0.5f);

    // Render title
    renderText("Cone-like Cylinder", -1.0f, 1.5f, 0.0f);

    setMaterial(0.2f, 0.8f, 0.8f); // Cyan
    drawCylinder(0.5, 0.1, 1.0);   // Cone-like
}

// Test scene 7: Cylinder (wide)
void drawTestCylinderWide()
{
    glLoadIdentity();
    auto [x, y, z] = calculateCoordinates();
    gluLookAt(x, y, z, 0, 0, 0, 0, 1, 0);

    drawGrid(2.0f, 0.5f);

    // Render title
    renderText("Wide Cylinder", -0.8f, 1.5f, 0.0f);

    setMaterial(0.8f, 0.4f, 0.2f); // Orange
    drawCylinder(0.8, 0.8, 0.5);   // Wide and short
}

// Test scene 8: Rotated shapes
void drawTestRotatedShapes()
{
    glLoadIdentity();
    auto [x, y, z] = calculateCoordinates();
    gluLookAt(x, y, z, 0, 0, 0, 0, 1, 0);

    drawGrid(2.0f, 0.5f);

    // Render title
    renderText("Rotated Shapes", -0.8f, 1.5f, 0.0f);

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
    auto [x, y, z] = calculateCoordinates();
    gluLookAt(x, y, z, 0, 0, 0, 0, 1, 0);

    drawGrid(2.0f, 0.5f);

    // Render title
    renderText("Scaled Shapes", -0.8f, 1.5f, 0.0f);

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

void timer(int value)
{
    glutPostRedisplay();         // Request redraw
    glutTimerFunc(50, timer, 0); // Schedule next timer call in 50ms
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
        if (cameraDistance > 100.0f)
            cameraDistance = 100.0f;
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
    glutCreateWindow("Shapes Test - Right-click to rotate camera, +/- to zoom - Press ESC to exit");

    // Handle command line argument for starting scene
    if (argc > 1)
    {
        int sceneArg = atoi(argv[1]);
        if (sceneArg >= 1 && sceneArg <= 3)
        {
            currentScene = sceneArg - 1; // Convert to 0-based index
        }
    }

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutTimerFunc(50, timer, 0); // Start timer for animations
    glutMainLoop();

    return 0;
}