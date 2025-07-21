#define GL_SILENCE_DEPRECATION // Silence deprecation warnings on macOS
#include "cow/cow.h"
#include "cow/head.h"
#include "cow/body.h"
#include "shapes/shapes.h"
#include <GLUT/glut.h>
#include <cmath>
#include <cstdlib>
#include <ctime>

void drawCow()
{
    drawBody();

    drawFullHead();

    // LEGS (vertical, under body)
    float legX[4] = {0.5f, 0.5f, -0.5f, -0.5f};
    float legZ[4] = {0.18f, -0.18f, 0.18f, -0.18f};
    for (int i = 0; i < 4; ++i)
    {
        glPushMatrix();
        glTranslatef(legX[i], -0.6f, legZ[i]);
        glColor3f(0.95f, 0.95f, 0.95f);
        drawCylinder(0.06, 0.05, 0.5);
        // Hoof
        glTranslatef(0, -0.01f, 0);
        glColor3f(0.2f, 0.1f, 0.1f);
        drawCylinder(0.05, 0.05, 0.08);
        glPopMatrix();
    }

    // TAIL (thin, with tuft)
    glPushMatrix();
    glTranslatef(-1.1f, 0.0f, 0.0f);
    glRotatef(-20, 0, 0, 1);
    glColor3f(0.7f, 0.5f, 0.3f);
    drawCylinder(0.025, 0.015, 0.45);
    // Tail tuft
    glTranslatef(0, -0.05f, 0);
    glColor3f(0.1f, 0.1f, 0.1f);
    drawEllipsoid(0.05, 0.09, 0.05);
    glPopMatrix();

    glPopMatrix();
}