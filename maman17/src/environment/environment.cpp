#include "environment/environment.h"
#include "environment/lights.h"
#include "shapes/shapes.h"
#ifdef COW_CONTROLS_AVAILABLE
#include "cow/cow.h"
#include "cow/cow_coordinates.h"
#endif
#include <GLUT/glut.h>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <cstring>

// Initialize random seed
static bool randomInitialized = false;
static int programStartTime = 0;

void initializeRandom()
{
    if (!randomInitialized)
    {
        programStartTime = (int)time(nullptr); // Use current time as base seed
        srand(programStartTime);               // Use program start time for general random seed
        randomInitialized = true;
    }
}

// Set a specific seed for deterministic generation based on position/id
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

// Random number generation utilities
float randomFloat(float min, float max)
{
    return min + (float)rand() / RAND_MAX * (max - min);
}

// Simple collision detection function to check if a point is too close to another point
bool isPositionTooCloseToPoint(float x, float z, float targetX, float targetZ, float minDistance)
{
    float dx = x - targetX;
    float dz = z - targetZ;
    float distanceSquared = dx * dx + dz * dz;
    return distanceSquared < (minDistance * minDistance);
}

#ifdef COW_CONTROLS_AVAILABLE
// Calculate cow's collision radius based on actual cow dimensions
float getCowCollisionRadius()
{
    // Calculate cow's extent in X direction (length)
    // From tail position to head position, plus some margin for actual geometry
    float lengthX = (COW_HEAD_X - COW_TAIL_X) + 0.5f; // Add 0.5 units margin for head/tail geometry

    // Calculate cow's extent in Z direction (width)
    // Body is scaled to 0.6f on a unit ellipsoid (radius 1.0), so body width is 1.2
    // Legs extend to ±0.25, but body is wider
    float widthZ = std::max(1.2f, 2.0f * std::abs(COW_LEG_FRONT_LEFT_Z)) + 0.2f; // Add 0.2 units margin

    // Use the larger dimension as collision radius for conservative collision detection
    float radius = std::max(lengthX, widthZ) / 2.0f;

    return radius;
}

// Calculate spawn protection radius (larger than collision radius for initial placement)
float getCowSpawnRadius()
{
    return getCowCollisionRadius() + 1.5f; // Extra clearance for spawn area
}

// Calculate clearance radius for dynamic rock placement (smaller than spawn radius)
float getCowClearanceRadius()
{
    return getCowCollisionRadius() + 0.5f; // Some clearance but not as much as spawn
}
#else
// Fallback functions when cow module is not available - use default values
float getCowCollisionRadius()
{
    return 1.5f; // Default conservative cow collision radius
}

float getCowSpawnRadius()
{
    return 3.0f; // Default spawn protection radius
}

float getCowClearanceRadius()
{
    return 2.5f; // Default clearance radius
}
#endif

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

void handleSunControls(unsigned char key, int x, int y)
{
    switch (key)
    {
    case '1':
        setSunPosition(0); // (-x,y,-z)
        break;
    case '2':
        setSunPosition(1); // (-x,y,z)
        break;
    case '3':
        setSunPosition(2); // (x,y,-z)
        break;
    case '4':
        setSunPosition(3); // (x,y,z)
        break;
    }
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

void drawBranch(float length, float radius, int depth, float angleX, float angleY)
{
    if (depth <= 0 || length < 0.1f)
        return;

    // Save current material state to prevent leakage
    glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT);

    setTreeMaterial();

    // Draw branch segment
    glPushMatrix();
    glRotatef(angleX, 1.0f, 0.0f, 0.0f);
    glRotatef(angleY, 0.0f, 1.0f, 0.0f);

    // Draw the branch cylinder
    drawCylinder(radius, radius * 0.7f, length);

    // Move to end of branch
    glTranslatef(0.0f, length, 0.0f);

    // Add leaves at branch endpoints
    if (depth == 1)
    {
        drawLeaves(0.0f, 0.0f, 0.0f, radius * 3.0f);
    }
    else
    {
        // Create sub-branches
        int numSubBranches = 2 + rand() % 3;
        for (int i = 0; i < numSubBranches; i++)
        {
            float subAngleX = randomFloat(-45.0f, 45.0f);
            float subAngleY = randomFloat(0.0f, 360.0f);
            float subLength = length * randomFloat(0.6f, 0.8f);
            float subRadius = radius * 0.7f;

            drawBranch(subLength, subRadius, depth - 1, subAngleX, subAngleY);
        }
    }

    glPopMatrix();

    // Restore previous material state
    glPopAttrib();
}

