#include "models.h"
#include <GLUT/glut.h>
#include <cmath>

void drawCircle(float cx, float cy, float r, int num_segments)
{
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < num_segments; ++i)
    {
        float theta = 2.0f * 3.1415926f * float(i) / float(num_segments);
        float x = r * cosf(theta);
        float y = r * sinf(theta);
        glVertex2f(cx + x, cy + y);
    }
    glEnd();
}

void drawEllipse(float cx, float cy, float rx, float ry, int num_segments)
{
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < num_segments; ++i)
    {
        float theta = 2.0f * 3.1415926f * float(i) / float(num_segments);
        float x = rx * cosf(theta);
        float y = ry * sinf(theta);
        glVertex2f(cx + x, cy + y);
    }
    glEnd();
}

void drawSquare(float cx, float cy, float size)
{
    float half = size / 2.0f;
    glBegin(GL_LINE_LOOP);
    glVertex2f(cx - half, cy - half);
    glVertex2f(cx + half, cy - half);
    glVertex2f(cx + half, cy + half);
    glVertex2f(cx - half, cy + half);
    glEnd();
}

void drawCube(float cx, float cy, float cz, float size)
{
    float half = size / 2.0f;
    float v[8][3] = {
        {cx - half, cy - half, cz - half},
        {cx + half, cy - half, cz - half},
        {cx + half, cy + half, cz - half},
        {cx - half, cy + half, cz - half},
        {cx - half, cy - half, cz + half},
        {cx + half, cy - half, cz + half},
        {cx + half, cy + half, cz + half},
        {cx - half, cy + half, cz + half}};
    int edges[12][2] = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0}, // bottom
        {4, 5},
        {5, 6},
        {6, 7},
        {7, 4}, // top
        {0, 4},
        {1, 5},
        {2, 6},
        {3, 7} // sides
    };
    glBegin(GL_LINES);
    for (int i = 0; i < 12; ++i)
    {
        glVertex3fv(v[edges[i][0]]);
        glVertex3fv(v[edges[i][1]]);
    }
    glEnd();
}

void drawSphere(float cx, float cy, float cz, float r, int slices, int stacks)
{
    glPushMatrix();
    glTranslatef(cx, cy, cz);
    glutWireSphere(r, slices, stacks);
    glPopMatrix();
}

void drawEllipsoid(float cx, float cy, float cz, float rx, float ry, float rz, int slices, int stacks)
{
    glPushMatrix();
    glTranslatef(cx, cy, cz);
    glScalef(rx, ry, rz);
    glutWireSphere(1.0, slices, stacks);
    glPopMatrix();
}