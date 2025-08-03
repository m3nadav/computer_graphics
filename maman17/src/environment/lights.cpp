#include "environment/lights.h"
#include <GLUT/glut.h>

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

    // Setup for rotating objects like the cow
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_NORMALIZE);                          // Auto-normalize normals after transformations
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE); // Light both sides of polygons
    glDisable(GL_CULL_FACE);                         // Render all polygons for proper lighting
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
    GLfloat matAmbient[] = {0.2f, 0.1f, 0.05f, 1.0f}; // Dark brown ambient
    GLfloat matDiffuse[] = {0.4f, 0.2f, 0.1f, 1.0f};  // Brown diffuse
    GLfloat matSpecular[] = {0.1f, 0.1f, 0.1f, 1.0f}; // Low specular for bark
    GLfloat matShininess[] = {10.0f};

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matAmbient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matDiffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpecular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, matShininess);
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
    glColor3f(0.4f, 0.4f, 0.4f);

    // Set metal material properties
    GLfloat matAmbient[] = {0.25f, 0.25f, 0.25f, 1.0f};              // Dark metallic ambient
    GLfloat matDiffuse[] = {0.4f, 0.4f, 0.4f, 1.0f};                 // Gray metallic diffuse
    GLfloat matSpecular[] = {0.774597f, 0.774597f, 0.774597f, 1.0f}; // High metallic specular
    GLfloat matShininess[] = {76.8f};                                // High shininess for metal

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
