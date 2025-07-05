#include "models.h"
#include <GLUT/glut.h>
#include <cmath>

// Helper to draw a filled circle (for wheels)
void drawFilledCircle(float cx, float cy, float r, int num_segments = 32)
{
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= num_segments; ++i)
    {
        float theta = 2.0f * 3.1415926f * float(i) / float(num_segments);
        float x = r * cosf(theta);
        float y = r * sinf(theta);
        glVertex2f(cx + x, cy + y);
    }
    glEnd();
}

// 2D Modeling Coordinates: Compose a more realistic train from basic 2D shapes
void drawTrain2D(float wheelAngleL, float wheelAngleR)
{
    // Body (rectangle)
    glPushMatrix();
    glColor3f(0.2f, 0.6f, 1.0f);
    glBegin(GL_POLYGON);
    glVertex2f(0.6f, -0.2f);
    glVertex2f(-0.6f, -0.2f);
    glVertex2f(-0.6f, 0.2f);
    glVertex2f(0.6f, 0.2f);
    glEnd();
    glPopMatrix();

    // Cabin (rectangle)
    glPushMatrix();
    glColor3f(0.8f, 0.8f, 0.2f);
    glBegin(GL_POLYGON);
    glVertex2f(-0.2f, 0.2f);
    glVertex2f(-0.5f, 0.2f);
    glVertex2f(-0.5f, 0.45f);
    glVertex2f(-0.2f, 0.45f);
    glEnd();
    glPopMatrix();

    // Chimney
    glPushMatrix();
    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_POLYGON);
    glVertex2f(0.5f, 0.2f);
    glVertex2f(0.4f, 0.2f);
    glVertex2f(0.4f, 0.35f);
    glVertex2f(0.5f, 0.35f);
    glEnd();
    glPopMatrix();
    drawEllipse(0.45f, 0.37f, 0.07f, 0.02f);

    // Windows
    glPushMatrix();
    glColor3f(0.7f, 0.9f, 1.0f);
    glBegin(GL_POLYGON);
    glVertex2f(-0.25f, 0.3f);
    glVertex2f(-0.32f, 0.3f);
    glVertex2f(-0.32f, 0.4f);
    glVertex2f(-0.25f, 0.4f);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(-0.38f, 0.3f);
    glVertex2f(-0.45f, 0.3f);
    glVertex2f(-0.45f, 0.4f);
    glVertex2f(-0.38f, 0.4f);
    glEnd();
    glPopMatrix();

    // Connectors (front)
    glPushMatrix();
    glColor3f(0.5f, 0.2f, 0.1f);
    glBegin(GL_POLYGON);
    glVertex2f(0.7f, -0.08f);
    glVertex2f(0.6f, -0.08f);
    glVertex2f(0.6f, 0.08f);
    glVertex2f(0.7f, 0.08f);
    glEnd();
    glPopMatrix();
    drawEllipse(0.72f, 0.0f, 0.03f, 0.08f);

    // Wheels (left and right, with rotation)
    float wheelY = -0.22f;
    float wheelR = 0.13f;
    float wheelX1 = 0.35f, wheelX2 = -0.35f;
    int numSpokes = 8;
    // Left wheel (now on the right visually)
    glPushMatrix();
    glTranslatef(wheelX1, wheelY, 0.0f);
    glRotatef(wheelAngleL, 0, 0, 1);
    glColor3f(0.1f, 0.1f, 0.1f);
    drawFilledCircle(0, 0, wheelR);
    // Spokes (debug: yellow, thick, with a red spoke at angle 0)
    glLineWidth(6.0f);
    for (int i = 0; i < numSpokes; ++i)
    {
        float a = i * 3.14159f * 2.0f / numSpokes;
        if (i == 0)
            glColor3f(1.0f, 0.0f, 0.0f); // Red for angle 0
        else
            glColor3f(1.0f, 1.0f, 0.0f); // Yellow for others
        glBegin(GL_LINES);
        glVertex2f(0, 0);
        glVertex2f(wheelR * 0.9f * cosf(a), wheelR * 0.9f * sinf(a));
        glEnd();
    }
    glColor3f(0.8f, 0.2f, 0.2f);
    drawCircle(0, 0, wheelR);
    glPopMatrix();
    // Right wheel (now on the left visually)
    glPushMatrix();
    glTranslatef(wheelX2, wheelY, 0.0f);
    glRotatef(wheelAngleR, 0, 0, 1);
    glColor3f(0.1f, 0.1f, 0.1f);
    drawFilledCircle(0, 0, wheelR);
    // Spokes (debug: yellow, thick, with a red spoke at angle 0)
    glLineWidth(6.0f);
    for (int i = 0; i < numSpokes; ++i)
    {
        float a = i * 3.14159f * 2.0f / numSpokes;
        if (i == 0)
            glColor3f(1.0f, 0.0f, 0.0f); // Red for angle 0
        else
            glColor3f(1.0f, 1.0f, 0.0f); // Yellow for others
        glBegin(GL_LINES);
        glVertex2f(0, 0);
        glVertex2f(wheelR * 0.9f * cosf(a), wheelR * 0.9f * sinf(a));
        glEnd();
    }
    glColor3f(0.8f, 0.2f, 0.2f);
    drawCircle(0, 0, wheelR);
    glPopMatrix();
}