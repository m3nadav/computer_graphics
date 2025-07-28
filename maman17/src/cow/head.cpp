#include "cow/head.h"
#include "cow/body.h"
#include "shapes/shapes.h"
#include <GLUT/glut.h>

void drawFullHead()
{
    glPushMatrix();
    glTranslatef(0.9f, 0.5f, 0.0f);
    glRotatef(0, 10, 0, 1);

    drawHead();
    drawHeadMuzzle();
    drawHeadEyes();
    drawHeadHorns();
    drawHeadEars();
    drawHeadNostrils();

    glPopMatrix(); // End head/neck group
    glPopMatrix(); // End neck
}
void drawHead()
{
    glPushMatrix();
    glColor3f(0.95f, 0.95f, 0.95f); // White base color like body
    glScalef(0.35f, 0.35f, 0.35f);
    
    // Use the same spots texture as the body, but mirrored for variety
    unsigned int spotsTexture = getCowSpotsTexture();
    drawTexturedEllipsoid(1.0, 1.0, 1.0, spotsTexture, true);
    
    glPopMatrix();
}

void drawHeadMuzzle()
{
    glPushMatrix();
    // Position muzzle at the front of the head
    glTranslatef(0.28f, 0.0f, 0.0f);
    glScalef(0.12f, 0.08f, 0.08f);
    glColor3f(0.85f, 0.7f, 0.6f);

    // Draw a slightly flattened muzzle by using a custom shape
    // First draw most of the muzzle as an ellipsoid
    glPushMatrix();
    glScalef(0.9f, 1.0f, 1.0f); // Slightly shorter in x direction
    drawEllipsoid(1.0, 1.0, 1.0);
    glPopMatrix();

    // Then add a flattened end
    glPushMatrix();
    glTranslatef(0.9f, 0.0f, 0.0f);
    glScalef(0.4f, 0.9f, 0.9f);
    glPopMatrix();

    glPopMatrix();
}

void drawHeadEyes()
{
    for (int s = -1; s <= 1; s += 2)
    {
        glPushMatrix();
        // Position eyes on the sides of the head, slightly forward
        glTranslatef(0.15f, 0.05f, 0.21f * s);
        glScalef(0.02f, 0.02f, 0.02f);
        glColor3f(0.1f, 0.1f, 0.1f);
        drawSphere(1.0);
        glPopMatrix();
    }
}

void drawHeadHorns()
{
    for (int s = -1; s <= 1; s += 2)
    {
        glPushMatrix();
        // Position horns on top of head, slightly back
        glTranslatef(-0.05f, 0.28f, 0.15f * s);
        glRotatef(30 * s, 0, 0, 1);
        // glRotatef(-30, 0, 1, 0);
        glColor3f(0.9f, 0.9f, 0.7f);
        drawCylinder(0.015, 0.01, 0.11);
        glPopMatrix();
    }
}

void drawHeadEars()
{
    for (int s = -1; s <= 1; s += 2)
    {
        glPushMatrix();
        // Position ears on sides of head, behind eyes
        glTranslatef(-0.05f, 0.30f, 0.245f * s);
        glRotatef(60, 0, 0, 1);
        glScalef(0.08f, 0.04f, 0.03f);
        glColor3f(0.7f, 0.5f, 0.3f);
        drawEllipsoid(1.0, 1.0, 1.0);
        glPopMatrix();
    }
}

void drawHeadNostrils()
{
    for (int s = -1; s <= 1; s += 2)
    {
        glPushMatrix();
        // Position nostrils at the front of the muzzle
        glTranslatef(0.36f, 0.0f, 0.05f * s);
        glScalef(0.012f, 0.012f, 0.012f);
        glColor3f(0.2f, 0.1f, 0.1f);
        drawSphere(1.0);
        glPopMatrix();
    }
}
