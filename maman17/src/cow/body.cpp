#define GL_SILENCE_DEPRECATION // Silence deprecation warnings on macOS
#include "cow/body.h"
#include "cow/spots.h"
#include "shapes/shapes.h"
#include <GLUT/glut.h>
#include <cmath>
#include <cstdlib>
#include <ctime>

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

void drawBody()
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
    glPopMatrix();
}