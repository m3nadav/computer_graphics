#include "scene/scene.h"
#include "cow/cow.h"
#include <GLUT/glut.h>

void drawMeadow()
{
    glColor3f(0.2f, 0.8f, 0.2f); // Green
    glPushMatrix();
    glTranslatef(0.0f, -1.0f, 0.0f);
    glScalef(50.0f, 0.1f, 50.0f); // Large flat ground
    glutSolidCube(1.0);
    glPopMatrix();
}

void drawScene()
{
    glLoadIdentity();
    gluLookAt(0.0, 5.0, 15.0, // Eye position
              0.0, 0.0, 0.0,  // Look at
              0.0, 1.0, 0.0); // Up
    drawMeadow();
    drawCow();
}