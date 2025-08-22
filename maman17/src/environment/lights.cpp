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

    // Setup for rotating objects
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
    {15.0f, 25.0f, 15.0f},   // Position 2: (x,y,z)
    {15.0f, 25.0f, -15.0f}   // Position 3: (x,y,-z)
};

// Light control variables
static float lightIntensity = 1.0f; // Multiplier for diffuse/specular
static float lightPositionX = 0.0f; // Custom X position
static float lightPositionY = 0.0f; // Custom Y position
static float lightPositionZ = 0.0f; // Custom Z position
static float ambientLevel = 0.3f;
static bool useCustomPosition = false; // Whether to use custom position or sunPositions array

// Lamp light control variables
static float lampIntensity = 1.5f;            // Lamp light intensity
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

    // Set up directional light source (sun) for uniform scene illumination
    // Calculate normalized direction from sun position to world center (0,0,0)
    GLfloat dirX = 0.0f - sunX; // Direction to (0,0,0)
    GLfloat dirY = 0.0f - sunY;
    GLfloat dirZ = 0.0f - sunZ;
    // Normalize direction vector
    float magnitude = sqrt(dirX * dirX + dirY * dirY + dirZ * dirZ);
    if (magnitude > 0.0f)
    {
        dirX /= magnitude;
        dirY /= magnitude;
        dirZ /= magnitude;
    }

    // Set up as directional light pointing toward world center
    GLfloat lightPos[] = {-dirX, -dirY, -dirZ, 0.0f};
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

    // Ensure sun light remains enabled and properly configured
    glEnable(GL_LIGHT0);

    // Calculate lamp position to match the visual lamp post position
    // This should match the positioning logic in drawMetalBenchAndLamp
    float cosRot = cos(currentBenchRotation * M_PI / 180.0f);
    float sinRot = sin(currentBenchRotation * M_PI / 180.0f);

    // Match the lamp positioning from drawMetalBenchAndLamp:
    // lampOffsetX = seatWidth * 0.5f + 0.175f * scale (behind and to the side)
    // lampOffsetZ = -seatDepth * 0.5f (at the back of the bench)
    float seatWidth = 2.0f; // Default scale = 1.0f
    float seatDepth = 0.5f;
    float lampOffsetX = seatWidth * 0.5f + 0.175f; // 1.175f
    float lampOffsetZ = -seatDepth * 0.5f;         // -0.25f

    // Transform the lamp offset by the bench rotation
    float lampX = currentBenchX + (lampOffsetX * cosRot - lampOffsetZ * sinRot);
    float lampY = currentBenchY + LAMP_HEIGHT - 0.2f; // Match lamp head/bulb center position
    float lampZ = currentBenchZ + (lampOffsetX * sinRot + lampOffsetZ * cosRot);

    // Calculate lamp direction to exactly match visual lamp rotation
    // Use initial direction pointing slightly forward and down (like a real lamp)
    float pitchRad = lampDirectionX * M_PI / 180.0f;
    float yawRad = -lampDirectionZ * M_PI / 180.0f; // Flip yaw sign to match visual rotation direction

    // Calculate light direction based on lamp's current rotation state
    // L/J always rotate around Y-axis, I/K always rotate around X-axis
    // Light should point based on these combined rotations from default downward direction

    // Start with default downward direction (0, -1, 0)
    // Apply the lamp's current rotations to determine where it should point

    // Method: Calculate direction vector from spherical coordinates
    // yaw (lampDirectionZ) rotates around Y-axis
    // pitch (lampDirectionX) rotates around X-axis from the downward position

    // Convert lamp angles to light direction
    // When pitch = 0, light points down: (0, -1, 0)
    // When pitch > 0, light tilts forward: (0, -cos(pitch), -sin(pitch))
    // When yaw != 0, light direction rotates around Y-axis

    float dirX = sin(yawRad) * sin(pitchRad);  // X component from yaw rotation
    float dirY = -cos(pitchRad);               // Y component - always downward when pitch=0
    float dirZ = -cos(yawRad) * sin(pitchRad); // Z component from yaw rotation

    // Set up directional spotlight for additive illumination
    // Note: OpenGL fixed-function lighting does not cast shadows - light is purely additive
    GLfloat lampPos[] = {lampX, lampY, lampZ, 1.0f}; // Point light position
    GLfloat lampDir[] = {dirX, dirY, dirZ};          // Spotlight direction

    // Configure directional spotlight with visible cone effect
    GLfloat lampAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};                                                  // No ambient (avoid washing out scene)
    GLfloat lampDiffuse[] = {lampIntensity * 0.4f, lampIntensity * 0.35f, lampIntensity * 0.2f, 1.0f}; // Warm diffuse light for visible spotlight
    GLfloat lampSpecular[] = {lampIntensity * 0.3f, lampIntensity * 0.3f, lampIntensity * 0.2f, 1.0f}; // Visible specular highlights

    glLightfv(GL_LIGHT1, GL_POSITION, lampPos);
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, lampDir);
    glLightfv(GL_LIGHT1, GL_AMBIENT, lampAmbient);   // No ambient contribution
    glLightfv(GL_LIGHT1, GL_DIFFUSE, lampDiffuse);   // Warm diffuse illumination
    glLightfv(GL_LIGHT1, GL_SPECULAR, lampSpecular); // Subtle specular highlights

    // Configure lamp as directional spotlight pointing in the calculated direction
    glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 45.0f);  // 45-degree cone for focused lamp effect
    glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 5.0f); // Moderate falloff for visible spotlight cone

    // Set moderate attenuation for realistic but not too rapid falloff
    glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.0f);
    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.1f);     // Reduced linear attenuation
    glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.05f); // Reduced quadratic attenuation

    // Ensure proper lighting state is maintained
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT1); // Ensure lamp light is enabled
    glEnable(GL_LIGHT0); // Ensure sun light stays enabled
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
    if (intensity >= 0.1f && intensity <= 10.0f)
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
