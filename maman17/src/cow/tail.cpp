#include "cow/tail.h"
#include "cow/cow_coordinates.h"
#include "cow/cow.h"
#include "shapes/shapes.h"
#include "common/animation.h"
#include "environment/environment.h"
#include "environment/lights.h"
#include <GLUT/glut.h>
#include <cmath>
#include <iostream>

// Constants
const int NUM_TAIL_VARIATIONS = 30;
const int NUM_CONTROL_POINTS = 5;
const int NUM_COORDS = 3;
const int TAIL_SEGMENTS = 20;
const int RADIAL_SEGMENTS = 8;
const float BASE_RADIUS = 0.04f;
const float TAPER_FACTOR = 0.8f;
const float HAIR_TUFT_SCALE_X = 0.04f;
const float HAIR_TUFT_SCALE_Y = 0.06f;
const float HAIR_TUFT_SCALE_Z = 0.04f;

// Tail variations data - 30 different positions for realistic wavy animation
const float TAIL_VARIATIONS[NUM_TAIL_VARIATIONS][NUM_CONTROL_POINTS][NUM_COORDS] = {
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
    // Variation 7: Middle at the center, tuft catching up
    {
        {0.0f, 0.0f, 0.0f},      // Start point
        {-0.2f, 0.0f, 0.0f},     // Straight back
        {-0.25f, -0.05f, 0.0f},  // Middle center
        {-0.35f, -0.1f, 0.0f},   // Middle center
        {-0.45f, -0.15f, -0.04f} // Tuft still left but catching up
    },
    // Variation 8: Middle to the right, tuft at the center
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

// Helper function to calculate tangent vector at the end of the tail curve for the tuft rotation.
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

// Helper function to calculate rotation from tangent vector to align hair tuft
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

        // If the cross product points in the negative Z direction, reverse the angle
        if (axisZ < 0)
        {
            angle = -angle;
        }
    }
    else
    {
        // In case the vectors are parallel (tangent points up or down), no rotation is needed.
        angle = 0.0f;
        axisX = 1.0f;
        axisY = 0.0f;
        axisZ = 0.0f;
    }
}

// Helper function to calculate position on spline using 4th degree polynomial
void calculateSplinePosition(float t, float controlPoints[5][3], float pos[3])
{
    // 4th degree polynomial: P(t) = (1-t)⁴P₀ + 4(1-t)³tP₁ + 6(1-t)²t²P₂ + 4(1-t)t³P₃ + t⁴P₄
    float t_4 = t * t * t * t;
    float t_3 = t * t * t;
    float t_2 = t * t;
    float t_1 = t;
    float t_0 = 1.0f;

    for (int j = 0; j < 3; j++)
    {
        pos[j] = (1 - t_1) * (1 - t_1) * (1 - t_1) * (1 - t_1) * controlPoints[0][j] +
                 4 * (1 - t_1) * (1 - t_1) * (1 - t_1) * t_1 * controlPoints[1][j] +
                 6 * (1 - t_1) * (1 - t_1) * t_2 * controlPoints[2][j] +
                 4 * (1 - t_1) * t_3 * controlPoints[3][j] +
                 t_4 * controlPoints[4][j];
    }
}

// Helper function to calculate perpendicular vectors for cone cross-section
void calculatePerpendicularVectors(float dx, float dy, float dz, float right[3], float forward[3])
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

