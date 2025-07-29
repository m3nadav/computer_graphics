#define GL_SILENCE_DEPRECATION // Silence deprecation warnings on macOS
#include "cow/tail.h"
#include "shapes/shapes.h"
#include "scene/scene.h"
#include <GLUT/glut.h>
#include <cmath>
#include <iostream>

// Function to calculate tangent vector at the end of the tail curve
float calculateTailTangent(float controlPoints[5][3], float tangent[3])
{
    // Tangent at the end is the direction from second-to-last to last control point
    tangent[0] = controlPoints[4][0] - controlPoints[3][0];
    tangent[1] = controlPoints[4][1] - controlPoints[3][1];
    tangent[2] = controlPoints[4][2] - controlPoints[3][2];

    // Normalize the tangent vector
    float length = sqrt(tangent[0] * tangent[0] + tangent[1] * tangent[1] + tangent[2] * tangent[2]);
    if (length > 0.001f)
    {
        tangent[0] /= length;
        tangent[1] /= length;
        tangent[2] /= length;
    }

    return length;
}

// Function to calculate rotation from tangent vector to align hair tuft
void calculateHairTuftRotation(float tangent[3], float &angle, float &axisX, float &axisY, float &axisZ)
{
    // Hair tuft is initially oriented along Y-axis (up)
    float up[3] = {0.0f, 1.0f, 0.0f};

    // Calculate rotation axis using cross product: up × tangent
    axisX = up[1] * tangent[2] - up[2] * tangent[1];
    axisY = up[2] * tangent[0] - up[0] * tangent[2];
    axisZ = up[0] * tangent[1] - up[1] * tangent[0];

    // Normalize the rotation axis
    float axisLength = sqrt(axisX * axisX + axisY * axisY + axisZ * axisZ);

    if (axisLength > 0.001f)
    {
        // Normal case: vectors are not parallel
        axisX /= axisLength;
        axisY /= axisLength;
        axisZ /= axisLength;

        // Calculate rotation angle using dot product: cos(θ) = up · tangent
        float dotProduct = up[0] * tangent[0] + up[1] * tangent[1] + up[2] * tangent[2];
        // Clamp dot product to [-1, 1] to avoid numerical errors
        if (dotProduct > 1.0f)
            dotProduct = 1.0f;
        if (dotProduct < -1.0f)
            dotProduct = -1.0f;

        angle = acos(dotProduct) * 180.0f / M_PI; // Convert to degrees

        // Better approach: Use the magnitude of the cross product to determine direction
        // If the cross product points in the negative Z direction, reverse the angle
        if (axisZ < 0)
        {
            angle = -angle;
        }
    }
    else
    {
        // Special case: vectors are parallel (tangent points up or down)
        // No rotation needed - set angle to 0 and use any axis (won't matter)
        angle = 0.0f;
        axisX = 1.0f;
        axisY = 0.0f;
        axisZ = 0.0f;
    }
}

