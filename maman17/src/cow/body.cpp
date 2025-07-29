#define GL_SILENCE_DEPRECATION // Silence deprecation warnings on macOS
#include "cow/body.h"
#include "cow/spots.h"
#include "cow/tail.h"
#include "shapes/shapes.h"
#include "scene/scene.h"
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

void drawLegs()
{
    // Define leg positions: front left, front right, back left, back right
    float legPositions[4][3] = {
        {0.4f, -0.3f, 0.25f},  // Front left
        {0.4f, -0.3f, -0.25f}, // Front right
        {-0.4f, -0.3f, 0.25f}, // Back left
        {-0.4f, -0.3f, -0.25f} // Back right
    };

    for (int i = 0; i < 4; i++)
    {
        glPushMatrix();
        glTranslatef(legPositions[i][0], legPositions[i][1], legPositions[i][2]);

        // Upper leg (thigh)
        glPushMatrix();
        glColor3f(0.95f, 0.95f, 0.95f); // Match body color
        glScalef(0.08f, 0.15f, 0.08f);
        drawEllipsoid(1.0, 1.0, 1.0);
        glPopMatrix();

        // Knee joint
        glPushMatrix();
        glTranslatef(0.0f, -0.12f, 0.0f);
        glColor3f(0.9f, 0.9f, 0.9f); // Slightly darker
        glScalef(0.06f, 0.04f, 0.06f);
        drawEllipsoid(1.0, 1.0, 1.0);
        glPopMatrix();

        // Lower leg (shin)
        glPushMatrix();
        glTranslatef(0.0f, -0.22f, 0.0f);
        glColor3f(0.95f, 0.95f, 0.95f);
        glScalef(0.06f, 0.12f, 0.06f);
        drawEllipsoid(1.0, 1.0, 1.0);
        glPopMatrix();

        // Ankle joint
        glPushMatrix();
        glTranslatef(0.0f, -0.32f, 0.0f);
        glColor3f(0.9f, 0.9f, 0.9f);
        glScalef(0.05f, 0.03f, 0.05f);
        drawEllipsoid(1.0, 1.0, 1.0);
        glPopMatrix();

        // Hoof
        glPushMatrix();
        glTranslatef(0.0f, -0.38f, 0.0f);
        glColor3f(0.2f, 0.1f, 0.1f);   // Dark brown/black
        glScalef(0.06f, 0.06f, 0.08f); // Slightly elongated forward
        drawEllipsoid(1.0, 1.0, 1.0);
        glPopMatrix();

        // Hoof split (cloven hoof detail)
        glPushMatrix();
        glTranslatef(0.0f, -0.36f, 0.0f);
        glColor3f(0.1f, 0.05f, 0.05f); // Even darker line
        glScalef(0.01f, 0.04f, 0.08f);
        drawEllipsoid(1.0, 1.0, 1.0);
        glPopMatrix();

        glPopMatrix();
    }
}