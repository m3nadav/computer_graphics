#define GL_SILENCE_DEPRECATION

#include "utils.h"
#include <GLUT/glut.h>
#include <cmath>

int getExitButtonX(int windowWidth, int exitButtonWidth)
{
    return windowWidth - exitButtonWidth - 20; // 20 pixels from right edge
}

void drawText(const char *text, float x, float y, float r, float g, float b, void *font)
{
    glColor3f(r, g, b);
    glRasterPos2f(x, y);
    for (const char *c = text; *c != '\0'; c++)
    {
        glutBitmapCharacter(font, *c);
    }
}

void drawCircle(float x, float y, float radius)
{
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i++)
    {
        float angle = i * 3.14159f / 180;
        glVertex2f(x + radius * cos(angle), y + radius * sin(angle));
    }
    glEnd();
}

void drawBodyTriangle(float pointyX, float pointyY, float bodyWidth, float bodyHeight, bool isPointingUp)
{
    const float baseX1 = pointyX - (bodyWidth / 2);
    const float baseX2 = pointyX + (bodyWidth / 2);
    const float baseY = isPointingUp ? pointyY - bodyHeight : pointyY + bodyHeight;
    glBegin(GL_POLYGON);
    glVertex2f(baseX1, baseY);
    glVertex2f(baseX2, baseY);
    glVertex2f(pointyX, pointyY);
    glEnd();
}

void drawRectangle(float x, float y, float length, float width)
{
    glBegin(GL_POLYGON);
    glVertex2f(x, y);
    glVertex2f(x + length, y);
    glVertex2f(x + length, y + width);
    glVertex2f(x, y + width);
    glEnd();
}
