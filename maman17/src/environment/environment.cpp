#include "environment/environment.h"
#include "environment/lights.h"
#include "shapes/shapes.h"
#include "cow/cow.h"
#include "cow/cow_coordinates.h"
#include <GLUT/glut.h>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <cstring>

// Initialize random seed
static bool randomInitialized = false;
static int programStartTime = 0;

/**
 * Initializes the random number generator with a time-based seed
 * for consistent randomization during the program execution
 */
void initializeRandom()
{
    if (!randomInitialized)
    {
        programStartTime = (int)time(nullptr); // Use current time as base seed
        srand(programStartTime);               // Use program start time for general random seed
        randomInitialized = true;
    }
}

// Sets a deterministic random seed based on object position and type for consistent generation.
void setSeedForObject(float x, float y, float z, int objectType)
{
    // Ensure random is initialized
    if (!randomInitialized)
    {
        initializeRandom();
    }

    // Create a unique seed based on position, object type, and program execution time
    int seed = (int)(x * 1000) + (int)(y * 1000) * 1000 + (int)(z * 1000) * 1000000 + objectType * 10000000 + programStartTime;
    srand(abs(seed));
}

// Generates a random floating-point number within the specified range.
float randomFloat(float min, float max)
{
    return min + (float)rand() / RAND_MAX * (max - min);
}

// Simple collision detection function to check if a point is too close to another point.
bool isPositionTooCloseToPoint(float x, float z, float targetX, float targetZ, float minDistance)
{
    float dx = x - targetX;
    float dz = z - targetZ;
    float distanceSquared = dx * dx + dz * dz;
    return distanceSquared < (minDistance * minDistance);
}

// Calculate cow's collision radius based on actual cow dimensions
float getCowCollisionRadius()
{
    // Calculate cow's extent in X direction (length)
    // From tail position to head position, plus some margin
    float lengthX = (COW_HEAD_X - COW_TAIL_X) + 0.5f;

    // Calculate cow's extent in Z direction (width)
    // Body is scaled to 0.6f on a unit ellipsoid (radius 1.0), so body width is 1.2
    // Legs extend to ±0.25, but body is wider
    float widthZ = std::max(1.2f, 2.0f * std::abs(COW_LEG_FRONT_LEFT_Z)) + 0.2f;

    // Use the larger dimension as collision radius for conservative collision detection
    float radius = std::max(lengthX, widthZ) / 2.0f;

    return radius;
}

// Calculate spawn protection radius (larger than collision radius for initial placement)
float getCowSpawnRadius()
{
    return getCowCollisionRadius() + 1.5f;
}

// Calculate clearance radius for dynamic rock placement (smaller than spawn radius)
float getCowClearanceRadius()
{
    return getCowCollisionRadius() + 0.5f;
}

// Check if position is safe for rock placement (not too close to cow spawn or current position)
bool isPositionSafeForRock(float rockX, float rockZ)
{
    // Cow spawn protection - avoid placing rocks near origin (0, 0)
    // Use calculated spawn radius based on cow's actual dimensions
    float spawnRadius = getCowSpawnRadius();
    if (isPositionTooCloseToPoint(rockX, rockZ, 0.0f, 0.0f, spawnRadius))
    {
        return false;
    }

    return true;
}

// Sun position management
static int currentSunPosition = 0; // 0: (-x,y,-z), 1: (-x,y,z), 2: (x,y,-z), 3: (x,y,z)

void setSunPosition(int positionIndex)
{
    if (positionIndex >= 0 && positionIndex < 4)
    {
        currentSunPosition = positionIndex;
    }
}

int getCurrentSunPosition()
{
    return currentSunPosition;
}

