#ifndef SPOTS_H
#define SPOTS_H

#include <GLUT/glut.h>
#include <cmath>

// Helper function to draw a simple spot
void drawSpot(float x, float y, float z, float rx, float ry, float rz);

// Helper function to draw a projected spot on an ellipsoid surface
template <typename F>
void drawProjectedSpot(
    float a, float b, float c,          // ellipsoid radii
    float spotRadius,                   // spot radius (in radians, small)
    float centerTheta, float centerPhi, // spot center (spherical coords)
    int numSegments,                    // spot smoothness
    F ellipsoidTransform                // function to transform (x, y, z) to world coords
)
{
    glColor3f(0.1f, 0.1f, 0.1f); // Black
    glBegin(GL_TRIANGLE_FAN);
    // Center point
    float cx = a * sinf(centerTheta) * cosf(centerPhi);
    float cy = b * sinf(centerTheta) * sinf(centerPhi);
    float cz = c * cosf(centerTheta);
    float wx, wy, wz;
    ellipsoidTransform(cx, cy, cz, wx, wy, wz);
    glVertex3f(wx, wy, wz);
    // Boundary points
    for (int i = 0; i <= numSegments; ++i)
    {
        float angle = 2.0f * M_PI * i / numSegments;
        // Offset in tangent plane (approximate small circle on ellipsoid)
        float dTheta = spotRadius * cosf(angle);
        float dPhi = spotRadius * sinf(angle) / sinf(centerTheta == 0 ? 0.0001f : centerTheta);
        float theta = centerTheta + dTheta;
        float phi = centerPhi + dPhi;
        float x = a * sinf(theta) * cosf(phi);
        float y = b * sinf(theta) * sinf(phi);
        float z = c * cosf(theta);
        ellipsoidTransform(x, y, z, wx, wy, wz);
        glVertex3f(wx, wy, wz);
    }
    glEnd();
}

#endif // SPOTS_H