void drawLeaves(float x, float y, float z, float size)
{
    // Save current material state to prevent leakage
    glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT);

    // Set leaf material (green)
    GLfloat matAmbient[] = {0.1f, 0.3f, 0.1f, 1.0f};
    GLfloat matDiffuse[] = {0.2f, 0.8f, 0.2f, 1.0f};
    GLfloat matSpecular[] = {0.1f, 0.2f, 0.1f, 1.0f};
    GLfloat matShininess[] = {30.0f};

    glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);

    // Create cluster of leaves using small spheres
    int numLeaves = 20 + rand() % 15;
    for (int i = 0; i < numLeaves; i++)
    {
        glPushMatrix();

        // Random position around center
        float leafX = x + randomFloat(-size, size);
        float leafY = y + randomFloat(-size * 0.5f, size * 0.5f);
        float leafZ = z + randomFloat(-size, size);

        glTranslatef(leafX, leafY, leafZ);

        // Vary leaf color slightly with proper material properties
        float colorVar = randomFloat(-0.1f, 0.1f);
        setMaterialFromColor(0.2f + colorVar, 0.8f + colorVar, 0.2f + colorVar, 30.0f, 0.3f);

        // Small leaf sphere
        glutSolidSphere(size * 0.15f, 6, 6);

        glPopMatrix();
    }

    // Restore previous material state
    glPopAttrib();
}

void drawTree(float x, float y, float z, float scale)
{
    // Save current material state to prevent leakage
    glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT);

    // Set deterministic seed based on tree position
    setSeedForObject(x, y, z, 1); // objectType = 1 for trees

    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(scale, scale, scale);

    // Draw trunk
    float trunkHeight = 3.0f;
    float trunkBaseRadius = 0.3f;
    float trunkTopRadius = 0.2f;

    drawTrunk(trunkHeight, trunkBaseRadius, trunkTopRadius);

    // Draw main branches from top of trunk
    glPushMatrix();
    glTranslatef(0.0f, trunkHeight, 0.0f);

    int numMainBranches = 4 + rand() % 3;
    for (int i = 0; i < numMainBranches; i++)
    {
        float angleX = randomFloat(15.0f, 60.0f);
        float angleY = (360.0f / numMainBranches) * i + randomFloat(-30.0f, 30.0f);
        float branchLength = randomFloat(1.5f, 2.5f);
        float branchRadius = trunkTopRadius * 0.6f;

        drawBranch(branchLength, branchRadius, 3, angleX, angleY);
    }

    glPopMatrix();
    glPopMatrix();

    // Restore previous material state
    glPopAttrib();
}

// Enhanced meadow implementation
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
        float bendOffset = bend * t * t; // Quadratic bend

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

void drawProceduralMeadow(float width, float depth, int grassDensity)
{
    // Save current material state to prevent leakage
    glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT);

    // Set deterministic seed for consistent grass generation
    setSeedForObject(width, depth, grassDensity, 3); // objectType = 3 for grass

    // Draw individual grass blades (no ground base)
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

// Metal bench implementation
void drawMetalBench(float x, float y, float z, float scale)
{
    // Save current material state to prevent leakage
    glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT);

    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(scale, scale, scale);

    setMetalMaterial();

    // Bench dimensions
    float seatWidth = 2.0f;
    float seatDepth = 0.5f;
    float seatHeight = 0.1f;
    float seatYPos = 0.4f;

    float backrestWidth = 2.0f;
    float backrestHeight = 0.8f;
    float backrestThickness = 0.1f;
    float backrestYPos = seatYPos + seatHeight + backrestHeight / 2;

    float legWidth = 0.1f;
    float legDepth = 0.1f;
    float legHeight = 0.4f;

    // Draw seat (horizontal rectangle)
    glPushMatrix();
    glTranslatef(0.0f, seatYPos, 0.0f);
    glScalef(seatWidth, seatHeight, seatDepth);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Draw backrest (vertical rectangle)
    glPushMatrix();
    glTranslatef(0.0f, backrestYPos, -seatDepth / 2 + backrestThickness / 2);
    glScalef(backrestWidth, backrestHeight, backrestThickness);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Draw legs (4 vertical rectangles at corners)
    // Front left leg
    glPushMatrix();
    glTranslatef(-seatWidth / 2 + legWidth / 2, legHeight / 2, seatDepth / 2 - legDepth / 2);
    glScalef(legWidth, legHeight, legDepth);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Front right leg
    glPushMatrix();
    glTranslatef(seatWidth / 2 - legWidth / 2, legHeight / 2, seatDepth / 2 - legDepth / 2);
    glScalef(legWidth, legHeight, legDepth);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Back left leg
    glPushMatrix();
    glTranslatef(-seatWidth / 2 + legWidth / 2, legHeight / 2, -seatDepth / 2 + legDepth / 2);
    glScalef(legWidth, legHeight, legDepth);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Back right leg
    glPushMatrix();
    glTranslatef(seatWidth / 2 - legWidth / 2, legHeight / 2, -seatDepth / 2 + legDepth / 2);
    glScalef(legWidth, legHeight, legDepth);
    glutSolidCube(1.0f);
    glPopMatrix();

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