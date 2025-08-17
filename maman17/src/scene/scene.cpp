#include "scene/scene.h"
#include "cow/cow.h"
#include "environment/environment.h"
#include "environment/lights.h"
#include <GLUT/glut.h>

// Global animation counter
static int animationFrame = 0;

/**
 * Creates a simple flat green ground plane as the base meadow surface.
 * Provides the foundational terrain element that other scene components
 * are built upon, using basic OpenGL geometry with green coloring.
 */
void drawMeadow()
{
    setMaterialFromColor(0.2f, 0.8f, 0.2f, 15.0f, 0.2f); // Green meadow material
    glPushMatrix();
    glTranslatef(0.0f, -1.0f, 0.0f);
    glScalef(50.0f, 0.1f, 50.0f); // Large flat ground
    glutSolidCube(1.0);
    glPopMatrix();
}

/** Returns the current global animation frame counter for time-based animations. */
int getAnimationFrame()
{
    return animationFrame;
}

/** Advances the global animation frame counter to drive time-based scene animations. */
void incrementAnimationFrame()
{
    animationFrame++;
}
