#include "environment/environment.h"
#include "shapes/shapes.h"
#include <GLUT/glut.h>
#include <cmath>
#include <cstdlib>
#include <ctime>

// Initialize random seed
static bool randomInitialized = false;

void initializeRandom() {
    if (!randomInitialized) {
        srand(time(nullptr));
        randomInitialized = true;
    }
}

// Random number generation utilities
float randomFloat(float min, float max) {
    return min + (float)rand() / RAND_MAX * (max - min);
}

// Lighting and material setup
void setupEnvironmentLighting() {
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

void setTreeMaterial() {
    GLfloat matAmbient[] = {0.2f, 0.1f, 0.05f, 1.0f}; // Dark brown ambient
    GLfloat matDiffuse[] = {0.4f, 0.2f, 0.1f, 1.0f};   // Brown diffuse
    GLfloat matSpecular[] = {0.1f, 0.1f, 0.1f, 1.0f};  // Low specular for bark
    GLfloat matShininess[] = {10.0f};
    
    glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);
}

void setGrassMaterial(float colorVariation) {
    // Base green with slight variation
    float baseGreen = 0.6f + colorVariation * 0.2f;
    float baseRed = 0.1f + colorVariation * 0.1f;
    float baseBlue = 0.2f + colorVariation * 0.1f;
    
    GLfloat matAmbient[] = {baseRed * 0.3f, baseGreen * 0.3f, baseBlue * 0.3f, 1.0f};
    GLfloat matDiffuse[] = {baseRed, baseGreen, baseBlue, 1.0f};
    GLfloat matSpecular[] = {0.2f, 0.3f, 0.2f, 1.0f};
    GLfloat matShininess[] = {20.0f};
    
    glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);
}

void setRockMaterial() {
    GLfloat matAmbient[] = {0.3f, 0.3f, 0.3f, 1.0f};   // Gray ambient
    GLfloat matDiffuse[] = {0.5f, 0.5f, 0.45f, 1.0f};  // Slightly warm gray
    GLfloat matSpecular[] = {0.1f, 0.1f, 0.1f, 1.0f};  // Low specular
    GLfloat matShininess[] = {5.0f};
    
    glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);
}

// Tree implementation
void drawTrunk(float height, float baseRadius, float topRadius) {
    setTreeMaterial();
    
    // Create trunk with texture-like appearance using multiple cylinders
    int segments = 20;
    float segmentHeight = height / segments;
    
    for (int i = 0; i < segments; i++) {
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
        for (int j = 0; j <= 16; j++) {
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

void drawBranch(float length, float radius, int depth, float angleX, float angleY) {
    if (depth <= 0 || length < 0.1f) return;
    
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
    if (depth == 1) {
        drawLeaves(0.0f, 0.0f, 0.0f, radius * 3.0f);
    } else {
        // Create sub-branches
        int numSubBranches = 2 + rand() % 3;
        for (int i = 0; i < numSubBranches; i++) {
            float subAngleX = randomFloat(-45.0f, 45.0f);
            float subAngleY = randomFloat(0.0f, 360.0f);
            float subLength = length * randomFloat(0.6f, 0.8f);
            float subRadius = radius * 0.7f;
            
            drawBranch(subLength, subRadius, depth - 1, subAngleX, subAngleY);
        }
    }
    
    glPopMatrix();
}

void drawLeaves(float x, float y, float z, float size) {
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
    for (int i = 0; i < numLeaves; i++) {
        glPushMatrix();
        
        // Random position around center
        float leafX = x + randomFloat(-size, size);
        float leafY = y + randomFloat(-size * 0.5f, size * 0.5f);
        float leafZ = z + randomFloat(-size, size);
        
        glTranslatef(leafX, leafY, leafZ);
        
        // Vary leaf color slightly
        float colorVar = randomFloat(-0.1f, 0.1f);
        glColor3f(0.2f + colorVar, 0.8f + colorVar, 0.2f + colorVar);
        
        // Small leaf sphere
        glutSolidSphere(size * 0.15f, 6, 6);
        
        glPopMatrix();
    }
}

void drawTree(float x, float y, float z, float scale) {
    initializeRandom();
    
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
    for (int i = 0; i < numMainBranches; i++) {
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
void drawGrassBlade(float height, float width, float bend, float colorVariation) {
    setGrassMaterial(colorVariation);
    
    // Create a single grass blade using triangular strips
    glBegin(GL_TRIANGLE_STRIP);
    
    int segments = 5;
    for (int i = 0; i <= segments; i++) {
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

void drawProceduralMeadow(float width, float depth, int grassDensity) {
    initializeRandom();
    
    // Draw base ground
    setGrassMaterial(0.0f);
    glColor3f(0.15f, 0.4f, 0.15f); // Dark green base
    glPushMatrix();
    glTranslatef(0.0f, -0.05f, 0.0f);
    glScalef(width, 0.1f, depth);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    // Draw individual grass blades
    for (int i = 0; i < grassDensity; i++) {
        float grassX = randomFloat(-width/2, width/2);
        float grassZ = randomFloat(-depth/2, depth/2);
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
void drawIrregularRock(float scale, int complexity) {
    setRockMaterial();
    
    // Create irregular rock using distorted sphere vertices
    glBegin(GL_TRIANGLES);
    
    // Generate vertices on a sphere and distort them
    int rings = complexity / 2;
    int sectors = complexity;
    
    for (int r = 0; r < rings; r++) {
        for (int s = 0; s < sectors; s++) {
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
            float len1 = sqrt(nx1*nx1 + ny1*ny1 + nz1*nz1);
            nx1 /= len1; ny1 /= len1; nz1 /= len1;
            
            glNormal3f(nx1, ny1, nz1);
            glVertex3f(x1, y1, z1);
            glVertex3f(x2, y2, z2);
            glVertex3f(x3, y3, z3);
            
            // Second triangle (2, 4, 3)
            float nx2 = (y4 - y2) * (z3 - z2) - (z4 - z2) * (y3 - y2);
            float ny2 = (z4 - z2) * (x3 - x2) - (x4 - x2) * (z3 - z2);
            float nz2 = (x4 - x2) * (y3 - y2) - (y4 - y2) * (x3 - x2);
            float len2 = sqrt(nx2*nx2 + ny2*ny2 + nz2*nz2);
            nx2 /= len2; ny2 /= len2; nz2 /= len2;
            
            glNormal3f(nx2, ny2, nz2);
            glVertex3f(x2, y2, z2);
            glVertex3f(x4, y4, z4);
            glVertex3f(x3, y3, z3);
        }
    }
    
    glEnd();
}

void drawRock(float x, float y, float z, float scale) {
    initializeRandom();
    
    glPushMatrix();
    glTranslatef(x, y, z);
    
    // Add some random rotation for natural placement
    glRotatef(randomFloat(0.0f, 360.0f), 0.0f, 1.0f, 0.0f);
    glRotatef(randomFloat(-10.0f, 10.0f), 1.0f, 0.0f, 0.0f);
    
    // Scale with slight random variation
    float scaleVar = scale * randomFloat(0.8f, 1.2f);
    glScalef(scaleVar, scaleVar * 0.7f, scaleVar); // Slightly flattened
    
    drawIrregularRock(1.0f, 24);
    
    glPopMatrix();
}