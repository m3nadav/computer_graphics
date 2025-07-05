#include <GLUT/glut.h>

// 3D Projection Coordinates: Set up perspective projection
void setProjection3D(int w, int h)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float aspect = (float)w / (float)h;
    gluPerspective(60.0, aspect, 0.1, 20.0);
    glMatrixMode(GL_MODELVIEW);
}