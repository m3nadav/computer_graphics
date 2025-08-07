#include "environment/lights.h"
#include "environment/environment.h"
#include <GLUT/glut.h>

// Define missing constants for separate specular color on some platforms
#ifndef GL_LIGHT_MODEL_COLOR_CONTROL
#define GL_LIGHT_MODEL_COLOR_CONTROL 0x81F8
#endif
#ifndef GL_SEPARATE_SPECULAR_COLOR
#define GL_SEPARATE_SPECULAR_COLOR 0x81FA
#endif
#ifndef GL_SINGLE_COLOR
#define GL_SINGLE_COLOR 0x81F9
#endif

// Lighting and material setup
void setupEnvironmentLighting()
{
    // Enable lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // Set up directional light (sun)
    GLfloat lightAmbient[] = {0.3f, 0.3f, 0.3f, 1.0f};

    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);

    // Setup for rotating objects like the cow
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_NORMALIZE);                          // Auto-normalize normals after transformations
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE); // Light both sides of polygons
    glDisable(GL_CULL_FACE);                         // Render all polygons for proper lighting

    // Improve specular behavior on flat geometry to look more metallic
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
    glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
}

// Sun lighting setup
// Sun position array for lighting calculations
static float sunPositions[4][3] = {
    {-15.0f, 25.0f, -15.0f}, // Position 0: (-x,y,-z)
    {-15.0f, 25.0f, 15.0f},  // Position 1: (-x,y,z)
    {15.0f, 25.0f, -15.0f},  // Position 2: (x,y,-z)
    {15.0f, 25.0f, 15.0f}    // Position 3: (x,y,z)
};

// Light control variables
static float lightIntensity = 1.0f;    // Multiplier for diffuse/specular (0.1 to 2.0)
static float lightPositionX = -15.0f;  // Custom X position
static float lightPositionY = 25.0f;   // Custom Y position
static float lightPositionZ = -15.0f;  // Custom Z position
static float ambientLevel = 0.3f;      // Ambient light level (0.0 to 1.0)
static bool useCustomPosition = false; // Whether to use custom position or sunPositions array

void setupSunLighting()
{
    // Enable lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // Determine light position
    float sunX, sunY, sunZ;

    if (useCustomPosition)
    {
        // Use custom adjustable position
        sunX = lightPositionX;
        sunY = lightPositionY;
        sunZ = lightPositionZ;
    }
    else
    {
        // Use predefined positions from 1-4 keys
        int currentPos = getCurrentSunPosition();
        if (currentPos >= 0 && currentPos < 4)
        {
            sunX = sunPositions[currentPos][0];
            sunY = sunPositions[currentPos][1];
            sunZ = sunPositions[currentPos][2];
        }
        else
        {
            // Default to position 0 if invalid
            sunX = sunPositions[0][0];
            sunY = sunPositions[0][1];
            sunZ = sunPositions[0][2];
        }
        // Update custom position variables to match current preset
        lightPositionX = sunX;
        lightPositionY = sunY;
        lightPositionZ = sunZ;
    }

    // Set up point light source (sun) with adjustable properties
    GLfloat lightPos[] = {sunX, sunY, sunZ, 1.0f}; // Point light (w=1.0)
    GLfloat lightAmbient[] = {ambientLevel, ambientLevel, ambientLevel, 1.0f};
    GLfloat lightDiffuse[] = {0.9f * lightIntensity, 0.85f * lightIntensity, 0.75f * lightIntensity, 1.0f};
    GLfloat lightSpecular[] = {0.8f * lightIntensity, 0.75f * lightIntensity, 0.7f * lightIntensity, 1.0f};

    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);

    // Setup for rotating objects like the cow
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_NORMALIZE);                          // Auto-normalize normals after transformations
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE); // Light both sides of polygons
    glDisable(GL_CULL_FACE);                         // Render all polygons for proper lighting

    // Improve specular behavior on flat geometry to look more metallic
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
    glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
}

