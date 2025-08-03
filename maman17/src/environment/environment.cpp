#include "environment/environment.h"
#include "shapes/shapes.h"
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
        srand(programStartTime); // Use program start time for general random seed
        randomInitialized = true;
    }
}

// Set a specific seed for deterministic generation based on position/id
void setSeedForObject(float x, float y, float z, int objectType)
{
    // Ensure random is initialized
    if (!randomInitialized) {
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

// Lighting and material setup
void setupEnvironmentLighting()
{
    // Enable lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // Set up directional light (sun)
    GLfloat lightPos[] = {10.0f, 20.0f, 10.0f, 0.0f}; // Directional light
    GLfloat lightAmbient[] = {0.3f, 0.3f, 0.3f, 1.0f};
    GLfloat lightDiffuse[] = {0.8f, 0.8f, 0.7f, 1.0f}; // Warm sunlight
    GLfloat lightSpecular[] = {0.5f, 0.5f, 0.4f, 1.0f};

    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);

    // Enable color material
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
}

void setTreeMaterial()
{
    GLfloat matAmbient[] = {0.2f, 0.1f, 0.05f, 1.0f}; // Dark brown ambient
    GLfloat matDiffuse[] = {0.4f, 0.2f, 0.1f, 1.0f};  // Brown diffuse
    GLfloat matSpecular[] = {0.1f, 0.1f, 0.1f, 1.0f}; // Low specular for bark
    GLfloat matShininess[] = {10.0f};

    glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);
}

void setGrassMaterial(float colorVariation)
{
    // Base green with slight variation
    float baseGreen = 0.6f + colorVariation * 0.2f;
    float baseRed = 0.1f + colorVariation * 0.1f;
    float baseBlue = 0.2f + colorVariation * 0.1f;

    // Set color for GL_COLOR_MATERIAL mode (when enabled)
    glColor3f(baseRed, baseGreen, baseBlue);

    GLfloat matAmbient[] = {baseRed * 0.3f, baseGreen * 0.3f, baseBlue * 0.3f, 1.0f};
    GLfloat matDiffuse[] = {baseRed, baseGreen, baseBlue, 1.0f};
    GLfloat matSpecular[] = {0.2f, 0.3f, 0.2f, 1.0f};
    GLfloat matShininess[] = {20.0f};

    glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);
}

void setRockMaterial(float colorVariation)
{
    // Base gray with slight variation
    float baseGray = 0.5f + colorVariation * 0.2f;

    // Set color for GL_COLOR_MATERIAL mode (when enabled)
    glColor3f(baseGray, baseGray, baseGray);

    GLfloat matAmbient[] = {baseGray * 0.3f, baseGray * 0.3f, baseGray * 0.3f, 1.0f};
    GLfloat matDiffuse[] = {baseGray, baseGray, baseGray, 1.0f};
    GLfloat matSpecular[] = {0.1f, 0.1f, 0.1f, 1.0f}; // Low specular
    GLfloat matShininess[] = {5.0f};

    glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);
}

// Helper function to set material properties from RGB color
void setMaterialFromColor(float r, float g, float b, float shininess, float specularLevel)
{
    // Set color for GL_COLOR_MATERIAL mode (when enabled)
    glColor3f(r, g, b);

    GLfloat matAmbient[] = {r * 0.3f, g * 0.3f, b * 0.3f, 1.0f};
    GLfloat matDiffuse[] = {r, g, b, 1.0f};
    GLfloat matSpecular[] = {specularLevel, specularLevel, specularLevel, 1.0f};
    GLfloat matShininess[] = {shininess};

    glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);
}

// Cow-specific material functions
void setCowWhiteMaterial()
{
    setMaterialFromColor(0.95f, 0.95f, 0.95f, 25.0f, 0.3f);
}

void setCowLightGrayMaterial()
{
    setMaterialFromColor(0.9f, 0.9f, 0.9f, 25.0f, 0.3f);
}

void setCowHoofMaterial()
{
    setMaterialFromColor(0.2f, 0.1f, 0.1f, 5.0f, 0.1f);
}

void setCowHoofDetailMaterial()
{
    setMaterialFromColor(0.1f, 0.05f, 0.05f, 5.0f, 0.1f);
}

void setCowPinkMaterial()
{
    setMaterialFromColor(1.0f, 0.8f, 0.6f, 15.0f, 0.2f);
}

void setCowBlackMaterial()
{
    setMaterialFromColor(0.1f, 0.1f, 0.1f, 10.0f, 0.1f);
}

