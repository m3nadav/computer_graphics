#include "scene/scene.h"
#include "cow/cow.h"
#include "environment/environment.h"
#include "environment/lights.h"
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