// Tree implementation
void drawTrunk(float height, float baseRadius, float topRadius)
{
    // Save current material state to prevent leakage
    glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT);

    setTreeMaterial();

    // Create trunk with texture-like appearance using multiple cylinders
    int segments = 20;
    float segmentHeight = height / segments;

    for (int i = 0; i < segments; i++)
    {
        float t1 = (float)i / segments;
        float t2 = (float)(i + 1) / segments;

        // Interpolate radius
        float r1 = baseRadius + t1 * (topRadius - baseRadius);
        float r2 = baseRadius + t2 * (topRadius - baseRadius);

        // Add slight randomness for bark texture
        float barkVariation = 0.05f * sin(i * 2.0f) * baseRadius;
        r1 += barkVariation;
        r2 += barkVariation;

        glPushMatrix();
        glTranslatef(0.0f, i * segmentHeight, 0.0f);

        // Create a slightly irregular cylinder segment
        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= 16; j++)
        {
            float angle = 2.0f * M_PI * j / 16.0f;
            float x1 = r1 * cos(angle);
            float z1 = r1 * sin(angle);
            float x2 = r2 * cos(angle);
            float z2 = r2 * sin(angle);

            // Add bark texture variation
            float texVariation = 0.02f * sin(angle * 4 + i);
            x1 += texVariation;
            z1 += texVariation;
            x2 += texVariation;
            z2 += texVariation;

            // Normal calculation for lighting
            glNormal3f(x1, 0.0f, z1);
            glVertex3f(x1, 0.0f, z1);
            glNormal3f(x2, 0.0f, z2);
            glVertex3f(x2, segmentHeight, z2);
        }
        glEnd();

        glPopMatrix();
    }

    // Restore previous material state
    glPopAttrib();
}

// Recursive function to draw a branch of a tree
void drawBranch(float length, float radius, int depth, float angleX, float angleY)
{
    if (depth <= 0 || length < 0.1f)
        return;

    glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT);
    setTreeMaterial();

    glPushMatrix();
    glRotatef(angleX, 1.0f, 0.0f, 0.0f);
    glRotatef(angleY, 0.0f, 1.0f, 0.0f);

    drawCylinder(radius, radius * 0.7f, length);

    // Move along the branch's local forward axis (Z) by the full parent length
    // so child branches start at the end of the parent, regardless of rotations
    glTranslatef(0.0f, 0.0f, length);

    if (depth == 1)
    {
        drawLeaves(0.0f, 0.0f, 0.0f, radius * 3.0f);
    }
    else
    {
        // Always create 3 sub-branches positioned orthogonally to avoid same-plane clustering
        float subLength = length * randomFloat(0.6f, 0.8f);
        float subRadius = radius * 0.7f;

        // Branch 1: Forward-left with upward tilt
        drawBranch(subLength, subRadius, depth - 1,
                   randomFloat(-40.0f, -20.0f),  // X: negative for upward tilt
                   randomFloat(-60.0f, -30.0f)); // Y: left direction

        // Branch 2: Forward-right with upward tilt
        drawBranch(subLength, subRadius, depth - 1,
                   randomFloat(-40.0f, -20.0f), // X: negative for upward tilt
                   randomFloat(30.0f, 60.0f));  // Y: right direction

        // Branch 3: More upward with slight random Y rotation
        drawBranch(subLength, subRadius, depth - 1,
                   randomFloat(-80.0f, -50.0f), // X: steeper negative for more upward
                   randomFloat(-20.0f, 20.0f)); // Y: slight random variation
    }

    glPopMatrix();
    glPopAttrib();
}

// Draw leaves as a sphere with a random position and size.
void drawLeaves(float x, float y, float z, float size)
{
    glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT);

    GLfloat matAmbient[] = {0.1f, 0.3f, 0.1f, 1.0f};
    GLfloat matDiffuse[] = {0.2f, 0.8f, 0.2f, 1.0f};
    GLfloat matSpecular[] = {0.1f, 0.2f, 0.1f, 1.0f};
    GLfloat matShininess[] = {30.0f};

    glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);

    int numLeaves = 40 + rand() % 5;
    for (int i = 0; i < numLeaves; i++)
    {
        glPushMatrix();

        float leafX = x + randomFloat(-size, size);
        float leafY = y + randomFloat(-size * 0.5f, size * 0.5f);
        float leafZ = z + randomFloat(-size, size);

        glTranslatef(leafX, leafY, leafZ);

        float colorVar = randomFloat(-0.1f, 0.1f);
        setMaterialFromColor(0.2f + colorVar, 0.8f + colorVar, 0.2f + colorVar, 30.0f, 0.3f);

        glutSolidSphere(size * 0.15f, 6, 6);

        glPopMatrix();
    }

    glPopAttrib();
}

/**
 * Renders a complete procedural tree with semi-realistic branching structure and foliage.
 * Creates a detailed tree model using recursive branch generation, trunk texturing,
 * and distributed leaf clusters. Uses deterministic random generation based on position
 * to ensure consistent tree appearance across renders while providing natural variation.
 */
