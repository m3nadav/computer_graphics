#include <GLUT/glut.h>
#include "scene/scene.h"
#include "cow/cow.h"

// Combined keyboard handler for both cow movement and head movement
void keyboardHandler(unsigned char key, int x, int y)
{
    // Try cow movement first
    handleCowMovement(key, x, y);
    // Then try head movement (this will only redraw if head moved)
    handleHeadMovement(key, x, y);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawEnhancedScene(); // Use the enhanced scene with trees, grass, and rocks
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
    incrementAnimationFrame();    // Increment animation frame
    glutPostRedisplay();          // Request redraw
    glutTimerFunc(50, timer, 0); // Schedule next timer call in 50ms (0.5 seconds)
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
    glutTimerFunc(50, timer, 0); // Start timer for 0.5 second intervals

    // Register keyboard handlers for cow and head movement
    glutKeyboardFunc(keyboardHandler);
    glutSpecialFunc(handleCowSpecialKeys);

    glutMainLoop();
    return 0;
}