#include "cow/cow.h"
#include "shapes/shapes.h"
#include <GLUT/glut.h>
#include <cmath>
#include <cstdlib>
#include <ctime>

// Helper to draw a spot
void drawSpot(float x, float y, float z, float rx, float ry, float rz)
{
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(rx, ry, rz);
    glColor3f(0.1f, 0.1f, 0.1f); // Black
    glutSolidSphere(1.0, 16, 16);
    glPopMatrix();
}

// Helper to seed RNG only once
void seedSpotsRNG()
{
    static bool seeded = false;
    if (!seeded)
    {
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        seeded = true;
    }
}

// Check if all 8 corners of the spot's bounding box are inside the ellipsoid
bool spotInsideEllipsoid(float x, float y, float z, float rx, float ry, float rz, float a, float b, float c)
{
    for (int dx = -1; dx <= 1; dx += 2)
    {
        for (int dy = -1; dy <= 1; dy += 2)
        {
            for (int dz = -1; dz <= 1; dz += 2)
            {
                float px = x + dx * rx;
                float py = y + dy * ry;
                float pz = z + dz * rz;
                float val = (px / a) * (px / a) + (py / b) * (py / b) + (pz / c) * (pz / c);
                if (val > 1.0f)
                    return false;
            }
        }
    }
    return true;
}

// Helper to draw a projected spot on an ellipsoid surface
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

void drawCow()
{
    glPushMatrix();
    // BODY (barrel-shaped)
    glColor3f(0.95f, 0.95f, 0.95f); // White
    glPushMatrix();
    glScalef(1.2f, 0.6f, 0.4f);
    drawEllipsoid(1.0, 1.0, 1.0);
    glPopMatrix();

    // BODY SPOTS (projected)
    seedSpotsRNG();
    int numSpots = 12;
    float a = 0.9f, b = 0.5f, c = 0.35f; // body ellipsoid radii
    for (int s = 0; s < numSpots; ++s)
    {
        // Random spherical coordinates (full ellipsoid)
        float theta = M_PI * ((std::rand() % 1000) / 1000.0f); // [0, pi]
        float phi = 2.0f * M_PI * ((std::rand() % 1000) / 1000.0f);
        float spotRadius = 0.13f * ((std::rand() % 1000) / 1000.0f) + 0.07f; // [0.07, 0.2] radians
        drawProjectedSpot(a, b, c, spotRadius, theta, phi, 24,
                          [](float x, float y, float z, float &wx, float &wy, float &wz)
                          {
                              // Body is scaled and centered at origin
                              wx = 1.2f * x;
                              wy = 0.6f * y;
                              wz = 0.4f * z;
                          });
    }

    // NECK (short, slightly angled)
    glPushMatrix();
    glTranslatef(1.1f, 0.15f, 0.0f);
    glRotatef(-20, 0, 0, 1);
    glColor3f(0.95f, 0.95f, 0.95f);
    drawCylinder(0.13, 0.11, 0.35);
    // HEAD (distinct ellipsoid)
    glTranslatef(0.0f, 0.35f, 0.0f);
    glRotatef(10, 0, 0, 1);
    glPushMatrix();
    glScalef(0.35f, 0.22f, 0.22f);
    drawEllipsoid(1.0, 1.0, 1.0);
    glPopMatrix();

    // HEAD SPOTS (projected)
    int numHeadSpots = 4;
    float ha = 0.33f, hb = 0.2f, hc = 0.2f; // head ellipsoid radii
    for (int s = 0; s < numHeadSpots; ++s)
    {
        float theta = M_PI * ((std::rand() % 1000) / 1000.0f); // [0, pi]
        float phi = 2.0f * M_PI * ((std::rand() % 1000) / 1000.0f);
        float spotRadius = 0.08f * ((std::rand() % 1000) / 1000.0f) + 0.04f; // [0.04, 0.12] radians
        drawProjectedSpot(ha, hb, hc, spotRadius, theta, phi, 20,
                          [](float x, float y, float z, float &wx, float &wy, float &wz)
                          {
                              // Head is scaled and offset
                              wx = 0.35f * x;
                              wy = 0.22f * y;
                              wz = 0.22f * z;
                          });
    }

    // MUZZLE (protruding ellipsoid)
    glPushMatrix();
    glTranslatef(0.32f, -0.03f, 0.0f);
    glScalef(0.15f, 0.09f, 0.09f);
    glColor3f(0.85f, 0.7f, 0.6f);
    drawEllipsoid(1.0, 1.0, 1.0);
    glPopMatrix();

    // NOSTRILS
    for (int s = -1; s <= 1; s += 2)
    {
        glPushMatrix();
        glTranslatef(0.41f, -0.03f, 0.04f * s);
        glScalef(0.02f, 0.02f, 0.02f);
        glColor3f(0.2f, 0.1f, 0.1f);
        drawSphere(1.0);
        glPopMatrix();
    }

    // EYES
    for (int s = -1; s <= 1; s += 2)
    {
        glPushMatrix();
        glTranslatef(0.18f, 0.07f, 0.09f * s);
        glScalef(0.025f, 0.025f, 0.025f);
        glColor3f(0.1f, 0.1f, 0.1f);
        drawSphere(1.0);
        glPopMatrix();
    }

    // EARS
    for (int s = -1; s <= 1; s += 2)
    {
        glPushMatrix();
        glTranslatef(0.0f, 0.13f, 0.13f * s);
        glRotatef(30 * s, 0, 0, 1);
        glScalef(0.09f, 0.04f, 0.03f);
        glColor3f(0.7f, 0.5f, 0.3f);
        drawEllipsoid(1.0, 1.0, 1.0);
        glPopMatrix();
    }

    // HORNS
    for (int s = -1; s <= 1; s += 2)
    {
        glPushMatrix();
        glTranslatef(0.07f, 0.13f, 0.07f * s);
        glRotatef(30 * s, 0, 0, 1);
        glRotatef(-30, 0, 1, 0);
        glColor3f(0.9f, 0.9f, 0.7f);
        drawCylinder(0.015, 0.01, 0.11);
        glPopMatrix();
    }
    glPopMatrix(); // End head/neck group
    glPopMatrix(); // End neck

    // LEGS (vertical, under body)
    float legX[4] = {0.5f, 0.5f, -0.5f, -0.5f};
    float legZ[4] = {0.18f, -0.18f, 0.18f, -0.18f};
    for (int i = 0; i < 4; ++i)
    {
        glPushMatrix();
        glTranslatef(legX[i], -0.6f, legZ[i]);
        glColor3f(0.95f, 0.95f, 0.95f);
        drawCylinder(0.06, 0.05, 0.5);
        // Hoof
        glTranslatef(0, -0.01f, 0);
        glColor3f(0.2f, 0.1f, 0.1f);
        drawCylinder(0.05, 0.05, 0.08);
        glPopMatrix();
    }

    // TAIL (thin, with tuft)
    glPushMatrix();
    glTranslatef(-1.1f, 0.0f, 0.0f);
    glRotatef(-20, 0, 0, 1);
    glColor3f(0.7f, 0.5f, 0.3f);
    drawCylinder(0.025, 0.015, 0.45);
    // Tail tuft
    glTranslatef(0, -0.05f, 0);
    glColor3f(0.1f, 0.1f, 0.1f);
    drawEllipsoid(0.05, 0.09, 0.05);
    glPopMatrix();

    glPopMatrix();
}