void drawTree(float x, float y, float z, float scale)
{
    glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT);

    setSeedForObject(x, y, z, 1);

    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(scale, scale, scale);

    float trunkHeight = 3.0f;
    float trunkBaseRadius = 0.3f;
    float trunkTopRadius = 0.2f;

    drawTrunk(trunkHeight, trunkBaseRadius, trunkTopRadius);

    // Create 4 main branches in orthogonal arrangement around trunk top
    float branchLength = randomFloat(1.5f, 2.5f);
    float branchRadius = trunkTopRadius * 0.6f;

    // Branch 1: North direction with upward tilt
    glPushMatrix();
    glTranslatef(0.0f, trunkHeight, 0.0f);
    glRotatef(0.0f, 0.0f, 1.0f, 0.0f);
    glTranslatef(trunkTopRadius, 0.0f, 0.0f);
    drawBranch(branchLength, branchRadius, 3, randomFloat(-45.0f, -25.0f), 0.0f);
    glPopMatrix();

    // Branch 2: East direction with upward tilt
    glPushMatrix();
    glTranslatef(0.0f, trunkHeight, 0.0f);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    glTranslatef(trunkTopRadius, 0.0f, 0.0f);
    drawBranch(branchLength, branchRadius, 3, randomFloat(-45.0f, -25.0f), 0.0f);
    glPopMatrix();

    // Branch 3: South direction with upward tilt
    glPushMatrix();
    glTranslatef(0.0f, trunkHeight, 0.0f);
    glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
    glTranslatef(trunkTopRadius, 0.0f, 0.0f);
    drawBranch(branchLength, branchRadius, 3, randomFloat(-45.0f, -25.0f), 0.0f);
    glPopMatrix();

    // Branch 4: West direction with upward tilt
    glPushMatrix();
    glTranslatef(0.0f, trunkHeight, 0.0f);
    glRotatef(270.0f, 0.0f, 1.0f, 0.0f);
    glTranslatef(trunkTopRadius, 0.0f, 0.0f);
    drawBranch(branchLength, branchRadius, 3, randomFloat(-45.0f, -25.0f), 0.0f);
    glPopMatrix();

    glPopMatrix();
    glPopAttrib();
}

// Renders a single realistic grass blade with natural tapering
void drawGrassBlade(float height, float width, float bend, float colorVariation)
{
    // Save current material state to prevent leakage
    glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT);

    setGrassMaterial(colorVariation);

    // Create a single grass blade using triangular strips
    glBegin(GL_TRIANGLE_STRIP);

    int segments = 5;
    for (int i = 0; i <= segments; i++)
    {
        float t = (float)i / segments;
        float currentHeight = height * t;
        float currentWidth = width * (1.0f - t * 0.7f); // Taper towards top

        // Apply bend (wind effect)
        float bendOffset = bend * t * t;

        // Left side of blade
        glNormal3f(-0.1f, 0.9f, 0.1f);
        glVertex3f(-currentWidth + bendOffset, currentHeight, 0.0f);

        // Right side of blade
        glNormal3f(0.1f, 0.9f, 0.1f);
        glVertex3f(currentWidth + bendOffset, currentHeight, 0.0f);
    }

    glEnd();

    // Restore previous material state
    glPopAttrib();
}

/**
 * Generates a realistic grass meadow using procedural blade placement and variation.
 * Creates {{grassDensity}} amount of individual grass blades with natural randomization
 * in height, width, bend, and color to simulate a living meadow environment. Uses
 * deterministic seeding for consistent generation while maintaining natural organic
 * appearance.
 */
void drawProceduralMeadow(float width, float depth, int grassDensity)
{
    // Save current material state to prevent leakage
    glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT);

    // Set deterministic seed for consistent grass generation
    setSeedForObject(width, depth, grassDensity, 3); // objectType = 3 for grass

    // Draw individual grass blades
    for (int i = 0; i < grassDensity; i++)
    {
        float grassX = randomFloat(-width / 2, width / 2);
        float grassZ = randomFloat(-depth / 2, depth / 2);
        float grassHeight = randomFloat(0.2f, 0.8f);
        float grassWidth = randomFloat(0.02f, 0.05f);
        float grassBend = randomFloat(-0.1f, 0.1f);
        float colorVar = randomFloat(-0.2f, 0.2f);

        glPushMatrix();
        glTranslatef(grassX, 0.0f, grassZ);
        glRotatef(randomFloat(0.0f, 360.0f), 0.0f, 1.0f, 0.0f); // Random rotation

        drawGrassBlade(grassHeight, grassWidth, grassBend, colorVar);

        glPopMatrix();
    }

    // Restore previous material state
    glPopAttrib();
}

