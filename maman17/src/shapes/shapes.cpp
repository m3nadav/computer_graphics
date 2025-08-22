#include "shapes/shapes.h"
#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>
#include <cstring>
// Include stb_image.h for image loading
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Renders a solid sphere with the specified radius using OpenGL primitives
void drawSphere(double radius)
{
    glutSolidSphere(radius, 32, 32);
}

// Creates an ellipsoid by scaling a unit sphere along each axis with given radii
void drawEllipsoid(double rx, double ry, double rz)
{
    glPushMatrix();
    glScalef(rx, ry, rz);
    glutSolidSphere(1.0, 32, 32);
    glPopMatrix();
}

// Draws a cylinder or cone with specified base radius, top radius, and height
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

/**
 * Loads image data from PNG files using the stb_image library for texture mapping.
 * Converts various pixel formats (RGB, RGBA, grayscale) to a consistent RGB format
 * suitable for OpenGL texture upload. Handles format conversion and provides
 * detailed error reporting for debugging texture loading issues.
 */
bool loadImageFile(const char *filename, std::vector<unsigned char> &image, int &width, int &height)
{
    int channels;
    unsigned char *data = stbi_load(filename, &width, &height, &channels, 0);

    if (!data)
    {
        std::cerr << "Failed to load image: " << filename << " - " << stbi_failure_reason() << std::endl;
        return false;
    }

    std::cout << "Loaded PNG file: " << filename << " (" << width << "x" << height << ", " << channels << " channels)" << std::endl;

    // Convert to RGB format (3 channels)
    image.resize(width * height * 3);

    if (channels == 3)
    {
        // Already RGB, just copy
        memcpy(image.data(), data, width * height * 3);
    }
    else if (channels == 4)
    {
        // RGBA, convert to RGB by skipping alpha
        for (int i = 0; i < width * height; i++)
        {
            image[i * 3 + 0] = data[i * 4 + 0]; // R
            image[i * 3 + 1] = data[i * 4 + 1]; // G
            image[i * 3 + 2] = data[i * 4 + 2]; // B
        }
    }
    else if (channels == 1)
    {
        // Grayscale, duplicate to RGB
        for (int i = 0; i < width * height; i++)
        {
            image[i * 3 + 0] = data[i]; // R
            image[i * 3 + 1] = data[i]; // G
            image[i * 3 + 2] = data[i]; // B
        }
    }

    stbi_image_free(data);
    return true;
}

/**
 * Creates an OpenGL texture from an image file with proper filtering and wrapping.
 * Generates texture IDs, configures texture parameters for optimal rendering quality,
 * and uploads image data to GPU memory. Essential for applying realistic surface
 * materials to 3D geometry throughout the scene.
 */
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

    // Try to load PNG file
    std::vector<unsigned char> imageData;
    int width, height;

    if (loadImageFile(filename, imageData, width, height))
    {
        // Use loaded PNG data
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData.data());
        std::cout << "Loaded texture from file: " << filename << std::endl;
    }

    return textureID;
}

/**
 * Renders a textured ellipsoid with proper UV coordinate mapping and optional mirroring.
 * Combines geometric scaling with texture application to create realistic textured surfaces.
 * Supports texture coordinate manipulation for specialized effects like mirrored textures
 * on symmetric geometry such as cow body parts.
 */
void drawTexturedEllipsoid(double rx, double ry, double rz, unsigned int textureID, bool mirrored)
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

            // Mirror vertically (flip t coordinates) if requested
            if (mirrored)
            {
                t1 = 1.0f - t1;
                t2 = 1.0f - t2;
            }

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