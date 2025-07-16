#include "cow/cow.h"
#include "shapes/shapes.h"
#include <GLUT/glut.h>

void drawCow()
{
    glPushMatrix();
    // Body
    glColor3f(0.9f, 0.9f, 0.8f);
    glPushMatrix();
    glScalef(2.0f, 1.0f, 1.0f);
    drawEllipsoid(1.0, 0.6, 0.5);
    glPopMatrix();

    // Head
    glPushMatrix();
    glTranslatef(1.2f, 0.3f, 0.0f);
    drawEllipsoid(0.4, 0.3, 0.3);
    glPopMatrix();

    // Legs
    for (int i = -1; i <= 1; i += 2)
    {
        for (int j = -1; j <= 1; j += 2)
        {
            glPushMatrix();
            glTranslatef(0.7f * i, -0.6f, 0.3f * j);
            drawCylinder(0.1, 0.1, 0.7);
            glPopMatrix();
        }
    }
    glPopMatrix();
}