// Rock implementation
// Creates a naturally irregular rock shape using vertex distortion and procedural texturing.
void drawIrregularRock(float scale, int complexity, float colorVariation)
{
    // Save current material state to prevent leakage
    glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT);

    setRockMaterial(colorVariation);

    // Create irregular rock using distorted sphere vertices
    glBegin(GL_TRIANGLES);

    // Generate vertices on a sphere and distort them
    int rings = complexity / 2;
    int sectors = complexity;

    for (int r = 0; r < rings; r++)
    {
        for (int s = 0; s < sectors; s++)
        {
            // Current ring
            float theta1 = M_PI * r / rings;
            float theta2 = M_PI * (r + 1) / rings;

            // Current sector
            float phi1 = 2 * M_PI * s / sectors;
            float phi2 = 2 * M_PI * (s + 1) / sectors;

            // Calculate vertices with distortion
            float distortion1 = 1.0f + 0.3f * sin(phi1 * 3) * sin(theta1 * 2);
            float distortion2 = 1.0f + 0.3f * sin(phi2 * 3) * sin(theta1 * 2);
            float distortion3 = 1.0f + 0.3f * sin(phi1 * 3) * sin(theta2 * 2);
            float distortion4 = 1.0f + 0.3f * sin(phi2 * 3) * sin(theta2 * 2);

            // Vertex 1
            float x1 = scale * distortion1 * sin(theta1) * cos(phi1);
            float y1 = scale * distortion1 * cos(theta1);
            float z1 = scale * distortion1 * sin(theta1) * sin(phi1);

            // Vertex 2
            float x2 = scale * distortion2 * sin(theta1) * cos(phi2);
            float y2 = scale * distortion2 * cos(theta1);
            float z2 = scale * distortion2 * sin(theta1) * sin(phi2);

            // Vertex 3
            float x3 = scale * distortion3 * sin(theta2) * cos(phi1);
            float y3 = scale * distortion3 * cos(theta2);
            float z3 = scale * distortion3 * sin(theta2) * sin(phi1);

            // Vertex 4
            float x4 = scale * distortion4 * sin(theta2) * cos(phi2);
            float y4 = scale * distortion4 * cos(theta2);
            float z4 = scale * distortion4 * sin(theta2) * sin(phi2);

            // First triangle (1, 2, 3)
            // Calculate normal
            float nx1 = (y2 - y1) * (z3 - z1) - (z2 - z1) * (y3 - y1);
            float ny1 = (z2 - z1) * (x3 - x1) - (x2 - x1) * (z3 - z1);
            float nz1 = (x2 - x1) * (y3 - y1) - (y2 - y1) * (x3 - x1);
            float len1 = sqrt(nx1 * nx1 + ny1 * ny1 + nz1 * nz1);
            nx1 /= len1;
            ny1 /= len1;
            nz1 /= len1;

            glNormal3f(nx1, ny1, nz1);
            glVertex3f(x1, y1, z1);
            glVertex3f(x2, y2, z2);
            glVertex3f(x3, y3, z3);

            // Second triangle (2, 4, 3)
            float nx2 = (y4 - y2) * (z3 - z2) - (z4 - z2) * (y3 - y2);
            float ny2 = (z4 - z2) * (x3 - x2) - (x4 - x2) * (z3 - z2);
            float nz2 = (x4 - x2) * (y3 - y2) - (y4 - y2) * (x3 - x2);
            float len2 = sqrt(nx2 * nx2 + ny2 * ny2 + nz2 * nz2);
            nx2 /= len2;
            ny2 /= len2;
            nz2 /= len2;

            glNormal3f(nx2, ny2, nz2);
            glVertex3f(x2, y2, z2);
            glVertex3f(x4, y4, z4);
            glVertex3f(x3, y3, z3);
        }
    }

    glEnd();

    // Restore previous material state
    glPopAttrib();
}

