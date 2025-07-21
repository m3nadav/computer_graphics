#define GL_SILENCE_DEPRECATION // Silence deprecation warnings on macOS
#include "cow/spots.h"
#include "shapes/shapes.h"
#include <GLUT/glut.h>
#include <iostream>

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Set up camera
    gluLookAt(0, 0, 5, 0, 0, 0, 0, 1, 0);

    // Test simple spot
    std::cout << "Drawing simple spot..." << std::endl;
    drawSpot(0, 0, 0, 0.2, 0.2, 0.2);

    // Test projected spot on ellipsoid
    std::cout << "Drawing projected spot..." << std::endl;
    drawProjectedSpot(1.0, 0.5, 0.3, 0.1, M_PI / 4, M_PI / 4, 16,
                      [](float x, float y, float z, float &wx, float &wy, float &wz)
                      {
                          wx = x;
                          wy = y;
                          wz = z;
                      });

    glutSwapBuffers();
}

void reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, (float)w / h, 0.1, 100);
    glMatrixMode(GL_MODELVIEW);
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

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Spots Test - Press ESC to exit");

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.8, 0.8, 0.8, 1.0);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);

    std::cout << "Spots test initialized. Press ESC to exit." << std::endl;

    glutMainLoop();
    return 0;
}