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

void drawTail()
{
    // TAIL (single cone following spline curve)
    glPushMatrix();
    glTranslatef(-0.866f, 0.15f, 0.0f); // Start position at back of body
    glColor3f(0.5f, 0.5f, 0.5f);        // Match body color

    // Define spline control points for realistic tail curve
    float controlPoints[5][3] = {
        {0.0f, 0.0f, 0.0f},      // Start point
        {-0.2f, 0.0f, 0.0f},     // Straight back with no curve or height change
        {-0.25f, -0.05f, 0.00f}, // Curve right while going down and back
        {-0.35f, -0.15f, -0.2f}, // Curve left while going down and back more
        {-0.45f, -0.25f, 0.0f}   // Curve right and end pointing right
    };

    // Create single curved cone using triangle strips
    int segments = 20; // More segments for smoother curve
    float baseRadius = 0.04f;
    int radialSegments = 8; // Number of sides around the cone

    glBegin(GL_TRIANGLES);

    // Generate cone surface following the spline
    for (int i = 0; i < segments; i++)
    {
        float t1 = (float)i / segments;
        float t2 = (float)(i + 1) / segments;

        // Calculate current and next positions on spline (using 4th degree polynomial for 5 points)
        float pos1[3], pos2[3];
        for (int j = 0; j < 3; j++)
        {
            // 4th degree polynomial: P(t) = (1-t)⁴P₀ + 4(1-t)³tP₁ + 6(1-t)²t²P₂ + 4(1-t)t³P₃ + t⁴P₄
            float t1_4 = t1 * t1 * t1 * t1;
            float t1_3 = t1 * t1 * t1;
            float t1_2 = t1 * t1;
            float t1_1 = t1;
            float t1_0 = 1.0f;

            float t2_4 = t2 * t2 * t2 * t2;
            float t2_3 = t2 * t2 * t2;
            float t2_2 = t2 * t2;
            float t2_1 = t2;
            float t2_0 = 1.0f;

            pos1[j] = (1 - t1_1) * (1 - t1_1) * (1 - t1_1) * (1 - t1_1) * controlPoints[0][j] +
                      4 * (1 - t1_1) * (1 - t1_1) * (1 - t1_1) * t1_1 * controlPoints[1][j] +
                      6 * (1 - t1_1) * (1 - t1_1) * t1_2 * controlPoints[2][j] +
                      4 * (1 - t1_1) * t1_3 * controlPoints[3][j] +
                      t1_4 * controlPoints[4][j];

            pos2[j] = (1 - t2_1) * (1 - t2_1) * (1 - t2_1) * (1 - t2_1) * controlPoints[0][j] +
                      4 * (1 - t2_1) * (1 - t2_1) * (1 - t2_1) * t2_1 * controlPoints[1][j] +
                      6 * (1 - t2_1) * (1 - t2_1) * t2_2 * controlPoints[2][j] +
                      4 * (1 - t2_1) * t2_3 * controlPoints[3][j] +
                      t2_4 * controlPoints[4][j];
        }

        // Calculate radii (taper from base to tip)
        float radius1 = baseRadius * (1.0f - t1 * 0.8f);
        float radius2 = baseRadius * (1.0f - t2 * 0.8f);

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
            float up[3] = {0, 1, 0};
            float right[3] = {dy * up[2] - dz * up[1], dz * up[0] - dx * up[2], dx * up[1] - dy * up[0]};
            float rightLen = sqrt(right[0] * right[0] + right[1] * right[1] + right[2] * right[2]);
            if (rightLen > 0.001f)
            {
                right[0] /= rightLen;
                right[1] /= rightLen;
                right[2] /= rightLen;
            }

            float forward[3] = {dy * right[2] - dz * right[1], dz * right[0] - dx * right[2], dx * right[1] - dy * right[0]};

            // Create triangles around the cone circumference
            for (int k = 0; k < radialSegments; k++)
            {
                float angle1 = 2.0f * M_PI * k / radialSegments;
                float angle2 = 2.0f * M_PI * (k + 1) / radialSegments;

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
    }

    glEnd();

    // Hair tuft at the tip
    glPushMatrix();
    glTranslatef(controlPoints[4][0], controlPoints[4][1], controlPoints[4][2]);
    glColor3f(0.1f, 0.1f, 0.1f);         // Black hair
    glRotatef(-50.0f, 1.0f, 0.0f, 0.5f); // Rotate 50 degrees clockwise around Z axis
    glScalef(0.04f, 0.06f, 0.04f);
    drawEllipsoid(1.0, 1.0, 1.0);
    glPopMatrix();

    glPopMatrix();
}

void drawLegs()
{
    // Define leg positions: front left, front right, back left, back right
    float legPositions[4][3] = {
        {0.4f, -0.3f, 0.25f},   // Front left
        {0.4f, -0.3f, -0.25f},  // Front right  
        {-0.4f, -0.3f, 0.25f},  // Back left
        {-0.4f, -0.3f, -0.25f}  // Back right
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
        glColor3f(0.2f, 0.1f, 0.1f); // Dark brown/black
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