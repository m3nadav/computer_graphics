#include "models.h"
#include <GLUT/glut.h>
#include <cmath>

// Helper to draw a cylinder (chimney)
void drawCylinder(float base, float top, float height, int slices = 16)
{
    GLUquadric *quad = gluNewQuadric();
    gluCylinder(quad, base, top, height, slices, 1);
    gluDeleteQuadric(quad);
}

// 3D Modeling Coordinates: Compose a more realistic train from basic 3D shapes
void drawTrain3D(float wheelAngleL = 0.0f, float wheelAngleR = 0.0f)
{
    // Body (cube)
    glPushMatrix();
    glColor3f(0.2f, 0.6f, 1.0f);
    glScalef(1.2f, 0.4f, 0.5f);
    drawCube(0.0f, 0.0f, 0.0f, 1.0f);
    glPopMatrix();

    // Cabin (cube)
    glPushMatrix();
    glTranslatef(-0.35f, 0.3f, 0.0f);
    glScalef(0.5f, 0.4f, 0.5f);
    glColor3f(0.8f, 0.8f, 0.2f);
    drawCube(0.0f, 0.0f, 0.0f, 1.0f);
    glPopMatrix();

    // Chimney (cylinder)
    glPushMatrix();
    glTranslatef(0.5f, 0.2f, 0.0f);
    glRotatef(-90, 0, 1, 0);
    glColor3f(0.3f, 0.3f, 0.3f);
    drawCylinder(0.08f, 0.06f, 0.25f);
    glPopMatrix();
    // Chimney top (ellipse)
    glPushMatrix();
    glTranslatef(0.75f, 0.2f, 0.0f);
    glRotatef(90, 0, 1, 0);
    glColor3f(0.3f, 0.3f, 0.3f);
    drawEllipse(0, 0, 0.08f, 0.06f);
    glPopMatrix();

    // Windows (front and side)
    glPushMatrix();
    glTranslatef(-0.35f, 0.35f, 0.13f);
    glScalef(0.12f, 0.12f, 1.0f);
    glColor3f(0.7f, 0.9f, 1.0f);
    drawCube(0.0f, 0.0f, 0.0f, 1.0f);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(-0.35f, 0.35f, -0.13f);
    glScalef(0.12f, 0.12f, 1.0f);
    glColor3f(0.7f, 0.9f, 1.0f);
    drawCube(0.0f, 0.0f, 0.0f, 1.0f);
    glPopMatrix();

    // Connector (front)
    glPushMatrix();
    glTranslatef(0.7f, -0.05f, 0.0f);
    glScalef(0.15f, 0.08f, 0.2f);
    glColor3f(0.5f, 0.2f, 0.1f);
    drawCube(0.0f, 0.0f, 0.0f, 1.0f);
    glPopMatrix();
    // Connector ellipse
    glPushMatrix();
    glTranslatef(0.8f, -0.05f, 0.0f);
    glScalef(1.0f, 1.0f, 0.5f);
    glColor3f(0.5f, 0.2f, 0.1f);
    drawEllipse(0, 0, 0.05f, 0.12f);
    glPopMatrix();

    // Wheels (left and right, with rotation)
    float wheelY = -0.25f;
    float wheelR = 0.15f;
    float wheelX1 = 0.35f, wheelX2 = -0.35f;
    float wheelZ1 = 0.18f, wheelZ2 = -0.18f;
    int numSpokes = 8;
    // Right wheel (front)
    glPushMatrix();
    glTranslatef(wheelX1, wheelY, wheelZ1);
    glRotatef(wheelAngleR, 0, 0, 1);
    glColor3f(0.1f, 0.1f, 0.1f);
    drawSphere(0, 0, 0, wheelR);
    glColor3f(0.8f, 0.2f, 0.2f);
    drawCircle(0, 0, wheelR);
    // Spokes
    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(2.0f);
    for (int i = 0; i < numSpokes; ++i)
    {
        float a = i * 3.14159f * 2.0f / numSpokes;
        glBegin(GL_LINES);
        glVertex3f(0, 0, 0);
        glVertex3f(wheelR * cosf(a), wheelR * sinf(a), 0);
        glEnd();
    }
    glPopMatrix();
    // Left wheel (back)
    glPushMatrix();
    glTranslatef(wheelX2, wheelY, wheelZ1);
    glRotatef(wheelAngleL, 0, 0, 1);
    glColor3f(0.1f, 0.1f, 0.1f);
    drawSphere(0, 0, 0, wheelR);
    glColor3f(0.8f, 0.2f, 0.2f);
    drawCircle(0, 0, wheelR);
    // Spokes
    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(2.0f);
    for (int i = 0; i < numSpokes; ++i)
    {
        float a = i * 3.14159f * 2.0f / numSpokes;
        glBegin(GL_LINES);
        glVertex3f(0, 0, 0);
        glVertex3f(wheelR * cosf(a), wheelR * sinf(a), 0);
        glEnd();
    }
    glPopMatrix();
    // Right wheel (front, other side)
    glPushMatrix();
    glTranslatef(wheelX1, wheelY, wheelZ2);
    glRotatef(wheelAngleR, 0, 0, 1);
    glColor3f(0.1f, 0.1f, 0.1f);
    drawSphere(0, 0, 0, wheelR);
    glColor3f(0.8f, 0.2f, 0.2f);
    drawCircle(0, 0, wheelR);
    // Spokes
    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(2.0f);
    for (int i = 0; i < numSpokes; ++i)
    {
        float a = i * 3.14159f * 2.0f / numSpokes;
        glBegin(GL_LINES);
        glVertex3f(0, 0, 0);
        glVertex3f(wheelR * cosf(a), wheelR * sinf(a), 0);
        glEnd();
    }
    glPopMatrix();
    // Left wheel (back, other side)
    glPushMatrix();
    glTranslatef(wheelX2, wheelY, wheelZ2);
    glRotatef(wheelAngleL, 0, 0, 1);
    glColor3f(0.1f, 0.1f, 0.1f);
    drawSphere(0, 0, 0, wheelR);
    glColor3f(0.8f, 0.2f, 0.2f);
    drawCircle(0, 0, wheelR);
    // Spokes
    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(2.0f);
    for (int i = 0; i < numSpokes; ++i)
    {
        float a = i * 3.14159f * 2.0f / numSpokes;
        glBegin(GL_LINES);
        glVertex3f(0, 0, 0);
        glVertex3f(wheelR * cosf(a), wheelR * sinf(a), 0);
        glEnd();
    }
    glPopMatrix();
}