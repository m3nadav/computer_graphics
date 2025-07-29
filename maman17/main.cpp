#include <GLUT/glut.h>
#include "scene/scene.h"

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
    glutTimerFunc(500, timer, 0); // Schedule next timer call in 500ms (0.5 seconds)
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("OpenGL Cow in a Meadow");
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.5f, 0.8f, 1.0f, 1.0f); // Sky blue

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(500, timer, 0); // Start timer for 0.5 second intervals

    glutMainLoop();
    return 0;
}