// Draws a rock at the given position with the given scale and color variation.
void drawRock(float x, float y, float z, float scale, float colorVariation)
{
    // Save current material state to prevent leakage
    glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT);

    // Set deterministic seed based on rock position
    setSeedForObject(x, y, z, 2); // objectType = 2 for rocks

    glPushMatrix();
    glTranslatef(x, y, z);

    // Add some random rotation for natural placement
    glRotatef(randomFloat(0.0f, 360.0f), 0.0f, 1.0f, 0.0f);
    glRotatef(randomFloat(-10.0f, 10.0f), 1.0f, 0.0f, 0.0f);

    // Scale with slight random variation
    float scaleVar = scale * randomFloat(0.8f, 1.2f);
    glScalef(scaleVar, scaleVar * 0.7f, scaleVar); // Slightly flattened

    drawIrregularRock(1.0f, 24, colorVariation);

    glPopMatrix();

    // Restore previous material state
    glPopAttrib();
}

/**
 * Places {{numRocks}} amount of rock formations throughout the scene using collision
 * avoidance. Generates rocks with procedural distortion and positioning, while
 * avoiding placement near the cow's spawn area and other obstacles.
 */
void drawScatteredRocks(float x, float y, float z, int numRocks)
{
    // Save current material state to prevent leakage
    glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT);

    // Set deterministic seed for consistent rock generation
    setSeedForObject(x, y, z, 2); // objectType = 2 for rocks

    // Draw individual rocks
    for (int i = 0; i < numRocks; i++)
    {
        float rockX, rockZ;
        int attempts = 0;
        const int MAX_ATTEMPTS = 50; // Prevent infinite loops

        // Generate rock position, avoiding the cow's spawn area and current position
        do
        {
            // Generate rock positions so that both rockX and rockZ can be positive or negative independently,
            // and so that rocks are distributed across the full area centered at (0,0).
            // Use the absolute values of x and z to define the extents, so sign of x/z doesn't affect the range.
            float halfX = std::abs(x) / 2.5f;
            float halfZ = std::abs(z) / 2.5f;
            rockX = randomFloat(-halfX, halfX);
            rockZ = randomFloat(-halfZ, halfZ);
            attempts++;
        } while (!isPositionSafeForRock(rockX, rockZ) && attempts < MAX_ATTEMPTS);

        // If we couldn't find a safe position after many attempts, skip this rock
        if (attempts >= MAX_ATTEMPTS)
        {
            std::cout << "Warning: Could not find safe position for rock " << i << ", skipping." << std::endl;
            continue;
        }

        float rockScale = randomFloat(0.5f, 1.5f);
        float colorVar = randomFloat(-0.2f, 0.2f);

        glPushMatrix();
        glTranslatef(rockX, 0.0f, rockZ);
        glRotatef(randomFloat(0.0f, 360.0f), 0.0f, 1.0f, 0.0f); // Random rotation

        drawIrregularRock(rockScale, 24, colorVar);

        glPopMatrix();
    }

    // Restore previous material state
    glPopAttrib();
}

float benchHalfWidth = 0.0f;
float benchHalfDepth = 0.0f;

