#include "cow/head.h"
#include "cow/spots.h"
#include "cow/cow_coordinates.h"
#include "cow/cow.h"
#include "shapes/shapes.h"
#include "environment/environment.h"
#include "environment/lights.h"
#include <GLUT/glut.h>
#include <cmath>

/**
 * Renders the complete cow head assembly with user-controlled positioning.
 * Coordinates all head components including facial features, applies user
 * rotation controls, and manages the head's position relative to the body.
 */
void drawFullHead()
{
    glPushMatrix();
    glTranslatef(COW_HEAD_X, COW_HEAD_Y, COW_HEAD_Z);

    // Apply head rotations for user control
    glRotatef(getHeadRotationY(), 0, 1, 0); // Left/right rotation
    glRotatef(getHeadRotationZ(), 0, 0, 1); // Up/down rotation (around Z-axis)

    drawHead();
    drawHeadMuzzle();
    drawHeadEyes();
    drawHeadHorns();
    drawHeadEars();
    drawHeadNostrils();

    glPopMatrix();
}
/**
 * Creates the main head geometry as a scaled ellipsoid with spots texture.
 * Forms the base head shape that other facial features are attached to,
 * using mirrored texture coordinates for visual variety from the body.
 */
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

// Constants for spline-based horn rendering
const int HORN_CONTROL_POINTS = 5;
const int HORN_SEGMENTS = 15;
const int HORN_RADIAL_SEGMENTS = 8;
const float HORN_BASE_RADIUS = 0.02f;
const float HORN_TAPER_FACTOR = 0.8f;

// Helper function to calculate position on horn spline using 4th degree polynomial
void calculateHornSplinePosition(float t, float controlPoints[5][3], float pos[3])
{
    // 4th degree polynomial: P(t) = (1-t)⁴P₀ + 4(1-t)³tP₁ + 6(1-t)²t²P₂ + 4(1-t)t³P₃ + t⁴P₄
    float t_4 = t * t * t * t;
    float t_3 = t * t * t;
    float t_2 = t * t;
    float t_1 = t;

    for (int j = 0; j < 3; j++)
    {
        pos[j] = (1 - t_1) * (1 - t_1) * (1 - t_1) * (1 - t_1) * controlPoints[0][j] +
                 4 * (1 - t_1) * (1 - t_1) * (1 - t_1) * t_1 * controlPoints[1][j] +
                 6 * (1 - t_1) * (1 - t_1) * t_2 * controlPoints[2][j] +
                 4 * (1 - t_1) * t_3 * controlPoints[3][j] +
                 t_4 * controlPoints[4][j];
    }
}

// Helper function to calculate perpendicular vectors for horn cone cross-section
void calculateHornPerpendicularVectors(float dx, float dy, float dz, float right[3], float forward[3])
{
    // Calculate perpendicular vectors for cone cross-section
    float up[3] = {0, 1, 0};

    // Calculate right vector using cross product
    right[0] = dy * up[2] - dz * up[1];
    right[1] = dz * up[0] - dx * up[2];
    right[2] = dx * up[1] - dy * up[0];

    float rightLen = sqrt(right[0] * right[0] + right[1] * right[1] + right[2] * right[2]);
    if (rightLen > 0.001f)
    {
        right[0] /= rightLen;
        right[1] /= rightLen;
        right[2] /= rightLen;
    }

    // Calculate forward vector using cross product
    forward[0] = dy * right[2] - dz * right[1];
    forward[1] = dz * right[0] - dx * right[2];
    forward[2] = dx * right[1] - dy * right[0];
}

