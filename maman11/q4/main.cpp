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
    glClearColor(0.9f, 1.0f, 0.9f, 1.0f); // Light green background
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
}

void mouse(int button, int state, int x, int y)
{
    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
    {
        // Convert y coordinate to OpenGL coordinate system
        y = windowHeight - y;

        // Get the exit button boundaries
        const int minX = getExitButtonX(windowWidth, EXIT_BUTTON_WIDTH);
        const int minY = EXIT_BUTTON_Y;
        const int maxX = minX + EXIT_BUTTON_WIDTH;
        const int maxY = minY + EXIT_BUTTON_HEIGHT;

        // Check if click is within exit button boundaries
        if (x >= minX && x <= maxX && y >= minY && y <= maxY)
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