// Metal bench implementation (at origin, no rotation)
void drawMetalBench(float x, float y, float z, float scale, float rotateY)
{
    // Save current material state to prevent leakage
    glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT);

    glPushMatrix();
    glScalef(scale, scale, scale);

    setMetalMaterial();

    // Bench proportions
    const float seatWidth = 2.0f;
    const float seatDepth = 0.5f;
    const float seatYPos = 0.4f; // Seat top height (also leg height)
    const float legRadius = 0.06f;
    const float railRadius = 0.04f;
    const float slatRadius = 0.03f;
    const int seatSlatCount = 7;
    const int backSlatCount = 6;
    const float backrestHeight = 0.7f;
    const float backTiltDegrees = 12.0f; // Slight lean for comfort

    // Convenience offsets
    benchHalfWidth = seatWidth * 0.5f;
    benchHalfDepth = seatDepth * 0.5f;
    // Calculate actual seat slat edge coordinates
    // Seat slats span X from -seatWidth/2 to +seatWidth/2
    const float slatXLeft = -benchHalfWidth; // -1.0
    const float slatXRight = benchHalfWidth; // +1.0

    // Seat slats Z positions: zPos = -halfD + (0.15f * seatDepth) + t * (seatDepth - 0.30f * seatDepth)
    // Front slat (t=1): -0.25 + 0.075 + 1.0 * 0.35 = 0.175
    // Back slat (t=0): -0.25 + 0.075 + 0.0 * 0.35 = -0.175
    const float slatZFront = -benchHalfDepth + (0.15f * seatDepth) + 1.0f * (seatDepth - 0.30f * seatDepth); // 0.175
    const float slatZBack = -benchHalfDepth + (0.15f * seatDepth) + 0.0f * (seatDepth - 0.30f * seatDepth);  // -0.175

    // Position legs at the seat slat edges
    const float legXLeft = slatXLeft;
    const float legXRight = slatXRight;
    const float legZFront = slatZFront;
    const float legZBack = slatZBack;

    // Legs: four tubular posts (vertical)
    auto drawVerticalPost = [&](float px, float pz, float height, float radius)
    {
        glPushMatrix();
        glTranslatef(px, 0.0f, pz);
        // Align cylinder axis (Z) to +Y by rotating -90° about X
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        // Cylinder draws from origin in +Z, which after rotation is +Y
        // So the cylinder base is at y=0, which is what we want
        drawCylinder(radius, radius, height);
        glPopMatrix();
    };

    // Front-left, front-right, back-left, back-right
    drawVerticalPost(legXLeft, legZFront, seatYPos, legRadius);
    drawVerticalPost(legXRight, legZFront, seatYPos, legRadius);
    drawVerticalPost(legXLeft, legZBack, seatYPos, legRadius);
    drawVerticalPost(legXRight, legZBack, seatYPos, legRadius);

    // Seat rails: two horizontal tubes connecting left-right at front and back
    auto drawHorizontalRailX = [&](float py, float pz, float length, float radius)
    {
        glPushMatrix();
        glTranslatef(-length * 0.5f, py, pz);
        // Align cylinder axis (Z) to X by rotating +90° about Y
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        drawCylinder(radius, radius, length);
        glPopMatrix();
    };

    drawHorizontalRailX(seatYPos - slatRadius * 1.2f, legZFront, seatWidth, railRadius);
    drawHorizontalRailX(seatYPos - slatRadius * 1.2f, legZBack, seatWidth, railRadius);

    // Seat side rails along depth (Z) to connect front and back legs
    auto drawHorizontalRailZ = [&](float px, float py, float length, float radius)
    {
        glPushMatrix();
        glTranslatef(px, py, 0.0f);
        // Axis is already Z; center the rail on Z
        glTranslatef(0.0f, 0.0f, -length * 0.5f);
        drawCylinder(radius, radius, length);
        glPopMatrix();
    };

    // Z-rails connect front and back legs at their actual positions
    float actualSeatDepth = slatZFront - slatZBack; // 0.35
    drawHorizontalRailZ(legXLeft, seatYPos - slatRadius * 1.3f, actualSeatDepth, railRadius);
    drawHorizontalRailZ(legXRight, seatYPos - slatRadius * 1.3f, actualSeatDepth, railRadius);

    // Seat slats: several rounded tubes spanning left-right, distributed along depth
    for (int i = 0; i < seatSlatCount; ++i)
    {
        float t = (seatSlatCount == 1) ? 0.5f : (float)i / (seatSlatCount - 1);
        float zPos = -benchHalfDepth + (0.15f * seatDepth) + t * (seatDepth - 0.30f * seatDepth); // inset a bit from edges

        glPushMatrix();
        glTranslatef(-benchHalfWidth, seatYPos, zPos);
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f); // make axis X
        drawCylinder(slatRadius, slatRadius, seatWidth);
        glPopMatrix();
    }

    // Backrest frame: two vertical posts from back legs up to backrest height
    drawVerticalPost(legXLeft, legZBack, seatYPos + backrestHeight + 0.05f, legRadius * 0.9f);
    drawVerticalPost(legXRight, legZBack, seatYPos + backrestHeight + 0.05f, legRadius * 0.9f);

    // Backrest slats: horizontal tubes across width, tilted back slightly
    for (int i = 0; i < backSlatCount; ++i)
    {
        float t = (backSlatCount == 1) ? 0.5f : (float)i / (backSlatCount - 1);
        float yPos = seatYPos + 0.10f + t * (backrestHeight - 0.20f); // leave small margins top/bottom

        glPushMatrix();
        // Position roughly at the back plane, then tilt about X to lean back
        glTranslatef(-benchHalfWidth, yPos, legZBack);
        glRotatef(-backTiltDegrees, 1.0f, 0.0f, 0.0f);
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f); // axis along X
        drawCylinder(slatRadius, slatRadius, seatWidth);
        glPopMatrix();
    }

    // Top backrest rail across width (tilted to match backrest)
    glPushMatrix();
    glTranslatef(-benchHalfWidth, seatYPos + backrestHeight, legZBack);
    glRotatef(-backTiltDegrees, 1.0f, 0.0f, 0.0f);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    drawCylinder(railRadius, railRadius, seatWidth);
    glPopMatrix();

    // Lower braces near ground for stability
    // Front X brace
    drawHorizontalRailX(legRadius * 1.2f, legZFront, seatWidth, railRadius * 0.8f);
    // Back X brace
    drawHorizontalRailX(legRadius * 1.2f, legZBack, seatWidth, railRadius * 0.8f);
    // Left Z brace
    drawHorizontalRailZ(legXLeft, legRadius * 1.2f, actualSeatDepth, railRadius * 0.8f);
    // Right Z brace
    drawHorizontalRailZ(legXRight, legRadius * 1.2f, actualSeatDepth, railRadius * 0.8f);

    glPopMatrix();

    // Restore previous material state
    glPopAttrib();
}

