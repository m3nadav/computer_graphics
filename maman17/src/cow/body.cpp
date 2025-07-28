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

unsigned int getCowSpotsTexture()
{
    // Load texture once if not already loaded
    if (!textureLoaded)
    {
        cowSpotsTexture = loadTexture("SPOTS.png");
        textureLoaded = true;
    }
    return cowSpotsTexture;
}

void drawBody()
{
    // Ensure texture is loaded
    getCowSpotsTexture();

    glPushMatrix();
    glColor3f(0.95f, 0.95f, 0.95f); // White
    glPushMatrix();
    glScalef(1.0f, 0.6f, 0.6f);

    // Draw textured ellipsoid instead of individual spots
    drawTexturedEllipsoid(1.0, 1.0, 1.0, cowSpotsTexture, false);

    glPopMatrix();
    glPopMatrix();
}