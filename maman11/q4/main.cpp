#define GL_SILENCE_DEPRECATION

#include <GLUT/glut.h>
#include <string>
#include "scene.h"
#include "utils.h"

// Default window dimensions
int windowWidth = 600;
int windowHeight = 300;

void init()
{
    glClearColor(0.9f, 0.9f, 0.9f, 1.0f); // Light gray background
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, windowWidth, 0, windowHeight);
    glMatrixMode(GL_MODELVIEW);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw the scene with current window dimensions
    drawScene(windowWidth, windowHeight);

    glutSwapBuffers();
}

void reshape(int w, int h)
{
    windowWidth = w;
    windowHeight = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);
    glMatrixMode(GL_MODELVIEW);
    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y)
{
    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
    {
        // Convert y coordinate to OpenGL coordinate system
        y = windowHeight - y;

        // Check if click is within exit button boundaries
        if (x >= getExitButtonX(windowWidth, EXIT_BUTTON_WIDTH) && x <= getExitButtonX(windowWidth, EXIT_BUTTON_WIDTH) + EXIT_BUTTON_WIDTH &&
            y >= EXIT_BUTTON_Y && y <= EXIT_BUTTON_Y + EXIT_BUTTON_HEIGHT)
        {
            exit(0);
        }
    }
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Children Playing Scene");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);

    glutMainLoop();
    return 0;
}