void setCowBeigeMaterial()
{
    setMaterialFromColor(0.85f, 0.7f, 0.6f, 15.0f, 0.2f);
}

void setCowDarkBrownMaterial()
{
    setMaterialFromColor(0.2f, 0.1f, 0.1f, 8.0f, 0.1f);
}

void setCowHornMaterial()
{
    setMaterialFromColor(0.9f, 0.9f, 0.7f, 12.0f, 0.2f);
}

// Drawing utility materials
void setGridMaterial()
{
    setMaterialFromColor(0.6f, 0.6f, 0.6f, 10.0f, 0.1f);
}

void setRedAxisMaterial()
{
    setMaterialFromColor(1.0f, 0.0f, 0.0f, 15.0f, 0.2f);
}

void setGreenAxisMaterial()
{
    setMaterialFromColor(0.0f, 1.0f, 0.0f, 15.0f, 0.2f);
}

void setBlueAxisMaterial()
{
    setMaterialFromColor(0.0f, 0.0f, 1.0f, 15.0f, 0.2f);
}

void setBlackTextMaterial()
{
    setMaterialFromColor(0.0f, 0.0f, 0.0f, 5.0f, 0.0f);
}

// Tree implementation
void drawTrunk(float height, float baseRadius, float topRadius)
{
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
}

void drawBranch(float length, float radius, int depth, float angleX, float angleY)
{
    if (depth <= 0 || length < 0.1f)
        return;

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
}

void drawLeaves(float x, float y, float z, float size)
{
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
}

void drawTree(float x, float y, float z, float scale)
{
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
}

// Enhanced meadow implementation
void drawGrassBlade(float height, float width, float bend, float colorVariation)
{
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
}

void drawProceduralMeadow(float width, float depth, int grassDensity)
{
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
}

// Rock implementation
void drawIrregularRock(float scale, int complexity, float colorVariation)
{
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
}

void drawRock(float x, float y, float z, float scale, float colorVariation)
{
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
}

void drawScatteredRocks(float x, float y, float z, int numRocks)
{
    // Set deterministic seed for consistent grass generation
    setSeedForObject(x, y, z, 2); // objectType = 3 for grass

    // Draw individual rocks
    for (int i = 0; i < numRocks; i++)
    {
        // Generate rock positions so that both rockX and rockZ can be positive or negative independently,
        // and so that rocks are distributed across the full area centered at (0,0).
        // Use the absolute values of x and z to define the extents, so sign of x/z doesn't affect the range.
        float halfX = std::abs(x) / 2.5f;
        float halfZ = std::abs(z) / 2.5f;
        float rockX = randomFloat(-halfX, halfX);
        float rockZ = randomFloat(-halfZ, halfZ);
        std::cout << "rockX: " << rockX << ", rockZ: " << rockZ << std::endl;
        float rockScale = randomFloat(0.5f, 1.5f);
        float colorVar = randomFloat(-0.2f, 0.2f);

        glPushMatrix();
        glTranslatef(rockX, 0.0f, rockZ);
        glRotatef(randomFloat(0.0f, 360.0f), 0.0f, 1.0f, 0.0f); // Random rotation

        drawIrregularRock(rockScale, 24, colorVar);

        glPopMatrix();
    }
}

// Metal material for bench
void setMetalMaterial()
{
    GLfloat matAmbient[] = {0.25f, 0.25f, 0.25f, 1.0f};              // Dark metallic ambient
    GLfloat matDiffuse[] = {0.4f, 0.4f, 0.4f, 1.0f};                 // Gray metallic diffuse
    GLfloat matSpecular[] = {0.774597f, 0.774597f, 0.774597f, 1.0f}; // High metallic specular
    GLfloat matShininess[] = {76.8f};                                // High shininess for metal

    glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);
}

// Metal bench implementation
void drawMetalBench(float x, float y, float z, float scale)
{
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
}

void drawTexturedGroundPatch(float centerX, float centerZ, float size, unsigned int textureID)
{
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
}

// World-wide ground texture
void drawWorldGround(float worldSize)
{
    static unsigned int worldGroundTextureID = 0;
    if (worldGroundTextureID == 0)
    {
        worldGroundTextureID = loadTexture("textures/dirt_texture.png");
    }

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, worldGroundTextureID);

    glPushMatrix();
    glTranslatef(0.0f, -0.01f, 0.0f); // Slightly below ground level

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
}