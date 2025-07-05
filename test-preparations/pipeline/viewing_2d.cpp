#include <GLUT/glut.h>

// 2D Viewing Coordinates: Set up camera/view transformation
void setView2D()
{
    // For 2D, we use a simple orthographic view
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    // No camera movement needed for 2D
}