void drawTail()
{
    // ANIMATED TAIL (multiple cone variations following different spline curves)
    glPushMatrix();
    glTranslatef(-0.866f, 0.15f, 0.0f); // Start position at back of body
    glColor3f(0.5f, 0.5f, 0.5f);        // Match body color

    // Create 30 different tail positions for realistic wavy motion
    float tailVariations[30][5][3] = {
        // Variation 1: Starting position - tail pointing down
        {
            {0.0f, 0.0f, 0.0f},     // Start point
            {-0.2f, 0.0f, 0.0f},    // Straight back
            {-0.25f, -0.05f, 0.0f}, // Slight down
            {-0.35f, -0.1f, 0.0f},  // More down
            {-0.45f, -0.15f, 0.0f}  // End pointing down
        },
        // Variation 2: Start moving middle to left, tuft lags behind
        {
            {0.0f, 0.0f, 0.0f},       // Start point
            {-0.2f, 0.0f, 0.0f},      // Straight back
            {-0.25f, -0.05f, -0.02f}, // Middle starts left
            {-0.35f, -0.1f, -0.03f},  // Middle more left, tuft still down
            {-0.45f, -0.15f, -0.01f}  // Tuft dragged behind, slight left
        },
        // Variation 3: Middle continues left, tuft catching up
        {
            {0.0f, 0.0f, 0.0f},       // Start point
            {-0.2f, 0.0f, 0.0f},      // Straight back
            {-0.25f, -0.05f, -0.05f}, // Middle further left
            {-0.35f, -0.1f, -0.08f},  // Middle more left
            {-0.45f, -0.15f, -0.05f}  // Tuft catching up
        },
        // Variation 4: Maximum left swing, tuft almost caught up
        {
            {0.0f, 0.0f, 0.0f},       // Start point
            {-0.2f, 0.0f, 0.0f},      // Straight back
            {-0.25f, -0.05f, -0.08f}, // Middle max left
            {-0.35f, -0.1f, -0.12f},  // Middle max left
            {-0.45f, -0.15f, -0.1f}   // Tuft almost caught up
        },
        // Variation 5: Start reversing direction, middle leads
        {
            {0.0f, 0.0f, 0.0f},       // Start point
            {-0.2f, 0.0f, 0.0f},      // Straight back
            {-0.25f, -0.05f, -0.06f}, // Middle starts back
            {-0.35f, -0.1f, -0.08f},  // Middle reversing
            {-0.45f, -0.15f, -0.12f}  // Tuft still moving left (momentum)
        },
        // Variation 6: Middle moving right, tuft still left
        {
            {0.0f, 0.0f, 0.0f},       // Start point
            {-0.2f, 0.0f, 0.0f},      // Straight back
            {-0.25f, -0.05f, -0.03f}, // Middle moving right
            {-0.35f, -0.1f, -0.04f},  // Middle more right
            {-0.45f, -0.15f, -0.08f}  // Tuft still left (lagging)
        },
        // Variation 7: Middle center, tuft catching up
        {
            {0.0f, 0.0f, 0.0f},      // Start point
            {-0.2f, 0.0f, 0.0f},     // Straight back
            {-0.25f, -0.05f, 0.0f},  // Middle center
            {-0.35f, -0.1f, 0.0f},   // Middle center
            {-0.45f, -0.15f, -0.04f} // Tuft still left but catching up
        },
        // Variation 8: Middle right, tuft center
        {
            {0.0f, 0.0f, 0.0f},      // Start point
            {-0.2f, 0.0f, 0.0f},     // Straight back
            {-0.25f, -0.05f, 0.03f}, // Middle right
            {-0.35f, -0.1f, 0.04f},  // Middle right
            {-0.45f, -0.15f, 0.0f}   // Tuft center (caught up)
        },
        // Variation 9: Maximum right swing, tuft following
        {
            {0.0f, 0.0f, 0.0f},      // Start point
            {-0.2f, 0.0f, 0.0f},     // Straight back
            {-0.25f, -0.05f, 0.06f}, // Middle max right
            {-0.35f, -0.1f, 0.08f},  // Middle max right
            {-0.45f, -0.15f, 0.04f}  // Tuft following
        },
        // Variation 10: Start reversing again, middle leads
        {
            {0.0f, 0.0f, 0.0f},      // Start point
            {-0.2f, 0.0f, 0.0f},     // Straight back
            {-0.25f, -0.05f, 0.03f}, // Middle starts back
            {-0.35f, -0.1f, 0.04f},  // Middle reversing
            {-0.45f, -0.15f, 0.08f}  // Tuft still right (momentum)
        },
        // Variation 11: Middle moving left, tuft still right
        {
            {0.0f, 0.0f, 0.0f},     // Start point
            {-0.2f, 0.0f, 0.0f},    // Straight back
            {-0.25f, -0.05f, 0.0f}, // Middle moving left
            {-0.35f, -0.1f, 0.0f},  // Middle more left
            {-0.45f, -0.15f, 0.04f} // Tuft still right (lagging)
        },
        // Variation 12: Middle center, tuft catching up
        {
            {0.0f, 0.0f, 0.0f},       // Start point
            {-0.2f, 0.0f, 0.0f},      // Straight back
            {-0.25f, -0.05f, -0.03f}, // Middle center
            {-0.35f, -0.1f, -0.04f},  // Middle center
            {-0.45f, -0.15f, 0.0f}    // Tuft center (caught up)
        },
        // Variation 13: Continue right swing, middle leads
        {
            {0.0f, 0.0f, 0.0f},      // Start point
            {-0.2f, 0.0f, 0.0f},     // Straight back
            {-0.25f, -0.05f, 0.03f}, // Middle right
            {-0.35f, -0.1f, 0.04f},  // Middle right
            {-0.45f, -0.15f, 0.0f}   // Tuft center (caught up)
        },
        // Variation 14: Further right swing
        {
            {0.0f, 0.0f, 0.0f},      // Start point
            {-0.2f, 0.0f, 0.0f},     // Straight back
            {-0.25f, -0.05f, 0.06f}, // Middle further right
            {-0.35f, -0.1f, 0.08f},  // Middle further right
            {-0.45f, -0.15f, 0.04f}  // Tuft following
        },
        // Variation 15: Even further right swing
        {
            {0.0f, 0.0f, 0.0f},      // Start point
            {-0.2f, 0.0f, 0.0f},     // Straight back
            {-0.25f, -0.05f, 0.09f}, // Middle even further right
            {-0.35f, -0.1f, 0.12f},  // Middle even further right
            {-0.45f, -0.15f, 0.08f}  // Tuft following
        },
        // Variation 16: Maximum extended right swing
        {
            {0.0f, 0.0f, 0.0f},      // Start point
            {-0.2f, 0.0f, 0.0f},     // Straight back
            {-0.25f, -0.05f, 0.12f}, // Middle max extended right
            {-0.35f, -0.1f, 0.16f},  // Middle max extended right
            {-0.45f, -0.15f, 0.12f}  // Tuft following
        },
        // Variation 17: Start reversing from max right, middle leads
        {
            {0.0f, 0.0f, 0.0f},      // Start point
            {-0.2f, 0.0f, 0.0f},     // Straight back
            {-0.25f, -0.05f, 0.09f}, // Middle starts back
            {-0.35f, -0.1f, 0.12f},  // Middle reversing
            {-0.45f, -0.15f, 0.16f}  // Tuft still max right (momentum)
        },
        // Variation 18: Middle moving back, tuft still extended right
        {
            {0.0f, 0.0f, 0.0f},      // Start point
            {-0.2f, 0.0f, 0.0f},     // Straight back
            {-0.25f, -0.05f, 0.06f}, // Middle moving back
            {-0.35f, -0.1f, 0.08f},  // Middle more back
            {-0.45f, -0.15f, 0.12f}  // Tuft still extended right (lagging)
        },
        // Variation 19: Middle center, tuft still right
        {
            {0.0f, 0.0f, 0.0f},      // Start point
            {-0.2f, 0.0f, 0.0f},     // Straight back
            {-0.25f, -0.05f, 0.03f}, // Middle center
            {-0.35f, -0.1f, 0.04f},  // Middle center
            {-0.45f, -0.15f, 0.08f}  // Tuft still right (lagging)
        },
        // Variation 20: Middle left, tuft catching up
        {
            {0.0f, 0.0f, 0.0f},     // Start point
            {-0.2f, 0.0f, 0.0f},    // Straight back
            {-0.25f, -0.05f, 0.0f}, // Middle left
            {-0.35f, -0.1f, 0.0f},  // Middle left
            {-0.45f, -0.15f, 0.04f} // Tuft catching up
        },
        // Variation 21: Middle further left, tuft center
        {
            {0.0f, 0.0f, 0.0f},       // Start point
            {-0.2f, 0.0f, 0.0f},      // Straight back
            {-0.25f, -0.05f, -0.03f}, // Middle further left
            {-0.35f, -0.1f, -0.04f},  // Middle further left
            {-0.45f, -0.15f, 0.0f}    // Tuft center (caught up)
        },
        // Variation 22: Middle max left, tuft following
        {
            {0.0f, 0.0f, 0.0f},       // Start point
            {-0.2f, 0.0f, 0.0f},      // Straight back
            {-0.25f, -0.05f, -0.06f}, // Middle max left
            {-0.35f, -0.1f, -0.08f},  // Middle max left
            {-0.45f, -0.15f, -0.04f}  // Tuft following
        },
        // Variation 23: Start reversing from max left, middle leads
        {
            {0.0f, 0.0f, 0.0f},       // Start point
            {-0.2f, 0.0f, 0.0f},      // Straight back
            {-0.25f, -0.05f, -0.03f}, // Middle starts back
            {-0.35f, -0.1f, -0.04f},  // Middle reversing
            {-0.45f, -0.15f, -0.08f}  // Tuft still max left (momentum)
        },
        // Variation 24: Middle moving right, tuft still left
        {
            {0.0f, 0.0f, 0.0f},      // Start point
            {-0.2f, 0.0f, 0.0f},     // Straight back
            {-0.25f, -0.05f, 0.0f},  // Middle moving right
            {-0.35f, -0.1f, 0.0f},   // Middle more right
            {-0.45f, -0.15f, -0.04f} // Tuft still left (lagging)
        },
        // Variation 25: Middle center, tuft catching up
        {
            {0.0f, 0.0f, 0.0f},      // Start point
            {-0.2f, 0.0f, 0.0f},     // Straight back
            {-0.25f, -0.05f, 0.03f}, // Middle center
            {-0.35f, -0.1f, 0.04f},  // Middle center
            {-0.45f, -0.15f, 0.0f}   // Tuft center (caught up)
        },
        // Variation 26: Middle right, tuft following
        {
            {0.0f, 0.0f, 0.0f},      // Start point
            {-0.2f, 0.0f, 0.0f},     // Straight back
            {-0.25f, -0.05f, 0.06f}, // Middle right
            {-0.35f, -0.1f, 0.08f},  // Middle right
            {-0.45f, -0.15f, 0.04f}  // Tuft following
        },
        // Variation 27: Start settling back to center, middle leads
        {
            {0.0f, 0.0f, 0.0f},      // Start point
            {-0.2f, 0.0f, 0.0f},     // Straight back
            {-0.25f, -0.05f, 0.03f}, // Middle starts settling
            {-0.35f, -0.1f, 0.04f},  // Middle settling
            {-0.45f, -0.15f, 0.08f}  // Tuft still right (momentum)
        },
        // Variation 28: Middle settling, tuft catching up
        {
            {0.0f, 0.0f, 0.0f},     // Start point
            {-0.2f, 0.0f, 0.0f},    // Straight back
            {-0.25f, -0.05f, 0.0f}, // Middle settling
            {-0.35f, -0.1f, 0.0f},  // Middle settling
            {-0.45f, -0.15f, 0.04f} // Tuft catching up
        },
        // Variation 29: Almost back to start
        {
            {0.0f, 0.0f, 0.0f},     // Start point
            {-0.2f, 0.0f, 0.0f},    // Straight back
            {-0.25f, -0.05f, 0.0f}, // Almost start
            {-0.35f, -0.1f, 0.0f},  // Almost start
            {-0.45f, -0.15f, 0.0f}  // Almost start
        },
        // Variation 30: Back to starting position (connects to variation 1)
        {
            {0.0f, 0.0f, 0.0f},     // Start point
            {-0.2f, 0.0f, 0.0f},    // Straight back
            {-0.25f, -0.05f, 0.0f}, // Slight down
            {-0.35f, -0.1f, 0.0f},  // More down
            {-0.45f, -0.15f, 0.0f}  // End pointing down
        }};

    // Use frame-based animation that changes every timer tick (0.5 seconds)
    int numVariations = sizeof(tailVariations) / sizeof(tailVariations[0]);
    int currentVariation = getAnimationFrame() % numVariations;

    float controlPoints[5][3];

    // Copy the current variation's control points
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            controlPoints[i][j] = tailVariations[currentVariation][i][j];
        }
    }

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

    // Hair tuft at the tip - synchronized with tail variation
    glPushMatrix();
    glTranslatef(controlPoints[4][0], controlPoints[4][1], controlPoints[4][2]);
    glColor3f(0.1f, 0.1f, 0.1f); // Black hair

    // Calculate tangent vector at the end of the tail curve
    float tangent[3];
    float length = calculateTailTangent(controlPoints, tangent);

    // Calculate rotation from tangent vector to align hair tuft
    float angle, axisX, axisY, axisZ;
    calculateHairTuftRotation(tangent, angle, axisX, axisY, axisZ);

    // Apply rotation based on current tail variation
    glRotatef(angle, axisX, axisY, axisZ);

    glScalef(0.04f, 0.06f, 0.04f);
    drawEllipsoid(1.0, 1.0, 1.0);
    glPopMatrix();

    glPopMatrix();
}
