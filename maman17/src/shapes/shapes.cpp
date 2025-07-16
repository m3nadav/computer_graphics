#include "shapes/shapes.h"
#include <GLUT/glut.h>

void drawSphere(double radius)
{
    glutSolidSphere(radius, 32, 32);
}

void drawEllipsoid(double rx, double ry, double rz)
{
    glPushMatrix();
    glScalef(rx, ry, rz);
    glutSolidSphere(1.0, 32, 32);
    glPopMatrix();
}

void drawCylinder(double baseRadius, double topRadius, double height)
{
#ifdef GLUT_VERSION_3_7
    glutSolidCylinder(baseRadius, height, 32, 8);
#else
    GLUquadric *quad = gluNewQuadric();
    gluCylinder(quad, baseRadius, topRadius, height, 32, 8);
    gluDeleteQuadric(quad);
#endif
}