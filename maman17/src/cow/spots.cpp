#define GL_SILENCE_DEPRECATION // Silence deprecation warnings on macOS
#include "cow/spots.h"
#include <GLUT/glut.h>

// Helper function to draw a spot
void drawSpot(float x, float y, float z, float rx, float ry, float rz)
{
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(rx, ry, rz);
    glColor3f(0.1f, 0.1f, 0.1f); // Black
    glutSolidSphere(1.0, 16, 16);
    glPopMatrix();
}