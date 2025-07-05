#include <GLUT/glut.h>

// 2D Projection Coordinates: Set up orthographic projection
void setProjection2D(int w, int h)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float aspect = (float)w / (float)h;
    // Keep coordinates in [-1.5, 1.5] horizontally, adjust vertically for aspect
    if (aspect >= 1.0f)
        gluOrtho2D(-1.5f * aspect, 1.5f * aspect, -1.5f, 1.5f);
    else
        gluOrtho2D(-1.5f, 1.5f, -1.5f / aspect, 1.5f / aspect);
    glMatrixMode(GL_MODELVIEW);
}