void drawTexturedGroundPatch(float centerX, float centerZ, float size, unsigned int textureID)
{
    // Save current material state to prevent leakage
    glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glPushMatrix();
    glTranslatef(centerX, 0.01f, centerZ); // Slightly above ground to avoid z-fighting

    // Draw a textured quad
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f); // Point up

    // Bottom-left
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-size / 2, 0.0f, -size / 2);

    // Bottom-right
    glTexCoord2f(2.0f, 0.0f); // Repeat texture 2x for tiling
    glVertex3f(size / 2, 0.0f, -size / 2);

    // Top-right
    glTexCoord2f(2.0f, 2.0f);
    glVertex3f(size / 2, 0.0f, size / 2);

    // Top-left
    glTexCoord2f(0.0f, 2.0f);
    glVertex3f(-size / 2, 0.0f, size / 2);

    glEnd();
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);

    // Restore previous material state
    glPopAttrib();
}

// World-wide ground texture
/**
 * Renders the base textured ground plane that forms the foundation of the entire scene.
 * Applies repeating dirt texture across a large area to provide realistic terrain
 * appearance, using proper texture coordinate scaling for seamless tiling and
 * consistent visual quality regardless of world size.
 */
void drawWorldGround(float worldSize)
{
    // Save current material state to prevent leakage
    glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT);

    // Set bright material for ground to show texture clearly
    GLfloat matFullAmbientOrDiffuse[] = {0.0f, 0.0f, 0.0f, 1.0f}; // Full ambient
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matFullAmbientOrDiffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matFullAmbientOrDiffuse);
    glColor3f(1.0f, 1.0f, 1.0f); // Ensure white color

    static unsigned int worldGroundTextureID = 0;
    if (worldGroundTextureID == 0)
    {
        worldGroundTextureID = loadTexture("textures/dirt_texture.png");
    }

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, worldGroundTextureID);

    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f); // Slightly below ground level

    // Calculate texture repeat count for 1:1 mapping
    // The texture is 1024x1024 pixels and represents 1 world unit
    // For a worldSize of 100, we need 100 texture repeats
    float textureRepeats = worldSize;

    // Draw a large textured quad covering the entire world
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f); // Point up

    // Bottom-left
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-worldSize / 2, 0.0f, -worldSize / 2);

    // Bottom-right
    glTexCoord2f(textureRepeats, 0.0f);
    glVertex3f(worldSize / 2, 0.0f, -worldSize / 2);

    // Top-right
    glTexCoord2f(textureRepeats, textureRepeats);
    glVertex3f(worldSize / 2, 0.0f, worldSize / 2);

    // Top-left
    glTexCoord2f(0.0f, textureRepeats);
    glVertex3f(-worldSize / 2, 0.0f, worldSize / 2);

    glEnd();
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);

    // Restore previous material state
    glPopAttrib();
}

/**
 * Renders the sun as a glowing sphere positioned according to current lighting settings.
 * Coordinates with the lighting system to place the visual sun representation at the
 * same location as the light source, creating visual consistency between illumination
 * and the apparent light source.
 */
