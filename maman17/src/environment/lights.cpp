#include "environment/lights.h"
#include "environment/environment.h"
#include <GLUT/glut.h>

// Lighting and material setup
void setupEnvironmentLighting()
{
    // Enable lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // Set up directional light (sun)
    // GLfloat lightPos[] = {10.0f, 20.0f, 10.0f, 0.0f}; // Directional light
    GLfloat lightAmbient[] = {0.3f, 0.3f, 0.3f, 1.0f};
    // GLfloat lightDiffuse[] = {0.8f, 0.8f, 0.7f, 1.0f};  // Warm sunlight
    // GLfloat lightSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f}; // Bright white specular for metallic reflections

    // glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    // glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    // glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);

    // Setup for rotating objects like the cow
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_NORMALIZE);                          // Auto-normalize normals after transformations
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE); // Light both sides of polygons
    glDisable(GL_CULL_FACE);                         // Render all polygons for proper lighting
}

// Sun lighting setup
// Sun position array for lighting calculations
static float sunPositions[4][3] = {
    {-15.0f, 25.0f, -15.0f}, // Position 0: (-x,y,-z)
    {-15.0f, 25.0f, 15.0f},  // Position 1: (-x,y,z)
    {15.0f, 25.0f, -15.0f},  // Position 2: (x,y,-z)
    {15.0f, 25.0f, 15.0f}    // Position 3: (x,y,z)
};

void setupSunLighting()
{
    // Enable lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // Get current sun position from environment module
    int currentPos = getCurrentSunPosition();
    float sunX, sunY, sunZ;

    // Use the sun positions array instead of hard-coded values
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

    // Set up point light source (sun)
    GLfloat lightPos[] = {sunX, sunY, sunZ, 1.0f}; // Point light (w=1.0)
    GLfloat lightAmbient[] = {0.3f, 0.3f, 0.3f, 1.0f};
    GLfloat lightDiffuse[] = {0.9f, 0.85f, 0.75f, 1.0f}; // Less yellow sunlight
    GLfloat lightSpecular[] = {0.8f, 0.75f, 0.7f, 1.0f};

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
}

// Draw the sun as a bright sphere
void drawSun()
{
    // Get current sun position from environment module
    int currentPos = getCurrentSunPosition();
    float sunX, sunY, sunZ;

    // Sun positions: 0: (-x,y,-z), 1: (-x,y,z), 2: (x,y,-z), 3: (x,y,z)
    switch (currentPos)
    {
    case 0:
        sunX = -15.0f;
        sunY = 25.0f;
        sunZ = -15.0f;
        break;
    case 1:
        sunX = -15.0f;
        sunY = 25.0f;
        sunZ = 15.0f;
        break;
    case 2:
        sunX = 15.0f;
        sunY = 25.0f;
        sunZ = -15.0f;
        break;
    case 3:
        sunX = 15.0f;
        sunY = 25.0f;
        sunZ = 15.0f;
        break;
    default:
        sunX = -15.0f;
        sunY = 25.0f;
        sunZ = -15.0f;
        break;
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
    glColor3f(0.6f, 0.6f, 0.6f);

    // Set shiny metal material properties (broader reflections for multi-angle visibility)
    GLfloat matAmbient[] = {0.15f, 0.15f, 0.15f, 1.0f}; // Slightly higher ambient for visibility
    GLfloat matDiffuse[] = {0.1f, 0.1f, 0.1f, 1.0f};    // Slightly higher diffuse for broader lighting
    GLfloat matSpecular[] = {0.9f, 0.9f, 0.9f, 1.0f};   // High white specular for metallic reflections
    GLfloat matShininess[] = {64.0f};                   // Reduced shininess for broader highlights

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
