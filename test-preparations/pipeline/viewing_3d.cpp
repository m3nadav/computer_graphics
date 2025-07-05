#include <GLUT/glut.h>

// 3D Viewing Coordinates: Set up camera/view transformation
void setView3D()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    // Camera at (3,2,4), looking at origin, up is +Y
    gluLookAt(3.0, 2.0, 4.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}