void drawSun()
{
    float sunX, sunY, sunZ;

    // Whether to use the custom position or the predefined positions
    if (getUseCustomPosition())
    {
        // Use custom adjustable position
        sunX = getLightPositionX();
        sunY = getLightPositionY();
        sunZ = getLightPositionZ();
    }
    else
    {
        // Use predefined positions from 1-4 keys
        int currentPos = getCurrentSunPosition();
        float *sunPositions = getSunPositions();
        if (currentPos >= 0 && currentPos < 4)
        {
            sunX = sunPositions[currentPos * 3 + 0];
            sunY = sunPositions[currentPos * 3 + 1];
            sunZ = sunPositions[currentPos * 3 + 2];
        }
        else
        {
            // Default to position 0 if invalid
            sunX = sunPositions[0];
            sunY = sunPositions[1];
            sunZ = sunPositions[2];
        }
    }

    glPushMatrix();
    glTranslatef(sunX, sunY, sunZ);

    // Disable lighting for the sun itself so it appears bright
    glDisable(GL_LIGHTING);

    // Set bright yellow color for the sun
    glColor3f(1.0f, 1.0f, 0.3f);

    // Draw sun as a sphere
    glutSolidSphere(2.0f, 20, 20);

    // Re-enable lighting for other objects
    glEnable(GL_LIGHTING);

    glPopMatrix();
}

// Renders an illuminated lamp post with adjustable direction and integrated lighting effects
void drawLampPost(float benchX, float benchY, float benchZ, float scale, float benchRotation)
{
    // Setup lamp post lighting
    setupLampLighting();

    glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT);
    glPushMatrix();

    float lampHeight = getLampHeight();
    glScalef(scale, scale, scale);

    // Draw lamp post pole
    setMaterialFromColor(0.2f, 0.2f, 0.2f, 10.0f, 0.3f); // Dark metallic material
    glPushMatrix();
    glTranslatef(0.0f, lampHeight * 0.5f, 0.0f);
    glScalef(0.05f, lampHeight, 0.05f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Draw lamp head (bulb housing)
    setMaterialFromColor(0.3f, 0.3f, 0.3f, 20.0f, 0.5f);
    glPushMatrix();
    glTranslatef(0.0f, lampHeight - 0.2f, 0.0f);

    // Rotate lamp head based on direction
    glRotatef(getLampDirectionZ(), 0.0f, 1.0f, 0.0f); // Yaw rotation
    glRotatef(getLampDirectionX(), 1.0f, 0.0f, 0.0f); // Pitch rotation

    glScalef(0.3f, 0.2f, 0.3f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Draw bright bulb inside lamp head
    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 1.0f, 0.7f); // Warm white/yellow
    glPushMatrix();
    glTranslatef(0.0f, lampHeight - 0.2f, 0.0f);

    // Rotate bulb with lamp head
    glRotatef(getLampDirectionZ(), 0.0f, 1.0f, 0.0f);
    glRotatef(getLampDirectionX(), 1.0f, 0.0f, 0.0f);

    glutSolidSphere(0.08f, 10, 10);
    glPopMatrix();
    glEnable(GL_LIGHTING);

    glPopMatrix();
    glPopAttrib();
}

// Wrapper function that handles positioning and rotation for both bench and lamp
/**
 * Creates a complete outdoor furniture scene with metal bench and illuminated lamp post.
 * Positions and orients both elements as a cohesive unit, with the lamp strategically
 * placed to provide scene lighting. Handles world transformation and coordinates
 * between the furniture and lighting systems for realistic environmental props.
 */
void drawMetalBenchAndLamp(float x, float y, float z, float scale, float rotateY)
{
    glPushMatrix();

    // Apply world transformation (translation and rotation)
    glTranslatef(x, y, z);
    glRotatef(rotateY, 0.0f, 1.0f, 0.0f);

    // Draw the bench at origin
    drawMetalBench(0.0f, 0.0f, 0.0f, scale, 0.0f);

    // Calculate lamp position relative to bench
    // The lamp should be positioned behind the bench backrest
    float seatWidth = 2.0f * scale; // Bench dimensions
    float seatDepth = 0.5f * scale;
    float lampOffsetX = seatWidth * 0.5f + 0.175f * scale; // Behind and to the side
    float lampOffsetZ = -seatDepth * 0.5f;                 // At the back of the bench

    glPushMatrix();
    glTranslatef(lampOffsetX, 0.0f, lampOffsetZ);
    drawLampPost(0.0f, 0.0f, 0.0f, scale, 0.0f);
    glPopMatrix();

    glPopMatrix();
}