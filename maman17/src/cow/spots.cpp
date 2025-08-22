#include "cow/spots.h"
#include "shapes/shapes.h"
#include <GLUT/glut.h>
#include <cmath>
#include <vector>
#include <iostream>

// Global texture ID for cow spots
static unsigned int cowSpotsTexture = 0;
static bool textureGenerated = false;

// Creates a procedural texture with realistic cow spot patterns for body texturing
unsigned int generateCowSpotsTexture()
{
    const int texSize = 512; // High resolution for smooth spots
    std::vector<unsigned char> texture(texSize * texSize * 3);

    // Define spots with fixed visual proportions
    struct SpotDef
    {
        float cx, cy, size;
    };

    SpotDef spots[] = {
        {0.2f, 0.3f, 0.09075f},
        {0.7f, 0.6f, 0.0726f},
        {0.4f, 0.8f, 0.05445f},
        {0.8f, 0.2f, 0.045375f},
        {0.5f, 0.1f, 0.063525f},
        {0.1f, 0.7f, 0.081675f},
        {0.6f, 0.45f, 0.05445f},
        {0.15f, 0.9f, 0.045375f}};

    // Generate texture
    for (int y = 0; y < texSize; y++)
    {
        for (int x = 0; x < texSize; x++)
        {
            int idx = (y * texSize + x) * 3;

            // Normalize coordinates to 0-1 range
            float fx = (float)x / texSize;
            float fy = (float)y / texSize;

            // Create spots with consistent visual size regardless of texture resolution
            bool spot = false;

            for (int i = 0; i < 8; i++)
            {
                float dx = fx - spots[i].cx;
                float dy = fy - spots[i].cy;
                float dist = dx * dx + dy * dy;
                float radius = spots[i].size;

                if (dist < radius * radius)
                {
                    spot = true;
                    break;
                }
            }

            if (spot)
            {
                // Black spots
                texture[idx] = 0;     // R
                texture[idx + 1] = 0; // G
                texture[idx + 2] = 0; // B
            }
            else
            {
                // White background
                texture[idx] = 255;     // R
                texture[idx + 1] = 255; // G
                texture[idx + 2] = 255; // B
            }
        }
    }

    // Create OpenGL texture
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Upload texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texSize, texSize, 0, GL_RGB, GL_UNSIGNED_BYTE, texture.data());

    return textureID;
}

// Returns the cached cow spots texture, generating it if not already created
unsigned int getCowSpotsTexture()
{
    // Generate texture only once
    if (!textureGenerated)
    {
        cowSpotsTexture = generateCowSpotsTexture();
        textureGenerated = true;
        std::cout << "Generated procedural cow spots texture" << std::endl;
    }
    return cowSpotsTexture;
}