#include "environment/lights.h"
#include "environment/environment.h"
#include <GLUT/glut.h>
#include <cmath>

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

// Configures OpenGL lighting environment with ambient light and material properties
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

// Lamp light control variables
static float lampIntensity = 1.5f;            // Lamp light intensity (0.1 to 3.0)
static float lampDirectionX = 0.0f;           // Lamp direction angle around X axis (pitch)
static float lampDirectionZ = 0.0f;           // Lamp direction angle around Z axis (yaw)
static bool lampEnabled = true;               // Whether lamp light is enabled
static const float LAMP_HEIGHT = 3.0f;        // Height of lamp above bench
static const float BACKREST_OFFSET = -0.175f; // Offset from bench center to backrest

// Current bench parameters
static float currentBenchX = -4.0f;
static float currentBenchY = 0.0f;
static float currentBenchZ = -6.0f;
static float currentBenchRotation = 30.0f;

// Establishes directional sun lighting based on current sun position settings
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

// Configures point lighting for the lamp post with directional controls
void setupLampLighting()
{
    if (!lampEnabled)
    {
        glDisable(GL_LIGHT1);
        return;
    }

    // Enable the second light source for the lamp
    glEnable(GL_LIGHT1);

    // Calculate lamp position (above the bench backrest, accounting for bench rotation)
    // Transform the backrest offset position by the bench rotation
    float cosRot = cos(currentBenchRotation * M_PI / 180.0f);
    float sinRot = sin(currentBenchRotation * M_PI / 180.0f);

    float lampX = currentBenchX + BACKREST_OFFSET * (-sinRot); // Transform backrest offset
    float lampY = currentBenchY + LAMP_HEIGHT;
    float lampZ = currentBenchZ + BACKREST_OFFSET * cosRot;

    // Calculate lamp direction based on rotation angles
    float dirX = sin(lampDirectionX * M_PI / 180.0f);
    float dirY = -cos(lampDirectionX * M_PI / 180.0f) * cos(lampDirectionZ * M_PI / 180.0f);
    float dirZ = cos(lampDirectionX * M_PI / 180.0f) * sin(lampDirectionZ * M_PI / 180.0f);

    // Set up spotlight parameters
    GLfloat lampPos[] = {lampX, lampY, lampZ, 1.0f}; // Point light position
    GLfloat lampDir[] = {dirX, dirY, dirZ};          // Spotlight direction

    // Warm white light color with high intensity
    GLfloat lampDiffuse[] = {1.0f * lampIntensity, 0.95f * lampIntensity, 0.8f * lampIntensity, 1.0f};
    GLfloat lampSpecular[] = {1.0f * lampIntensity, 0.95f * lampIntensity, 0.8f * lampIntensity, 1.0f};
    GLfloat lampAmbient[] = {0.1f, 0.1f, 0.1f, 1.0f}; // Minimal ambient contribution

    glLightfv(GL_LIGHT1, GL_POSITION, lampPos);
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, lampDir);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, lampDiffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, lampSpecular);
    glLightfv(GL_LIGHT1, GL_AMBIENT, lampAmbient);

    // Configure spotlight cone
    glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 45.0f);  // 45-degree cone
    glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 2.0f); // Moderate falloff

    // Set attenuation for realistic distance falloff
    glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.0f);
    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.1f);
    glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.02f);
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
// Applies brown bark material properties for tree rendering
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

// Sets green grass material with color variation for natural appearance
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

// Applies gray stone material with color variation for realistic rock surfaces
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

// Sets metallic material properties for bench and lamp post components
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
// Applies white cow hide material for main body parts
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

// Sets black material for cow features like nose and eyes
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

// Returns the current sun light intensity level
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

// Lamp light control functions implementation
// Adjusts the lamp post light intensity level
void setLampIntensity(float intensity)
{
    if (intensity >= 0.1f && intensity <= 3.0f)
    {
        lampIntensity = intensity;
    }
}

// Sets the lamp post directional angles for targeted lighting
void setLampDirection(float angleX, float angleZ)
{
    // Clamp pitch angle to prevent lamp from pointing too far up or down
    if (angleX < -60.0f)
        angleX = -60.0f;
    if (angleX > 30.0f)
        angleX = 30.0f;

    // Allow full 360-degree rotation for yaw
    while (angleZ < 0.0f)
        angleZ += 360.0f;
    while (angleZ >= 360.0f)
        angleZ -= 360.0f;

    lampDirectionX = angleX;
    lampDirectionZ = angleZ;
}

void getLampDirection(float *angleX, float *angleZ)
{
    if (angleX)
        *angleX = lampDirectionX;
    if (angleZ)
        *angleZ = lampDirectionZ;
}

// Returns the current lamp post intensity level
float getLampIntensity()
{
    return lampIntensity;
}

void enableLampLight(bool enable)
{
    lampEnabled = enable;
}

// Accessor functions for variables needed by moved functions
float *getSunPositions()
{
    return (float *)sunPositions;
}

bool getUseCustomPosition()
{
    return useCustomPosition;
}

// Returns the lamp's pitch angle in degrees
float getLampDirectionX()
{
    return lampDirectionX;
}

// Returns the lamp's yaw angle in degrees
float getLampDirectionZ()
{
    return lampDirectionZ;
}

float getLampHeight()
{
    return LAMP_HEIGHT;
}

void updateCurrentBenchParameters(float x, float y, float z, float rotation)
{
    currentBenchX = x;
    currentBenchY = y;
    currentBenchZ = z;
    currentBenchRotation = rotation;
}

void resetLightingToDefaults()
{
    // Reset sun lighting to defaults
    lightIntensity = 1.0f;
    lightPositionX = -15.0f;
    lightPositionY = 25.0f;
    lightPositionZ = -15.0f;
    ambientLevel = 0.3f;
    useCustomPosition = false;

    // Reset lamp lighting to defaults
    lampIntensity = 1.5f;
    lampDirectionX = 0.0f;
    lampDirectionZ = 0.0f;
    lampEnabled = true;
}
