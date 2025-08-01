#define GL_SILENCE_DEPRECATION // Silence deprecation warnings on macOS
#include "cow/spots.h"
#include "shapes/shapes.h"
#include <GLUT/glut.h>

// Global texture ID for cow spots
static unsigned int cowSpotsTexture = 0;
static bool textureLoaded = false;

unsigned int getCowSpotsTexture()
{
    // Load texture once if not already loaded
    if (!textureLoaded)
    {
        cowSpotsTexture = loadTexture("textures/spots.png");
        textureLoaded = true;
    }
    return cowSpotsTexture;
}