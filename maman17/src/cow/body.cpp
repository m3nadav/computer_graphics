#define GL_SILENCE_DEPRECATION // Silence deprecation warnings on macOS
#include "cow/body.h"
#include "cow/spots.h"
#include "shapes/shapes.h"
#include <GLUT/glut.h>
#include <cmath>
#include <iostream>

void drawBody()
{
    glPushMatrix();
    glColor3f(0.95f, 0.95f, 0.95f); // White
    glPushMatrix();
    glScalef(1.0f, 0.6f, 0.6f);

    // Draw textured ellipsoid with spots texture
    unsigned int spotsTexture = getCowSpotsTexture();
    drawTexturedEllipsoid(1.0, 1.0, 1.0, spotsTexture, false);

    glPopMatrix();
    glPopMatrix();
}