// Draw the sun as a bright sphere
void drawSun()
{
    // Use the same position logic as setupSunLighting()
    float sunX, sunY, sunZ;

    if (useCustomPosition)
    {
        // Use custom adjustable position
        sunX = lightPositionX;
        sunY = lightPositionY;
        sunZ = lightPositionZ;
    }
    else
    {
        // Use predefined positions from 1-4 keys
        int currentPos = getCurrentSunPosition();
        if (currentPos >= 0 && currentPos < 4)
        {
            sunX = sunPositions[currentPos][0];
            sunY = sunPositions[currentPos][1];
            sunZ = sunPositions[currentPos][2];
        }
        else
        {
            // Default to position 0 if invalid
            sunX = sunPositions[0][0];
            sunY = sunPositions[0][1];
            sunZ = sunPositions[0][2];
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

// Helper function to set material properties from RGB color
void setMaterialFromColor(float r, float g, float b, float shininess, float specularLevel)
{
    // Set color for GL_COLOR_MATERIAL mode (when enabled)
    glColor3f(r, g, b);

    GLfloat matAmbient[] = {r * 0.3f, g * 0.3f, b * 0.3f, 1.0f};
    GLfloat matDiffuse[] = {r, g, b, 1.0f};
    GLfloat matSpecular[] = {specularLevel, specularLevel, specularLevel, 1.0f};
    GLfloat matShininess[] = {shininess};

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matAmbient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matDiffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpecular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, matShininess);
}

// Environment-specific materials
void setTreeMaterial()
{
    // Disable color material temporarily to override any glColor3f effects
    glDisable(GL_COLOR_MATERIAL);

    // Set brown color explicitly - this won't be overridden by GL_COLOR_MATERIAL
    glColor3f(0.35f, 0.18f, 0.10f);
    // Tree materials with high ambient and low diffuse to reduce sun influence
    GLfloat matAmbient[] = {0.35f, 0.18f, 0.10f, 1.0f};  // High brown ambient for self-lighting
    GLfloat matDiffuse[] = {0.15f, 0.08f, 0.04f, 1.0f};  // Low diffuse to reduce sun dependency
    GLfloat matSpecular[] = {0.02f, 0.02f, 0.02f, 1.0f}; // Minimal specular for bark
    GLfloat matShininess[] = {1.0f};

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matAmbient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matDiffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpecular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, matShininess);

    // Re-enable color material for other objects that might need it
    glEnable(GL_COLOR_MATERIAL);
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

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matAmbient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matDiffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpecular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, matShininess);
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

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matAmbient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matDiffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpecular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, matShininess);
}

void setMetalMaterial()
{
    // Set color for GL_COLOR_MATERIAL mode (when enabled)
    glColor3f(0.3f, 0.6f, 1.0f); // Slightly bright blue

    // More metallic: low diffuse, modest ambient, very high specular, higher shininess
    GLfloat matAmbient[] = {0.12f, 0.12f, 0.12f, 1.0f};
    GLfloat matDiffuse[] = {0.06f, 0.06f, 0.06f, 1.0f};
    GLfloat matSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat matShininess[] = {96.0f};

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matAmbient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matDiffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpecular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, matShininess);
}

// Cow-specific material functions
void setCowWhiteMaterial()
{
    glDisable(GL_LIGHTING);
    glColor3f(0.95f, 0.95f, 0.95f);
}

void setCowLightGrayMaterial()
{
    glDisable(GL_LIGHTING);
    glColor3f(0.9f, 0.9f, 0.9f);
}

void setCowHoofMaterial()
{
    glDisable(GL_LIGHTING);
    glColor3f(0.2f, 0.1f, 0.1f);
}

void setCowHoofDetailMaterial()
{
    glDisable(GL_LIGHTING);
    glColor3f(0.1f, 0.05f, 0.05f);
}

void setCowPinkMaterial()
{
    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 0.8f, 0.6f);
}

void setCowBlackMaterial()
{
    glDisable(GL_LIGHTING);
    glColor3f(0.1f, 0.1f, 0.1f);
}

void setCowBeigeMaterial()
{
    glDisable(GL_LIGHTING);
    glColor3f(0.85f, 0.7f, 0.6f);
}

void setCowDarkBrownMaterial()
{
    glDisable(GL_LIGHTING);
    glColor3f(0.2f, 0.1f, 0.1f);
}

void setCowHornMaterial()
{
    glDisable(GL_LIGHTING);
    glColor3f(0.9f, 0.9f, 0.7f);
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

// Light control functions implementation
void setLightIntensity(float intensity)
{
    if (intensity >= 0.1f && intensity <= 2.0f)
    {
        lightIntensity = intensity;
    }
}

void setLightPosition(float x, float y, float z)
{
    lightPositionX = x;
    lightPositionY = y;
    lightPositionZ = z;
    useCustomPosition = true;
}

void setAmbientLevel(float level)
{
    if (level >= 0.0f && level <= 1.0f)
    {
        ambientLevel = level;
    }
}

float getLightIntensity()
{
    return lightIntensity;
}

float getLightPositionX()
{
    return lightPositionX;
}

float getLightPositionY()
{
    return lightPositionY;
}

float getLightPositionZ()
{
    return lightPositionZ;
}

float getAmbientLevel()
{
    return ambientLevel;
}

void enableCustomLightPosition(bool enable)
{
    useCustomPosition = enable;
}
