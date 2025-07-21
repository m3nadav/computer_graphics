#define GL_SILENCE_DEPRECATION // Silence deprecation warnings on macOS
#include "cow/body.h"
#include "cow/spots.h"
#include "shapes/shapes.h"
#include <GLUT/glut.h>
#include <cmath>
#include <iostream>

// Global texture ID for cow spots
static unsigned int cowSpotsTexture = 0;
static bool textureLoaded = false;

void drawBody()
{
    // Load texture once
    if (!textureLoaded)
    {
        cowSpotsTexture = loadTexture("SPOTS.png");
        textureLoaded = true;
    }

    glPushMatrix();
    glColor3f(0.95f, 0.95f, 0.95f); // White
    glPushMatrix();
    glScalef(1.2f, 0.6f, 0.4f);

    // Draw textured ellipsoid instead of individual spots
    drawTexturedEllipsoid(1.0, 1.0, 1.0, cowSpotsTexture);

    glPopMatrix();
    glPopMatrix();
}