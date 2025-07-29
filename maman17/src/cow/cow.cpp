#define GL_SILENCE_DEPRECATION // Silence deprecation warnings on macOS
#include "cow/cow.h"
#include "cow/head.h"
#include "cow/body.h"
#include "shapes/shapes.h"
#include <GLUT/glut.h>
#include <cmath>
#include <cstdlib>
#include <ctime>

void drawCow()
{
    drawBody();

    drawFullHead();
    drawLegs();

    // Draw the tail
    drawTail();
}