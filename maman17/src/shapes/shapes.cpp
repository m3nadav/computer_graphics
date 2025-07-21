#include "shapes/shapes.h"
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <iostream>
#include <cmath>

void drawSphere(double radius)
{
    glutSolidSphere(radius, 32, 32);
}

void drawEllipsoid(double rx, double ry, double rz)
{
    glPushMatrix();
    glScalef(rx, ry, rz);
    glutSolidSphere(1.0, 32, 32);
    glPopMatrix();
}

void drawCylinder(double baseRadius, double topRadius, double height)
{
#ifdef GLUT_VERSION_3_7
    glutSolidCylinder(baseRadius, height, 32, 8);
#else
    GLUquadric *quad = gluNewQuadric();
    gluCylinder(quad, baseRadius, topRadius, height, 32, 8);
    gluDeleteQuadric(quad);
#endif
}

// Load texture from file
unsigned int loadTexture(const char *filename)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Create a procedural spots texture
    const int texSize = 256;
    unsigned char texture[texSize][texSize][3];

    // Create a simple spots pattern
    for (int i = 0; i < texSize; i++)
    {
        for (int j = 0; j < texSize; j++)
        {
            // White background
            texture[i][j][0] = 255;
            texture[i][j][1] = 255;
            texture[i][j][2] = 255;

            // Add some black spots
            float x = (float)i / texSize;
            float y = (float)j / texSize;

            // Create spots at various positions
            if ((x - 0.3f) * (x - 0.3f) + (y - 0.5f) * (y - 0.3f) < 0.005f ||
                (x - 0.7f) * (x - 0.7f) + (y - 0.8f) * (y - 0.8f) < 0.005f ||
                (x - 0.4f) * (x - 0.4f) + (y - 0.8f) * (y - 0.8f) < 0.005f ||
                (x - 0.8f) * (x - 0.8f) + (y - 0.2f) * (y - 0.2f) < 0.005f)
            {
                texture[i][j][0] = 0;
                texture[i][j][1] = 0;
                texture[i][j][2] = 0;
            }
        }
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texSize, texSize, 0, GL_RGB, GL_UNSIGNED_BYTE, texture);

    return textureID;
}

// Draw ellipsoid with texture mapping
void drawTexturedEllipsoid(double rx, double ry, double rz, unsigned int textureID)
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glPushMatrix();
    glScalef(rx, ry, rz);

    // Draw textured sphere (will be scaled to ellipsoid)
    int slices = 32;
    int stacks = 32;

    for (int i = 0; i < stacks; i++)
    {
        float phi1 = (float)i / stacks * M_PI;
        float phi2 = (float)(i + 1) / stacks * M_PI;

        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= slices; j++)
        {
            float theta = (float)j / slices * 2.0f * M_PI;

            float x1 = sinf(phi1) * cosf(theta);
            float y1 = sinf(phi1) * sinf(theta);
            float z1 = cosf(phi1);

            float x2 = sinf(phi2) * cosf(theta);
            float y2 = sinf(phi2) * sinf(theta);
            float z2 = cosf(phi2);

            float s = (float)j / slices;
            float t1 = (float)i / stacks;
            float t2 = (float)(i + 1) / stacks;

            glTexCoord2f(s, t1);
            glVertex3f(x1, y1, z1);

            glTexCoord2f(s, t2);
            glVertex3f(x2, y2, z2);
        }
        glEnd();
    }

    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}