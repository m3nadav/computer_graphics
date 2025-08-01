#include <GLUT/glut.h>
#include "scene/scene.h"
#include "cow/cow.h"
#include "environment/environment.h"
#include <cmath>

// Camera zoom control
static float cameraDistance = 20.0f;
static const float MIN_DISTANCE = 5.0f;
static const float MAX_DISTANCE = 100.0f;
static const float ZOOM_STEP = 2.0f;

// Combined keyboard handler for cow movement, head movement, and zoom
void keyboardHandler(unsigned char key, int x, int y)
{
    // Handle zoom controls
    if (key == '+' || key == '=') {
        cameraDistance -= ZOOM_STEP;
        if (cameraDistance < MIN_DISTANCE) {
            cameraDistance = MIN_DISTANCE;
        }
        glutPostRedisplay();
        return;
    }
    if (key == '-' || key == '_') {
        cameraDistance += ZOOM_STEP;
        if (cameraDistance > MAX_DISTANCE) {
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
    
    // Calculate camera position based on distance
    float cameraX = 0.0f;
    float cameraY = 8.0f;
    float cameraZ = cameraDistance;
    
    gluLookAt(cameraX, cameraY, cameraZ, // Eye position with adjustable distance
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
    incrementAnimationFrame();    // Keep animation frame for cow animations (tail, etc.)
    glutPostRedisplay();          // Request redraw
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

    // Initialize cow movement
    initCowMovement();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(50, timer, 0); // Keep timer for cow animations

    // Register keyboard handlers for cow and head movement
    glutKeyboardFunc(keyboardHandler);
    glutSpecialFunc(handleCowSpecialKeys);

    glutMainLoop();
    return 0;
}