// Helper function to draw a single horn cone segment
void drawHornConeSegment(float pos1[3], float pos2[3], float radius1, float radius2, float right[3], float forward[3])
{
    // Create triangles around the cone circumference
    for (int k = 0; k < HORN_RADIAL_SEGMENTS; k++)
    {
        float angle1 = 2.0f * M_PI * k / HORN_RADIAL_SEGMENTS;
        float angle2 = 2.0f * M_PI * (k + 1) / HORN_RADIAL_SEGMENTS;

        float cos1 = cos(angle1), sin1 = sin(angle1);
        float cos2 = cos(angle2), sin2 = sin(angle2);

        // Calculate points on cone surface
        float p1[3], p2[3], p3[3], p4[3];

        // Current ring
        for (int j = 0; j < 3; j++)
        {
            p1[j] = pos1[j] + radius1 * (cos1 * right[j] + sin1 * forward[j]);
            p2[j] = pos1[j] + radius1 * (cos2 * right[j] + sin2 * forward[j]);
        }

        // Next ring
        for (int j = 0; j < 3; j++)
        {
            p3[j] = pos2[j] + radius2 * (cos1 * right[j] + sin1 * forward[j]);
            p4[j] = pos2[j] + radius2 * (cos2 * right[j] + sin2 * forward[j]);
        }

        // Draw two triangles for this section
        glVertex3f(p1[0], p1[1], p1[2]);
        glVertex3f(p3[0], p3[1], p3[2]);
        glVertex3f(p2[0], p2[1], p2[2]);

        glVertex3f(p2[0], p2[1], p2[2]);
        glVertex3f(p3[0], p3[1], p3[2]);
        glVertex3f(p4[0], p4[1], p4[2]);
    }
}

// Helper function to draw the horn cone surface
void drawHornCone(float controlPoints[5][3])
{
    glBegin(GL_TRIANGLES);

    // Generate cone surface following the spline
    for (int i = 0; i < HORN_SEGMENTS; i++)
    {
        float t1 = (float)i / HORN_SEGMENTS;
        float t2 = (float)(i + 1) / HORN_SEGMENTS;

        // Calculate current and next positions on spline
        float pos1[3], pos2[3];
        calculateHornSplinePosition(t1, controlPoints, pos1);
        calculateHornSplinePosition(t2, controlPoints, pos2);

        // Calculate radii (taper from base to tip)
        float radius1 = HORN_BASE_RADIUS * (1.0f - t1 * HORN_TAPER_FACTOR);
        float radius2 = HORN_BASE_RADIUS * (1.0f - t2 * HORN_TAPER_FACTOR);

        // Calculate direction for proper orientation
        float dx = pos2[0] - pos1[0];
        float dy = pos2[1] - pos1[1];
        float dz = pos2[2] - pos1[2];
        float len = sqrt(dx * dx + dy * dy + dz * dz);

        if (len > 0.001f)
        {
            dx /= len;
            dy /= len;
            dz /= len; // Normalize direction

            // Calculate perpendicular vectors for cone cross-section
            float right[3], forward[3];
            calculateHornPerpendicularVectors(dx, dy, dz, right, forward);

            // Draw the cone segment
            drawHornConeSegment(pos1, pos2, radius1, radius2, right, forward);
        }
    }

    glEnd();
}

void drawHeadHorns()
{
    for (int s = -1; s <= 1; s += 2)
    {
        glPushMatrix();
        // Position horns on top of head
        glTranslatef(-0.05f, 0.30f, 0.15f * s);
        setCowHornMaterial();

        // Define control points for realistic bull horn geometry
        // Horns curve outward then back toward center while growing up and slightly forward
        float controlPoints[HORN_CONTROL_POINTS][3] = {
            {0.0f, 0.0f, 0.0f},            // Base point at head
            {0.01f * s, 0.02f, 0.03f * s}, // Start curving up and slightly outward
            {0.03f * s, 0.05f, 0.06f * s}, // Maximum outward curve, growing up
            {0.02f * s, 0.08f, 0.04f * s}, // Start curving back toward center
            {0.0f * s, 0.11f, 0.01f * s}   // End pointing slightly forward and inward
        };

        // Draw the horn using spline-based cone surface
        drawHornCone(controlPoints);

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
