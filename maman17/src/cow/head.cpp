#include "cow/head.h"
#include "cow/spots.h"
#include "cow/cow_coordinates.h"
#include "cow/cow.h"
#include "shapes/shapes.h"
#include "environment/environment.h"
#include "environment/lights.h"
#include <GLUT/glut.h>

void drawFullHead()
{
    glPushMatrix();
    glTranslatef(COW_HEAD_X, COW_HEAD_Y, COW_HEAD_Z);

    // Apply head rotations for user control
    glRotatef(getHeadRotationY(), 0, 1, 0); // Left/right rotation
    glRotatef(getHeadRotationX(), 0, 0, 1); // Up/down rotation (around Z-axis)

    drawHead();
    drawHeadMuzzle();
    drawHeadEyes();
    drawHeadHorns();
    drawHeadEars();
    drawHeadNostrils();

    glPopMatrix();
}
void drawHead()
{
    glPushMatrix();
    setCowWhiteMaterial();
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
    setCowBeigeMaterial();

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
        // Position eyes higher than nose (nose at y=0.0f) and wider spaced
        // Head ellipsoid radius = 0.35f, at y=0.12f, z=0.12f, surface X ≈ 0.29f
        // Place eyes slightly outside surface for visibility
        glTranslatef(0.30f, 0.12f, 0.12f * s);

        // Create eye socket (rotated 90 degrees around Y-axis to align with head surface)
        glPushMatrix();
        glTranslatef(-0.005f, 0.0f, 0.0f); // Less recessed for flatter look
        setCowPinkMaterial();              // Eye socket color
        glScalef(0.01f, 0.04f, 0.05f);     // Rotated: depth in X, height in Y, width in Z
        drawEllipsoid(1.0, 1.0, 1.0);
        glPopMatrix();

        // Draw white eyeball (sclera) - positioned on rotated socket surface
        glPushMatrix();
        glTranslatef(0.005f, 0.0f, 0.0f); // Less forward projection
        setCowWhiteMaterial();
        glScalef(0.008f, 0.035f, 0.035f); // Rotated: very flat X depth, height Y, width Z
        drawEllipsoid(1.0, 1.0, 1.0);
        glPopMatrix();

        // Draw black pupil with iris - on rotated surface
        glPushMatrix();
        glTranslatef(0.010f, 0.0f, 0.0f); // Less forward projection
        setCowBlackMaterial();
        glScalef(0.005f, 0.012f, 0.015f); // Rotated: flat X depth, pupil size Y and Z
        drawEllipsoid(1.0, 1.0, 1.0);
        glPopMatrix();

        // Add small highlight for realism - on rotated surface
        glPushMatrix();
        glTranslatef(0.015f, 0.005f, -0.002f * s); // Less forward projection
        setCowWhiteMaterial();
        glScalef(0.003f, 0.006f, 0.006f); // Rotated: flat X depth, highlight size Y and Z
        drawSphere(1.0);
        glPopMatrix();

        glPopMatrix();
    }
}

void drawHeadHorns()
{
    for (int s = -1; s <= 1; s += 2)
    {
        glPushMatrix();
        // Position horns on top of head where ears used to be
        glTranslatef(-0.05f, 0.30f, 0.15f * s);
        setCowHornMaterial();

        // Create curved horn using multiple segments
        int segments = 6;
        float segmentHeight = 0.018f;
        float baseRadius = 0.02f;

        for (int i = 0; i < segments; i++)
        {
            glPushMatrix();

            // Calculate position and rotation for curved effect
            float t = (float)i / (segments - 1);
            float curveAngle = t * 25.0f * s; // Curve outward, mirrored
            float yOffset = i * segmentHeight;
            float radius = baseRadius * (1.0f - t * 0.6f); // Taper to point

            // Apply curve transformation
            glTranslatef(0.0f, yOffset, 0.0f);
            glRotatef(curveAngle, 1, 0, 0); // Curve forward and outward
            glRotatef(15 * s, 0, 0, 1);     // Slight outward angle

            // Draw horn segment as tapered cylinder
            drawCylinder(radius, radius * 0.8f, segmentHeight);

            glPopMatrix();
        }

        glPopMatrix();
    }
}

void drawHeadEars()
{
    for (int s = -1; s <= 1; s += 2)
    {
        glPushMatrix();
        // Position ears prominently on the sides of the head
        glTranslatef(-0.05f, 0.15f, 0.25f * s); // More visible position
        glRotatef(20 * s, 0, 1, 0);             // Slight outward angle
        glRotatef(-10, 1, 0, 0);                // Slight backward tilt
        setCowPinkMaterial();

        // Create ear shape: large oval base with pointed tip
        // Main oval part of the ear - reduced to 75% size
        glPushMatrix();
        glScalef(0.1125f, 0.135f, 0.0375f); // 75% of previous size
        drawEllipsoid(1.0, 1.0, 1.0);
        glPopMatrix();

        // Pointed tip at the top - also reduced to 75%
        glPushMatrix();
        glTranslatef(0.0f, 0.09f, 0.0f); // Move to top of oval (adjusted for smaller ear)
        glScalef(0.06f, 0.06f, 0.0225f); // 75% of previous size
        drawEllipsoid(1.0, 1.0, 1.0);
        glPopMatrix();

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
        setCowDarkBrownMaterial();
        drawSphere(1.0);
        glPopMatrix();
    }
}
