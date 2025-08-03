#include "scene/scene.h"
#include "cow/cow.h"
#include "environment/environment.h"
#include <GLUT/glut.h>

// Global animation counter
static int animationFrame = 0;

void drawMeadow()
{
    setMaterialFromColor(0.2f, 0.8f, 0.2f, 15.0f, 0.2f); // Green meadow material
    glPushMatrix();
    glTranslatef(0.0f, -1.0f, 0.0f);
    glScalef(50.0f, 0.1f, 50.0f); // Large flat ground
    glutSolidCube(1.0);
    glPopMatrix();
}

int getAnimationFrame()
{
    return animationFrame;
}

void incrementAnimationFrame()
{
    animationFrame++;
}

void drawEnhancedScene()
{
    glLoadIdentity();
    gluLookAt(0.0, 8.0, 20.0, // Eye position - moved back and up for better view
              0.0, 0.0, 0.0,  // Look at
              0.0, 1.0, 0.0); // Up

    // Setup lighting for realistic rendering
    setupEnvironmentLighting();

    // Draw world-wide ground texture
    drawWorldGround(100.0f);

    // Draw enhanced procedural meadow (grass only, no ground base)
    drawProceduralMeadow(50.0f, 50.0f, 2000);

    // Draw multiple trees at different positions
    drawTree(-8.0f, 0.0f, -5.0f, 1.2f);
    drawTree(12.0f, 0.0f, -8.0f, 0.9f);
    drawTree(-15.0f, 0.0f, 10.0f, 1.1f);
    drawTree(8.0f, 0.0f, 12.0f, 1.0f);
    drawTree(-3.0f, 0.0f, -15.0f, 0.8f);

    // Draw rocks scattered around
    drawRock(-5.0f, 0.0f, 8.0f, 1.0f);
    drawRock(10.0f, 0.0f, 3.0f, 0.7f);
    drawRock(-12.0f, 0.0f, -2.0f, 1.3f);
    drawRock(15.0f, 0.0f, -12.0f, 0.9f);
    drawRock(2.0f, 0.0f, 18.0f, 1.1f);

    // Draw the cow in the center
    drawCow();
}

void drawScene()
{
    glLoadIdentity();
    gluLookAt(0.0, 5.0, 15.0, // Eye position
              0.0, 0.0, 0.0,  // Look at
              0.0, 1.0, 0.0); // Up

    // Draw world-wide ground texture instead of colored meadow
    drawWorldGround(100.0f);

    drawCow();
}