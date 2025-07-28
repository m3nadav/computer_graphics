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
    glTranslatef(-0.6f, 0.3f, 0.0f); // Start position at back of body
    glColor3f(0.95f, 0.95f, 0.95f);  // Match body color

    // Define spline control points for S-curve (right then left)
    float controlPoints[4][3] = {
        {0.0f, 0.0f, 0.0f},     // Start point
        {-0.2f, -0.1f, 0.15f},  // Curve right (positive z)
        {-0.4f, -0.25f, -0.1f}, // Curve left (negative z)
        {-0.6f, -0.4f, 0.0f}    // End point (back to center)
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

        // Calculate current and next positions on spline
        float pos1[3], pos2[3];
        for (int j = 0; j < 3; j++)
        {
            pos1[j] = (1 - t1) * (1 - t1) * (1 - t1) * controlPoints[0][j] +
                      3 * (1 - t1) * (1 - t1) * t1 * controlPoints[1][j] +
                      3 * (1 - t1) * t1 * t1 * controlPoints[2][j] +
                      t1 * t1 * t1 * controlPoints[3][j];

            pos2[j] = (1 - t2) * (1 - t2) * (1 - t2) * controlPoints[0][j] +
                      3 * (1 - t2) * (1 - t2) * t2 * controlPoints[1][j] +
                      3 * (1 - t2) * t2 * t2 * controlPoints[2][j] +
                      t2 * t2 * t2 * controlPoints[3][j];
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
    glTranslatef(controlPoints[3][0], controlPoints[3][1], controlPoints[3][2]);
    glColor3f(0.1f, 0.1f, 0.1f);         // Black hair
    glRotatef(-30.0f, 0.0f, 0.0f, 1.0f); // Rotate 30 degrees clockwise around Z axis
    glScalef(0.04f, 0.06f, 0.04f);
    drawEllipsoid(1.0, 1.0, 1.0);
    glPopMatrix();

    glPopMatrix();
}