// Helper function to draw a single cone segment
void drawConeSegment(float pos1[3], float pos2[3], float radius1, float radius2, float right[3], float forward[3])
{
    // Create triangles around the cone circumference
    for (int k = 0; k < RADIAL_SEGMENTS; k++)
    {
        float angle1 = 2.0f * M_PI * k / RADIAL_SEGMENTS;
        float angle2 = 2.0f * M_PI * (k + 1) / RADIAL_SEGMENTS;

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

/**
 * Helper function to draw the tail cone surface.
 * Creates the main tail body using curved cone segments between control points.
 * Generates a smooth, tapered tail shape with proper radius progression from base to tip using interpolated cone segments.
 */
void drawTailCone(float controlPoints[5][3])
{
    glBegin(GL_TRIANGLES);

    // Generate cone surface following the spline
    for (int i = 0; i < TAIL_SEGMENTS; i++)
    {
        float t1 = (float)i / TAIL_SEGMENTS;
        float t2 = (float)(i + 1) / TAIL_SEGMENTS;

        // Calculate current and next positions on spline
        float pos1[3], pos2[3];
        calculateSplinePosition(t1, controlPoints, pos1);
        calculateSplinePosition(t2, controlPoints, pos2);

        // Calculate radii (taper from base to tip)
        float radius1 = BASE_RADIUS * (1.0f - t1 * TAPER_FACTOR);
        float radius2 = BASE_RADIUS * (1.0f - t2 * TAPER_FACTOR);

        // Calculate direction for proper orientation
        float dx = pos2[0] - pos1[0];
        float dy = pos2[1] - pos1[1];
        float dz = pos2[2] - pos1[2];
        float len = sqrt(dx * dx + dy * dy + dz * dz);

        if (len > 0.001f)
        {
            // Normalize direction
            dx /= len;
            dy /= len;
            dz /= len;

            // Calculate perpendicular vectors for cone cross-section
            float right[3], forward[3];
            calculatePerpendicularVectors(dx, dy, dz, right, forward);

            // Draw the cone segment
            drawConeSegment(pos1, pos2, radius1, radius2, right, forward);
        }
    }

    glEnd();
}

// Helper function to draw the hair tuft at the tail tip
void drawHairTuft(float controlPoints[5][3])
{
    glPushMatrix();
    glTranslatef(controlPoints[4][0], controlPoints[4][1], controlPoints[4][2]);
    setCowBlackMaterial();

    // Calculate tangent vector at the end of the tail curve
    float tangent[3];
    // mutates the tangent variable
    calculateTailTangent(controlPoints, tangent);

    // Calculate rotation from tangent vector to align hair tuft
    float angle, axisX, axisY, axisZ;
    // mutates the angle, axisX, axisY, axisZ variables
    calculateHairTuftRotation(tangent, angle, axisX, axisY, axisZ);

    // Apply rotation based on current tail variation
    glRotatef(angle, axisX, axisY, axisZ);

    glScalef(HAIR_TUFT_SCALE_X, HAIR_TUFT_SCALE_Y, HAIR_TUFT_SCALE_Z);
    drawEllipsoid(1.0, 1.0, 1.0);
    glPopMatrix();
}

// Helper function to get current tail variation control points
void getCurrentTailVariation(float controlPoints[5][3])
{
    // Slow down the animation by dividing by 4 to make the movement more realistic
    int currentVariation = (getAnimationFrame() / 4) % NUM_TAIL_VARIATIONS;

    // Mutate the controlPoints variable
    for (int i = 0; i < NUM_CONTROL_POINTS; i++)
    {
        for (int j = 0; j < NUM_COORDS; j++)
        {
            controlPoints[i][j] = TAIL_VARIATIONS[currentVariation][i][j];
        }
    }
}

/**
 * Renders the animated cow tail with procedural motion and user controls.
 * Combines automatic swishing animation with user rotation controls to create realistic tail movement.
 * Includes both the main tail body and hair tuft.
 */
void drawTail()
{
    // Setup tail position and color
    glPushMatrix();
    glTranslatef(COW_TAIL_X, COW_TAIL_Y, COW_TAIL_Z);

    // Apply user-controlled tail rotations
    glRotatef(getTailRotationZ(), 0.0f, 0.0f, 1.0f); // SHIFT+I/K rotation around Z-axis
    glRotatef(getTailRotationY(), 0.0f, 1.0f, 0.0f); // SHIFT+J/L rotation around Y-axis

    // Paint the tail in light gray
    setCowLightGrayMaterial();

    // Get current tail variation control points for animation
    float controlPoints[NUM_CONTROL_POINTS][NUM_COORDS];
    getCurrentTailVariation(controlPoints);

    // Draw the tail cone surface
    drawTailCone(controlPoints);

    // Draw the hair tuft at the tip
    drawHairTuft(controlPoints);

    glPopMatrix();
}
